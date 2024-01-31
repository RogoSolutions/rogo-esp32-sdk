#include "wile_config.h"
#include "sdkconfig.h"

const char* rgmgt_config_device_id(void){
    #ifdef CONFIG_DEVICE_NAME
    // WILE_DEVICE_NAME = CONFIG_DEVICE_NAME;
    return CONFIG_DEVICE_NAME;
    #else
    // WILE_DEVICE_NAME = "WILE_DEV_NODE";
    return "WILE_DEV_NODE";
    #endif
    // return WILE_DEVICE_NAME;
}

esp_err_t rgmgt_config_nvs_enc(void){
    esp_err_t err = ESP_OK;
    #ifdef CONFIG_NVS_ENCRYPTION
    const esp_partition_t *nvs_key_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS, NULL);
    nvs_sec_cfg_t nvs_sec_cfg = {};
    err = nvs_flash_read_security_cfg(nvs_key_part, &nvs_sec_cfg);
    err = nvs_flash_secure_init_partition("rogo", &nvs_sec_cfg);
    if (err != ESP_OK) ESP_LOGE("NVS", "rogo data encrypt err=0x%X", err);
    #else
    err = nvs_flash_init_partition("rogo");
    #endif
    return err;
}