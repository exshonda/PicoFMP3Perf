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
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(fmp3_pico_sdk C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# TOPPERS/FMP3 の Raspberry Pi Pico SDK 用のコンパイラ定義をインクルード
include(../fmp3/fmp3_pico_sdk.cmake)

# Add executable. Default name is the project name, version 0.1

add_executable(fmp3_pico_sdk
    fmp3_pico_sdk.c
    sample1.c
)

pico_set_program_name(fmp3_pico_sdk "fmp3_pico_sdk")
pico_set_program_version(fmp3_pico_sdk "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(fmp3_pico_sdk 1)
pico_enable_stdio_usb(fmp3_pico_sdk 0)

# TOPPERS/FMP3 のカーネルオブジェクト定義のcfgファイルを設定
set(FMP3_APP_CFG_FILE ${PROJECT_SOURCE_DIR}/sample1.cfg)

list(APPEND FMP3_INCLUDE_DIRS
    ${FMP3_KERNEL_DIR}/sample
)

# TOPPERS/FMP3 のライブラリを追加（第二引数は作業フォルダ）
add_subdirectory(${FMP3_ROOT_DIR} fmp3)

# TOPPERS/FMP3 付属のソースファイルを追加
include(${FMP3_ROOT_DIR}/library/library.cmake)
include(${FMP3_ROOT_DIR}/syssvc/syssvc.cmake)

# Add the standard library to the build
target_link_libraries(fmp3_pico_sdk
    pico_stdlib
    pico_multicore
    fmp3
)

# Add the standard include files to the build
target_include_directories(fmp3_pico_sdk PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${PROJECT_SOURCE_DIR}
)

# TOPPERS/FMP3 を使うための Raspberry Pi Pico SDK の設定
fmp3_set_pico_sdk_options(${CMAKE_PROJECT_NAME})

pico_add_extra_outputs(fmp3_pico_sdk)

# TOPPERS/FMP3 のチェックを行う（シンボルがGCされるとエラーになるので省略）
#fmp3_cfg_check(${CMAKE_PROJECT_NAME})
