# STM32 Minimal CMake project for C/C++ projects
cmake_minimum_required(VERSION 3.5)
###############################################################################

include(./gcc-arm-none-eabi.cmake)

# Set project name and some defaults
if ("${PROJECT_NAME}" STREQUAL "")
    set(PROJECT_NAME Probe)
endif()
project(${PROJECT_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set debug mode as default
if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

# Set microcontroller information
set(MCU_FAMILY STM32L4xx)
set(MCU_MODEL STM32L432xx)
set(CPU_PARAMETERS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard)

set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32l432xx.s)
set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32L432KCUx_FLASH.ld)

###############################################################################
set(EXECUTABLE ${CMAKE_PROJECT_NAME})

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
set(CMAKE_CXX_EXTENSIONS ON)
###############################################################################

set(STM32CUBEMX_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include)

set(PROJECT_INCLUDE_DIRECTORIES 
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/Core/Cam
)

set (TINY_USB_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyusb/src
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyusb/src/class/cdc
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyusb/src/class/msc
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyusb/src/device
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyusb/hw
    )

file(GLOB_RECURSE STM32CUBEMX_SOURCES CONFIGURE_DEPENDS
${CMAKE_CURRENT_SOURCE_DIR}/Core/Cam/*.c
${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/*.c
${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/*.c
${CMAKE_CURRENT_SOURCE_DIR}/Drivers/*.c)


set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src)
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    ${PROJECT_DIR}/*.cpp
    ${PROJECT_DIR}/*.c)

set(TINY_USB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tinyusb/src)
file(GLOB_RECURSE TINY_USB_SOURCES CONFIGURE_DEPENDS
    ${TINY_USB_DIR}/*.cpp
    ${TINY_USB_DIR}/*.c
    ${TINY_USB_DIR}/class/cdc/*.c
    ${TINY_USB_DIR}/class/msc/*.c
    ${TINY_USB_DIR}/device/*.c)

add_executable(${EXECUTABLE}
    ${STM32CUBEMX_SOURCES} 
    ${PROJECT_SOURCES}
    ${TINY_USB_SOURCES}
    ${STARTUP_SCRIPT})

target_compile_definitions(${EXECUTABLE} PRIVATE
    #$<$<CONFIG:Debug>:DEBUG>
    ${MCU_MODEL}
    USE_HAL_DRIVER)

target_include_directories(${EXECUTABLE} SYSTEM PRIVATE
    ${STM32CUBEMX_INCLUDE_DIRECTORIES})

target_include_directories(${EXECUTABLE} PRIVATE
    ${PROJECT_INCLUDE_DIRECTORIES})

target_include_directories(${EXECUTABLE} PRIVATE
    ${TINY_USB_INCLUDE_DIRECTORIES})
###############################################################################
target_compile_options(${EXECUTABLE} PRIVATE
    ${CPU_PARAMETERS}
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wdouble-promotion
    -Wformat=2 -Wformat-truncation
    -Wundef
    -fno-common
    -Wno-unused-parameter
    $<$<COMPILE_LANGUAGE:CXX>:
        -Wconversion
        -Wno-volatile
        -Wold-style-cast
        -Wuseless-cast
        -Wsuggest-override>
    $<$<CONFIG:Debug>:-Og -g3 -ggdb>
    $<$<CONFIG:Release>:-Og -g0>)

target_link_options(${EXECUTABLE} PRIVATE
    -T${MCU_LINKER_SCRIPT}
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    $<$<VERSION_GREATER:$<C_COMPILER_VERSION>,10.3.1>:-Wl,--no-warn-rwx-segments>
    -Wl,--start-group
    -lc
    -lm
    -lnosys
    -lstdc++
    -Wl,--end-group
    -Wl,--print-memory-usage)
###############################################################################

# The last command can take a couple of seconds on larger project, usefull for debugging
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin)

option(DUMP_ASM "Create full assembly of final executable" OFF)
if (${DUMP_ASM})
    add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} -D -C $<TARGET_FILE:${EXECUTABLE}> > ${EXECUTABLE}.s)
endif()

