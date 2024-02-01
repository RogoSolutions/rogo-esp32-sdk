# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/components/rogo/wile_config
                         $ENV{IDF_PATH}/components/rogo/wile_common)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

#list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/components/rogo/wile_common/sdkconfig.wile"
#                                "$ENV{IDF_PATH}/components/rogo/wile_common/sdkconfig.wile.esp32c3"
#                                "sdkconfig.defaults")
list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/components/rogo/wile_common/sdkconfig.wile")

if(CONFIG_IDF_TARGET_ESP32C3)
list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/components/rogo/wile_common/sdkconfig.wile.esp32c3")
endif()

list(PREPEND SDKCONFIG_DEFAULTS "sdkconfig.defaults")

project(wile_switch)
