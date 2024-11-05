set(srcs "main.cpp"
         "board.c"
         "board_led.c"
         "app_device.cpp")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS
                       ".")