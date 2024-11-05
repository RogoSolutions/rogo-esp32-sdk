#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "board_led.h"
#include "app_device.h"
#include "wile_define.h"

#define DEVICE_DATA_PARTITION       "device"
#define DEVICE_DATA_NAMESPACE       "device"
#define DEVICE_STATE_BLOB           "state"

element_power_state_t elementState[ELM_NUM];

esp_err_t root_device_state_init(void);
esp_err_t root_device_save_state(void);
esp_err_t root_device_control(uint16_t element, uint16_t type, uint8_t *value);
esp_err_t root_device_local_control(uint8_t element, uint8_t mode);
void      root_device_identify(void);
void      root_device_delete_indicate(void);
void      root_device_prov_complete(void);
void      root_device_prov_none(void);
esp_err_t root_device_set_state(uint16_t element, uint16_t type, uint8_t *value);
esp_err_t root_device_get_state(uint16_t element, uint16_t feature, void **state, uint8_t *stateSize);

esp_err_t root_device_state_init(void){
    esp_err_t err = ESP_OK;

    for (uint8_t i=0; i<ELM_NUM; i++){
        elementState[i].element = i+1;
        elementState[i].state   = CTR_ONOFF_OFF;
    }

    #ifdef CONFIG_NVS_ENCRYPTION
    const esp_partition_t *nvs_key_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS, NULL);
    nvs_sec_cfg_t nvs_sec_cfg = {};
    err = nvs_flash_read_security_cfg(nvs_key_part, &nvs_sec_cfg);
    err = nvs_flash_secure_init_partition(DEVICE_DATA_PARTITION, &nvs_sec_cfg);
    if (err != ESP_OK) ESP_LOGE("DEVICE", "device data encrypt err=0x%X", err);
    #else
    err = nvs_flash_init_partition(DEVICE_DATA_PARTITION);
    #endif

    if (err != ESP_OK) return err;

    nvs_handle_t nvs_handler;
    err = nvs_open_from_partition(DEVICE_DATA_PARTITION, DEVICE_DATA_NAMESPACE, NVS_READWRITE, &nvs_handler);

    size_t elementStateSize = 0;
    err = nvs_get_item_size(nvs_handler, DEVICE_STATE_BLOB, NVS_TYPE_BLOB, &elementStateSize);

    if (err == ESP_OK){
        err = nvs_get_blob(nvs_handler, DEVICE_STATE_BLOB, elementState, &elementStateSize);
    }
    else{
        ESP_LOGW("DEVICE", "STATE NOT SET (%s)", esp_err_to_name(err));
        // err = nvs_set_blob(nvs_handler, DEVICE_STATE_BLOB, elementState, ELM_NUM * sizeof(element_power_state_t));
        // nvs_commit(nvs_handler);
    }
    nvs_close(nvs_handler);

    if (PROV_STATE == STEP_CFG_COMPLETE){
        uint8_t *devState = NULL;
        uint8_t devStateSize = 0;

        for (uint8_t i=0; i<ELM_NUM; i++){
            if (root_device_get_state(elementState[i].element, FEATURE_ONOFF, (void **)&devState, &devStateSize) == ESP_OK){
                elementState[i].state = devState[0];
            }
            if (devState != NULL) { free(devState); devState = NULL; }
        }
    }

    for (uint8_t i=0; i<ELM_NUM; i++){
        ESP_LOGW("DEVICE", "INIT STATE: ELM #%d, state %s",
                                        elementState[i].element,
                                        elementState[i].state == CTR_ONOFF_ON ? "on" : "off");
    }

    uint8_t initValue[2] = {0x00, CTR_ONOFF_OFF};
    for (int i=0; i<ELM_NUM; i++) {
        initValue[1] = elementState[i].state;
        root_device_control(elementState[i].element, FEATURE_ONOFF, initValue);
    }

    return err;
}

esp_err_t root_device_save_state(void){
    esp_err_t ret = ESP_OK;

    nvs_handle_t nvs_handler;
    ret += nvs_open_from_partition(DEVICE_DATA_PARTITION, DEVICE_DATA_NAMESPACE, NVS_READWRITE, &nvs_handler);
    ret += nvs_set_blob(nvs_handler, DEVICE_STATE_BLOB, elementState, ELM_NUM * sizeof(element_power_state_t));
    ret += nvs_commit(nvs_handler);
    nvs_close(nvs_handler);

    return ret;
}

esp_err_t root_device_control(uint16_t element, uint16_t type, uint8_t *value){
    ESP_LOGI("DEVICE", "Root device control, elm: %d", element);
    ESP_LOG_BUFFER_HEX("ATTR VALUE", value, rgmsg_feature_size(type));

    esp_err_t ret = ESP_OK;

    switch (type){
        case FEATURE_ONOFF:{
            uint16_t onoffState = value[0] << 8 | value[1];

            if (onoffState == CTR_ONOFF_ON){
                ret += board_led_indicator_brightness(element, BOARD_LED_ON_BRIGHTNESS);
                ret += board_switch_control(element, SWITCH_ACTIVE_LEVEL);
            }
            else if (onoffState == CTR_ONOFF_OFF){
                ret += board_led_indicator_brightness(element, BOARD_LED_OFF_BRIGHTNESS);
                ret += board_switch_control(element, !SWITCH_ACTIVE_LEVEL);
            }

            for (uint8_t i=0; i<ELM_NUM; i++){
                if (elementState[i].element == element && elementState[i].state != onoffState){
                    elementState[i].state = (uint8_t)onoffState;
                    break;
                }
            }
            break;
        }
        default:{
            ESP_LOGW("DEVICE", "NOT SUPPORTED CONTROL ATTR %d", type);
            ret = ESP_ERR_NOT_SUPPORTED;
            break;
        }
    }

    if (ret != ESP_OK) ESP_LOGE("DEVICE", "root_device_control fail, err=0x%04X", ret);
    root_device_save_state();

    return ret;
}

