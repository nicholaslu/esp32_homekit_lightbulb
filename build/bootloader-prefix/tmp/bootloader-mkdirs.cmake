# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/uiuan/esp/esp-idf/components/bootloader/subproject"
  "/Users/uiuan/dev/esp-homekit-sdk/examples/esp32_homekit_lightbulb/build/bootloader"
  "/Users/uiuan/dev/esp-homekit-sdk/examples/esp32_homekit_lightbulb/build/bootloader-prefix"
  "/Users/uiuan/dev/esp-homekit-sdk/examples/esp32_homekit_lightbulb/build/bootloader-prefix/tmp"
  "/Users/uiuan/dev/esp-homekit-sdk/examples/esp32_homekit_lightbulb/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/uiuan/dev/esp-homekit-sdk/examples/esp32_homekit_lightbulb/build/bootloader-prefix/src"
  "/Users/uiuan/dev/esp-homekit-sdk/examples/esp32_homekit_lightbulb/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/uiuan/dev/esp-homekit-sdk/examples/esp32_homekit_lightbulb/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/uiuan/dev/esp-homekit-sdk/examples/esp32_homekit_lightbulb/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
