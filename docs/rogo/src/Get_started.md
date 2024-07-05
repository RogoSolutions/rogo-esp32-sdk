# Get started with Rogo ESP32 WILE SDK

## Environment setup

Rogo ESP32 WILE SDK is based on the ESP-IDF version 5.1.1 release with added proprietary library for supporting Wi-Le network. Hence, the toolchain installation process is identical to the ESP-IDF documentation published by Espressif. Therefore, this document also feature and make use of the **ESP-IDF instructions** which is publicly available [here](https://docs.espressif.com/projects/esp-idf/en/v5.1.2/esp32/index.html) 

[Links to Rogo ESP32 WILE SDK repo](https://github.com/RogoSolutions/rogo-esp32-sdk/tree/dev/v5.1.1 ':)')
**Note:** This link pointed to the **dev/5.1.1 branch** which currently is the correct and stable branch for the SDK 

### Manual installation 

- [Linux and macOS](./linux_mac_install.rst '🐧 & 🍎')

**Windows** *(to be updated)*

### Install with IDE (VScode extension) 
*This method is highly recommended to novice users by **Espressif**! However the author of this docs find it subjectively confusing and recommend to install it manually especially if you are using **MacOS or Linux***

- [Linux and macOS](./vscode_extesion_linux_macos.md '🐧& 🍏 or ')
- [Windows](vscode_extension_windows.md '❖')

## Build Your First Project !
After install the **necessary tools and dependency** with **Manual installation**, if you never work with **ESP-IDF** before you can get start with simple projects. *e.g blink, hello-world,.....* by following build and flash instructions :

```{eval-rst}
-   Mac0S and Linux: :ref:`rst_anchor`
-   Windows: to be updated 
```

After you familiar with a simple project let's get started with a Wi-LE project with this [Wi-LE example](./wile_project_start.md ':)').




If you **installed with IDE** there will be an **instruction on the IDE extension** which is self elaboration.