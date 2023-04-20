# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/David Zaha/Desktop/2022 school year/YWISE/pico-sdk/tools/pioasm"
  "C:/DavidZaha/Programming/Github/EEGLES_Firmware/blink_test/build/pioasm"
  "C:/DavidZaha/Programming/Github/EEGLES_Firmware/blink_test/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm"
  "C:/DavidZaha/Programming/Github/EEGLES_Firmware/blink_test/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "C:/DavidZaha/Programming/Github/EEGLES_Firmware/blink_test/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp"
  "C:/DavidZaha/Programming/Github/EEGLES_Firmware/blink_test/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "C:/DavidZaha/Programming/Github/EEGLES_Firmware/blink_test/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/DavidZaha/Programming/Github/EEGLES_Firmware/blink_test/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/DavidZaha/Programming/Github/EEGLES_Firmware/blink_test/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
