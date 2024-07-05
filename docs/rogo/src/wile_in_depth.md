# Wi-le example project in-depth exploration 

## Wi-le Source and config files structure

### Wi-le config files
#### CMakeLists.txt
In order to use the built-in wile library, you need to add few lines of settings to the *CMakeLists.txt* of your project, located in your project folder, following is the *CMakeLists.txt* file of the **wile_switch** project.

```{eval-rst}
.. note::
    There are **2 CMakeLists.txt files**, one located in the **project directory**, other locate inside **main** or **src** folder where source files of the project are located. Make sure you edit the correct file in the **project directory** !

```
```C

# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/components/rogo/wile_config
                         $ENV{IDF_PATH}/components/rogo/wile_common)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/components/rogo/wile_common/sdkconfig.wile")

if(CONFIG_IDF_TARGET_ESP32C3)
list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/components/rogo/wile_common/sdkconfig.wile.esp32c3")
endif()

list(PREPEND SDKCONFIG_DEFAULTS "sdkconfig.defaults")

project(wile_switch)

```

First of all you need to tell the compiler where to find the library, header files, which are located at `{path to SDK}/components/rogo` with following lines:
```C
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/components/rogo/wile_config
                         $ENV{IDF_PATH}/components/rogo/wile_common)
```
In this projects, we will need to use header files inside `wile_config` and `wile_common` folders, which are **required** if you want to use Wi-le.
You can also add your own remote components (source file, library) using this command !

After that you also **required** to add an **default SDK config for Wi-le projects**, and also **chip target specified sdk config** , which can occasionally be omitted.

```C
list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/components/rogo/wile_common/sdkconfig.wile")

if(CONFIG_IDF_TARGET_ESP32C3)
list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/components/rogo/wile_common/sdkconfig.wile.esp32c3")
endif()
```

They are configurations in the SDK config which are needed to run Wi-le.

The rest of the file is similar to the normal esp32 project .

### Kconfig.projbuild
This file belongs to the project configuration of the ESP-IDF, you can refer here for more details, below just a summary of how this work in the context of the **Wile_switch** which belongs to ROGO ESP32 SDK. 
This file is for adding options, and configuration of your own into the **menuconfig**, these setting will be treated as *define* when including `sdkconfig.h` file.
Note: that whenever you need to use any of the define variable, you can just include `sdkconfig.h` files, but the `sdkconfig.h` file itself doesn't exist, every definition will be written to `sdkconfig`, and the compiler will knows which definition belongs to `sdkconfig.h` and execute accordingly 'for example :

```C
menu "WiLe device"

   ...

    menu "Device configuration"      
        config BOARD_TEST_ENABLE
            bool "Enable factory board test"
            default n
    endmenu
endmenu
```
In the `sdkconfig` file will have this definition :
```C
# Device configuration
#
# CONFIG_BOARD_TEST_ENABLE is not set
# end of Device configuration
# end of WiLe device
```

and if you want to use it in your source file :

```{eval-rst}
.. figure:: ../doc/picture/ex1.jpg
    :align: center

```


### idf_component.yml
This is a manifest file which contains the components that user want to add bui