idf_build_get_property(idf_target IDF_TARGET)

list(PREPEND SDKCONFIG_DEFAULTS "sdkconfig.defaults")

idf_component_register(INCLUDE_DIRS
                       "."
                       "../wile_config"
                       "../ir_common"
                       PRIV_REQUIRES main nvs_flash app_update)

if(CONFIG_IDF_TARGET_ESP32)
elseif(CONFIG_IDF_TARGET_ESP32C3)
    add_prebuilt_library(libwile_common "esp32c3/libwile_common.a"
                         PRIV_REQUIRES main spi_flash nvs_flash app_update log)
else()
    add_prebuilt_library(libwile_common "libwile_common.a"
                         PRIV_REQUIRES main spi_flash nvs_flash app_update log)
endif()

target_link_libraries(${COMPONENT_LIB} INTERFACE libwile_common)