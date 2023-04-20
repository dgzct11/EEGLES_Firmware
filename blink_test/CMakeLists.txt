cmake_minimum_required(VERSION 3.25)
include ($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)


project(blink_test)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${PROJECT_NAME}
	blink.c
)




