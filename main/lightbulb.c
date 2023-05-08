/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/* HomeKit Lightbulb Example Dummy Implementation
 * Refer ESP IDF docs for LED driver implementation:
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/rmt.html
*/

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

static const char *TAG = "lightbulb";

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL_G          LEDC_CHANNEL_0
#define LEDC_CHANNEL_R          LEDC_CHANNEL_1
#define LEDC_CHANNEL_B          LEDC_CHANNEL_2
#define LEDC_CHANNEL_W          LEDC_CHANNEL_3
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz

#define G_GPIO GPIO_NUM_9
#define R_GPIO GPIO_NUM_11
#define B_GPIO GPIO_NUM_12
#define W_GPIO GPIO_NUM_38

typedef struct {
    float r;
    float g;
    float b;
} rgb;

typedef struct {
    float h;
    float s;
    float v;
} hsv;

hsv bulb = {
        .h = 0,
        .s = 0.0,
        .v = 1.0
};

bool rgb_lightbulb_switch = true;

bool mono_lightbulb_switch = true;

void rgb_lightbulb_update();
void mono_lightbulb_update();

rgb hsv2rgb(hsv);
float br = 0.0;

/**
 * @brief initialize the lightbulb lowlevel module
 */
void rgb_lightbulb_init(void)
{

    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t g_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_G,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = G_GPIO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&g_channel));

    ledc_channel_config_t r_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_R,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = R_GPIO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&r_channel));

    ledc_channel_config_t b_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_B,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = B_GPIO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&b_channel));

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_G, LEDC_DUTY));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_R, LEDC_DUTY));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_B, LEDC_DUTY));

    ESP_LOGI(TAG, "Dummy Light Driver Init.");
}

void mono_lightbulb_init(void)
{

    // Prepare and then apply the LEDC PWM timer configuration
    // ledc_timer_config_t ledc_timer = {
    //     .speed_mode       = LEDC_MODE,
    //     .timer_num        = LEDC_TIMER,
    //     .duty_resolution  = LEDC_DUTY_RES,
    //     .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
    //     .clk_cfg          = LEDC_AUTO_CLK
    // };
    // ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t w_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_W,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = W_GPIO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&w_channel));

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_W, LEDC_DUTY));

    ESP_LOGI(TAG, "Dummy Light Driver Init.");
}

/**
 * @brief turn on/off the lowlevel lightbulb
 */
int rgb_lightbulb_set_on(bool value)
{
    rgb_lightbulb_switch = value;
    rgb_lightbulb_update();
    ESP_LOGI(TAG, "lightbulb_set_on : %s", value == true ? "true" : "false");
    return 0;
}

int mono_lightbulb_set_on(bool value)
{
    mono_lightbulb_switch = value;
    mono_lightbulb_update();
    ESP_LOGI(TAG, "lightbulb_set_on : %s", value == true ? "true" : "false");
    return 0;
}

/**
 * @brief set the saturation of the lowlevel lightbulb
 */
int rgb_lightbulb_set_saturation(float value)
{
    rgb_lightbulb_switch = true;
    bulb.s = value;
    rgb_lightbulb_update();
    ESP_LOGI(TAG, "lightbulb_set_saturation : %f", value);
    return 0;
}

/**
 * @brief set the hue of the lowlevel lightbulb
 */
int rgb_lightbulb_set_hue(float value)
{
    rgb_lightbulb_switch = true;
    bulb.h = value;
    rgb_lightbulb_update();
    ESP_LOGI(TAG, "lightbulb_set_hue : %f", value);
    return 0;
}

/**
 * @brief set the brightness of the lowlevel lightbulb
 */
int rgb_lightbulb_set_brightness(int value)
{
    rgb_lightbulb_switch = true;
    bulb.v = value;
    rgb_lightbulb_update();
    ESP_LOGI(TAG, "lightbulb_set_brightness : %d", value);
    return 0;
}

int mono_lightbulb_set_brightness(int value)
{
    mono_lightbulb_switch = true;
    br = 0.01*value;
    mono_lightbulb_update();
    ESP_LOGI(TAG, "lightbulb_set_brightness : %d", value);
    return 0;
}

rgb hsv2rgb(hsv hsv){
    float h = hsv.h;
    float s = 0.01*hsv.s;
    float v = 0.01*hsv.v;
    float r = 0, g = 0, b = 0;
    if (s == 0){
		r = v;
		g = v;
		b = v;
	}else{
		int i;
		float f, p, q, t;

		if (h == 360)
			h = 0;
		else
			h = h / 60;

		i = (int)trunc(h);
		f = h - i;

		p = v * (1.0 - s);
		q = v * (1.0 - (s* f));
		t = v * (1.0 - (s* (1.0 - f)));

		switch (i)
		{
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:
			r = v;
			g = p;
			b = q;
			break;
		}
	}

	rgb ret;
	ret.r = r;
	ret.g = g;
	ret.b = b;

	return ret;
}

void rgb_lightbulb_update()
{
    ESP_LOGD(TAG, "Updating RGB lightbulb");
    if (rgb_lightbulb_switch){
        rgb rgb = hsv2rgb(bulb);
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_G, 8191*rgb.g);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_G);
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_R, 8191*rgb.r);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_R);
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_B, 8191*rgb.b);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_B);
        ESP_LOGD(TAG, "RGB value set to: %f, %f, %f", rgb.r, rgb.g, rgb.b);
    } else {
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_G, 0);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_G);
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_R, 0);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_R);
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_B, 0);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_B);
        ESP_LOGD(TAG, "RGB value set to: %f, %f, %f", 0.0, 0.0, 0.0);
    }
}

void mono_lightbulb_update()
{
    ESP_LOGD(TAG, "Updating monochrome lightbulb");
    if (mono_lightbulb_switch){
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_W, 8191*br);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_W);
        ESP_LOGD(TAG, "Brightness value set to: %f", br);
    } else {
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_W, 0);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_W);
        ESP_LOGD(TAG, "Brightness value set to: %f", 0.0);
    }
}