cmake_minimum_required(VERSION 3.25)
include ($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)


project(TalonFirmware)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${PROJECT_NAME}
	main.c
)

add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)
pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
	pico_stdlib
	pico_multicore
	hardware_i2c
	hardware_spi
	hardware_pio
	hardware_uart
	FatFs_SPI
)