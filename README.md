# ESP32 HomeKit Lightbulb
Example of controlling an RGB LED and a monochrome LED connected to ESP32 through HomeKit.

## How to use
Clone this repo under `examples` directory of [esp-homekit-sdk](https://github.com/espressif/esp-homekit-sdk).

For the first time of flashing the firmware, use [ESP SoftAP Prov](https://apps.apple.com/us/app/esp-softap-provisioning/id1474040630) to scan the generated QR code showing in the terminal to do the Wi-Fi Provisioning.

Use Home app or camera app to scan the QR code showing in the terminal to add the accessory to Home.

## Customization
### Pin definition
Pins of RGB LED are defined as `R_GPIO`, `G_GPIO` and `B_GPIO` in `lightbulb.c`. 

Pin of monochrome LED is defined as `W_GPIO` in `lightbulb.c`.

Pin of reset is defined as `RESET_GPIO` in `app_main.c`.

### Service definition
One service controls a single LED and is created by function `hap_serv_lightbulb_create` in `app_main.c`. 

### Accessory definition
Information of accessory is defined as a `hap_acc_cfg_t` structure in `app_main.c`.