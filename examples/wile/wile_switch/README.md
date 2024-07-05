# Wile Switch Example - 4 Gag Smart Switch - an example of Rogo SDK based on Wi-Le  
This is an example for the Rogo Wile SDK based on Esp32 series Micro-controller, this is customization version of the ESP IDF 5.1.1, which comes with Rogo's proprietary Libraries for Wile Functionalities 

## Supported Hardware 
### Chip supported 
- ESP32C3 
- ESP32C6
- ESP32S3
**NOTICE**- Original ESP32 chip can successfully be built with wile examples, however **Rogo** does NOT recommend using ESP32 since it lacks of Bluetooth LE 5.0 support, which may cause unknown functional issue and future support.

## Getting start
Please refer to this [documentation](https://tudangrogo.github.io/documentation-example-/wile_project_start.html)

### Upload when using Secure Boot

- Build project:\
  `idf.py build`
- Upload bootloader:\
  `esptool.py --chip esp32c3 --before=default_reset --after=no_reset --no-stub write_flash --flash_mode dio --flash_freq 80m --flash_size 4MB 0x0 ./build/bootloader/bootloader.bin`
- Upload partition table:\
  `idf.py partition-table-flash`
- Upload app:\
  `idf.py encrypted-flash monitor`

