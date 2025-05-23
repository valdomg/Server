# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Server C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

pico_add_library(example_lwip_http_util NOFLAG)
target_sources(example_lwip_http_util INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/example_http_client_util.c
        )
pico_mirrored_target_link_libraries(example_lwip_http_util INTERFACE
        pico_lwip_http
        pico_lwip_mbedtls
        pico_mbedtls
        hardware_clocks
        hardware_pll
        )
target_include_directories(example_lwip_http_util INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        )

# Add executable. Default name is the project name, version 0.1

add_executable(Server Server.c )
set(WIFI_SSID "SUA REDE") # Nome da sua rede wifi
set(WIFI_PASSWORD "SENHA DA REDE") # Senha da rede wifi


pico_set_program_name(Server "Server")
pico_set_program_version(Server "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Server 0)
pico_enable_stdio_usb(Server 1)

# Add the standard library to the build
target_link_libraries(Server
        pico_stdlib
        pico_cyw43_arch_lwip_threadsafe_background
        example_lwip_http_util
        hardware_adc)

# Add the standard include files to the build
target_include_directories(Server PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/..
)

# Add any user requested libraries
target_link_libraries(Server 
        
        )

pico_add_extra_outputs(Server)

