set(srcs "wile_config.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS
                       "."
                       "../wile_common"
                       PRIV_REQUIRES main nvs_flash app_update)