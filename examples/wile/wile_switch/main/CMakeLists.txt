set(srcs "main.cpp"
         "board.c"
         "board_led.c"
         "board_test.c"
         "app_device.cpp")

idf_component_register(SRCS "board_test.c" "${srcs}"
                       INCLUDE_DIRS
                       ".")