esp_err_t root_device_local_control(uint8_t element, uint8_t mode){
    uint8_t buttonIdx = 0;
    bool validButton = false;

    for (buttonIdx=0; buttonIdx<BTN_NUM; buttonIdx++){
        if (element == elementState[buttonIdx].element){
            validButton = true;
            break;
        }
    }

    if (!validButton) return ESP_ERR_NOT_FOUND;

    if (mode == CTR_ONOFF_FLIP){
        elementState[buttonIdx].state = !elementState[buttonIdx].state;
    }
    else if (mode == CTR_ONOFF_ON){
        elementState[buttonIdx].state = CTR_ONOFF_ON;
    }
    else if (mode == CTR_ONOFF_OFF){
        elementState[buttonIdx].state = CTR_ONOFF_OFF;
    }
    else{
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI("DEVICE", "Local control element #%d, state: %d", element, elementState[buttonIdx].state);

    uint8_t controlValue[2] = {0x00, elementState[buttonIdx].state};

    // uint8_t *devState = NULL;
    // uint8_t devStateSize = 0;
    if (PROV_STATE == STEP_CFG_COMPLETE){
        root_device_set_state(element, FEATURE_ONOFF, controlValue);
    }
    // if (devState != NULL) free(devState);

    return root_device_control(element, FEATURE_ONOFF, controlValue);
}

esp_err_t rgmgt_device_event_cb(uint16_t event){
    switch (event){
        case CTR_WILE_EVT_WIFI_CONNECTED:
            break;
        case CTR_WILE_EVT_WIFI_DISCONNECTED:
            break;
        case CTR_WILE_EVT_WIFI_CONNECT_FAIL:
            break;
        case CTR_WILE_EVT_CLOUD_CONNECTED:
            break;
        case CTR_WILE_EVT_CLOUD_DISCONNECTED_LOWMEM:
            break;
        case CTR_WILE_EVT_CLOUD_DISCONNECTED:
            break;
        case CTR_WILE_EVT_DEVICE_INDENTIFY:
            break;
        case CTR_WILE_EVT_DEVICE_PROV_CANCELED:
            board_led_indicator_stop(DEVICE_ELEMENT_ALL, BLINK_FAST);
            board_led_indicator_state();
            break;
        case CTR_WILE_EVT_DEVICE_PROV_COMPLETE:
            root_device_prov_complete();
            for (uint8_t i=0; i<ELM_NUM; i++){
                uint8_t deviceValue[2] = {0x00, elementState[i].state};
                root_device_set_state(elementState[i].element, FEATURE_ONOFF, deviceValue);
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}

void root_device_delete_indicate(void){
    #ifdef LED_RGB_DATA
    deviceLedFlipNum = 4;
    xTaskCreate(board_led_rgb_flip_task, "board_led_rgb_flip_task", 1024, &deviceLedFlipNum, tskIDLE_PRIORITY, NULL);
    #endif
}

void root_device_identify(void){
    board_led_indicator(DEVICE_ELEMENT_ALL, BLINK_FAST);
}

void root_device_prov_complete(void){
    #if LED_RGB_WS2812
    board_led_indicator_color(DEVICE_ELEMENT_ALL, BOARD_LED_BLUE_HUE);
    #endif
    board_led_indicator_stop(DEVICE_ELEMENT_ALL, BLINK_FAST);
    board_led_indicator_state();
}

void root_device_prov_none(void){
    #if LED_RGB_WS2812
    board_led_indicator_color(DEVICE_ELEMENT_ALL, BOARD_LED_PURPLE_HUE);
    #endif
    board_led_indicator(DEVICE_ELEMENT_ALL, BLINK_TRIPLE_FAST);
    vTaskDelay((200*6 + 300) / portTICK_PERIOD_MS); // Time BLINK_TRIPLE_FAST + 300ms
}

esp_err_t root_device_set_state(uint16_t element, uint16_t type, uint8_t *value){
    return rgmgt_device_set_state(rootEID, element, type, value, true, true);
}

esp_err_t root_device_get_state(uint16_t element, uint16_t feature, void **state, uint8_t *stateSize){
    esp_err_t ret = ESP_OK;
    uint8_t *elmInfo = NULL;
    size_t elmInfoLen = 0;
    ret = rgmgt_device_get_state(rootEID, (void **)&elmInfo, &elmInfoLen);
    ESP_LOG_BUFFER_HEX("STATE", elmInfo, elmInfoLen);

    uint8_t *elmState = NULL;
    size_t elmStateLen = 0;
    ret += rgmgt_device_get_state_elm_attr(element, feature, elmInfo, elmInfoLen, (void **)&elmState, &elmStateLen);
    *stateSize = elmStateLen;
    if (*state != NULL) free(*state);
    *state = malloc(*stateSize);
    memcpy(*state, elmState, *stateSize);

    if (elmInfo != NULL) free(elmInfo);
    if (elmState != NULL) free(elmState);
    return ret;
}

#ifdef __cplusplus
}
#endif