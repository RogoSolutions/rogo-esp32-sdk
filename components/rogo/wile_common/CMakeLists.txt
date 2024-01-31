idf_build_get_property(idf_target IDF_TARGET)

list(PREPEND SDKCONFIG_DEFAULTS "sdkconfig.defaults")

idf_component_register(INCLUDE_DIRS
                       "."
                       "../wile_config"
                       "../ir_common"
                       PRIV_REQUIRES main nvs_flash app_update)

add_prebuilt_library(libwile_common "libwile_common.a"
                     PRIV_REQUIRES main spi_flash nvs_flash app_update log)

target_link_libraries(${COMPONENT_LIB} INTERFACE libwile_common)