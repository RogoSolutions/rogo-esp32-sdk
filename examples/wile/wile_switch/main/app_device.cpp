#ifdef __cplusplus
extern "C" {
#endif
#include "board.h"
#include "board_led.h"
#include "app_device.h"
#include "wile_define.h"

// bool localDevState = 0;
// uint8_t elementState[BTN_NUM] = {CTR_ONOFF_OFF};
// __NOINIT_ATTR uint8_t elementState[BTN_NUM]= {CTR_ONOFF_OFF};

#if defined(CONFIG_ESP32C3_RD_CN_04_V11)
element_power_state_t elementState[BTN_NUM] = {
    { 1, CTR_ONOFF_OFF },
    { 2, CTR_ONOFF_OFF },
    { 3, CTR_ONOFF_OFF },
    { 4, CTR_ONOFF_OFF },
};
uint8_t deviceLedFlipNum = 2;
TaskHandle_t boardLedIndicateHardwareHandle = NULL;
uint8_t deviceZeroCrossingCheck = true;
// __NOINIT_ATTR static element_power_state_t elementState[BTN_NUM];
#elif defined(CONFIG_ESP32C3_RD_CN_03_REM_V11)
element_power_state_t elementState[BTN_NUM] = {
    { 1, CTR_ONOFF_OFF },
    { 2, CTR_ONOFF_OFF },
    { 3, CTR_ONOFF_OFF },
};
uint8_t deviceLedFlipNum = 2;
TaskHandle_t boardLedIndicateHardwareHandle = NULL;
#else
element_power_state_t elementState[BTN_NUM] = {
    { 1, CTR_ONOFF_OFF },
    { 2, CTR_ONOFF_OFF },
    { 3, CTR_ONOFF_OFF },
    { 4, CTR_ONOFF_OFF },
};
uint8_t deviceLedFlipNum = 2;
TaskHandle_t boardLedIndicateHardwareHandle = NULL;
#endif

void root_device_state_init(void){
    if (PROV_STATE == STEP_CFG_COMPLETE){
        uint8_t *devState = NULL;
        uint8_t devStateSize = 0;
        for (uint8_t i=1; i<=BTN_NUM; i++){
            root_device_get_state(i, FEATURE_ONOFF, (void **)&devState, &devStateSize);
            ESP_LOG_BUFFER_HEX("STATE ELM", devState, devStateSize);
            elementState[i-1].state = devState[0];
            uint8_t elmState[2] = {0x00, devState[0]};
            root_device_control(i, FEATURE_ONOFF, elmState);
        }
        if (devState != NULL) free(devState);
    }
    else{
        uint8_t value[2] = {0x00, CTR_ONOFF_OFF};
        for (int i=1; i<=BTN_NUM; i++) {
            root_device_control(i, FEATURE_ONOFF, value);
            // board_sw_zc_control(i, CTR_ONOFF_OFF);
        }
    }
}

esp_err_t root_device_control(uint16_t element, uint16_t type, uint8_t *value){
    ESP_LOGI("DEVICE", "Root device control, elm: %d", element);
    uint16_t devState = value[0] << 8 | value[1];
    ESP_LOGI("ROOT", "State: %d", devState);

    #ifdef CONFIG_ESP32C3_RD_CN_03_REM_V11
    if (devState == CTR_ONOFF_ON){
        board_led_rgb_set_brightness(element, LED_ON);
    }
    else if (devState == CTR_ONOFF_OFF){
        board_led_rgb_set_brightness(element, LED_OFF);
    }
    switch (element){
        case 1: // open
            if (devState == CTR_ONOFF_ON){
                board_sw_control(1, SW_ON);
                board_sw_control(2, SW_ON);
                board_sw_control(3, SW_OFF);
            }
            else if (devState == CTR_ONOFF_OFF){
                board_sw_control(1, SW_OFF);
                board_sw_control(2, SW_ON);
                board_sw_control(3, SW_OFF);
            }
            break;
        case 2: // stop
            if (devState == CTR_ONOFF_ON){
                board_sw_control(1, SW_OFF);
                board_sw_control(2, SW_OFF);
                board_sw_control(3, SW_OFF);
            }
            else if (devState == CTR_ONOFF_OFF){
                board_sw_control(1, SW_OFF);
                board_sw_control(2, SW_ON);
                board_sw_control(3, SW_OFF);
            }
            break;
        case 3: // close
            if (devState == CTR_ONOFF_ON){
                board_sw_control(1, SW_OFF);
                board_sw_control(2, SW_OFF);
                board_sw_control(3, SW_ON);
            }
            else if (devState == CTR_ONOFF_OFF){
                board_sw_control(1, SW_OFF);
                board_sw_control(2, SW_ON);
                board_sw_control(3, SW_OFF);
            }
            break;
    }
    return ESP_OK;

    #else

    switch (type){
        case FEATURE_ONOFF:
            if (devState == CTR_ONOFF_ON){
                #ifdef LED_RGB_DATA
                board_led_rgb_set_brightness(element, LED_ON);
                #endif
                #ifdef ZERO_CROSS
                board_sw_zc_control(element, SW_ON);
                #else
                board_sw_control(element, SW_ON);
                #endif
            }
            else if (devState == CTR_ONOFF_OFF){
                #ifdef LED_RGB_DATA
                board_led_rgb_set_brightness(element, LED_OFF);
                #endif
                #ifdef ZERO_CROSS
                board_sw_zc_control(element, SW_OFF);
                #else
                board_sw_control(element, SW_OFF);
                #endif
            }
            break;
        default:
            break;
    }

    #endif
    for (uint8_t i=0; i<BTN_NUM; i++){
        if (elementState[i].element == element && elementState[i].state != devState){
            elementState[i].state = (uint8_t)devState;
            break;
        }
    }
    return ESP_OK;
}

esp_err_t root_device_local_control(uint8_t element, uint8_t mode){
    uint8_t localIdx = 0;
    for (localIdx = 0; localIdx < BTN_NUM; localIdx++){
        if (element == elementState[localIdx].element) break;
    }
    ESP_LOGI("DEVICE", "Local Index: %d", localIdx);

    if (mode == CTR_ONOFF_FLIP){
        elementState[localIdx].state = !elementState[localIdx].state;
    }
    else if (mode == CTR_ONOFF_ON){
        elementState[localIdx].state = CTR_ONOFF_ON;
    }
    else if (mode == CTR_ONOFF_OFF){
        elementState[localIdx].state = CTR_ONOFF_OFF;
    }

    uint8_t controlValue[2] = {0x00, elementState[localIdx].state};

    uint8_t *devState = NULL;
    uint8_t devStateSize = 0;
    if (root_device_get_state(element, FEATURE_ONOFF, (void **)&devState, &devStateSize) == ESP_OK){
        // ESP_LOG_BUFFER_HEX("ROOT DEVICE STATE", devState, devStateSize);
        root_device_set_state(element, FEATURE_ONOFF, controlValue);
    }

    if (devState != NULL) free(devState);

    return root_device_control(element, FEATURE_ONOFF, controlValue);
}

#ifndef LED_RGB_DATA
static void task_led_slow(void *pvParameters)
{
    while(PROV_STATE != STEP_CFG_COMPLETE){
        vTaskDelay(500);
        // board_led_operation(LED_B, LED_OFF);
        gpio_set_level(LED_R, LED_OFF);
        vTaskDelay(500);
        // board_led_operation(LED_B, LED_ON);
        gpio_set_level(LED_R, LED_ON);
    }
    vTaskDelete(ledIndicateHandle);
}

static void task_led_fast(void *pvParameters)
{
    uint32_t lastTime = xTaskGetTickCount();
    while(xTaskGetTickCount() - lastTime <= 5000){
        vTaskDelay(100);
        gpio_set_level(LED_R, LED_OFF);
        vTaskDelay(100);
        gpio_set_level(LED_R, LED_ON);
    }
    if (ledIndicateHandle != NULL){
        vTaskResume(ledIndicateHandle);
    }
    vTaskDelete(NULL);
}
#endif

void root_device_identify(void){
    // xTaskCreate(task_led_fast, "task_led_fast", 1024, NULL, 3, NULL);
    #if defined(CONFIG_ESP32C3_RD_CN_04_V11) || defined(CONFIG_ESP32C3_RD_CN_03_REM_V11)
    xTaskCreate(board_led_rgb_prov_run_task, "board_led_rgb_prov_run_task", 512, NULL, tskIDLE_PRIORITY, NULL);
    // deviceLedFlipNum = 6;
    // xTaskCreate(board_led_rgb_flip_task, "board_led_rgb_flip_task", 1024, &deviceLedFlipNum, tskIDLE_PRIORITY, NULL);
    #endif
}

esp_err_t rgmgt_device_event_cb(uint16_t event){
    switch (event){
        case CTR_WILE_EVT_WIFI_CONNECTED:
            if (deviceZeroCrossingCheck && PROV_STATE == STEP_CFG_COMPLETE){
                board_led_rgb_set_color(LED_ALL, LED_BLUE, true);
            }
            break;
        case CTR_WILE_EVT_WIFI_DISCONNECTED:
            if (deviceZeroCrossingCheck) board_led_rgb_set_color(LED_ALL, LED_YELLOW, true);
            break;
        case CTR_WILE_EVT_WIFI_CONNECT_FAIL:
            break;
        case CTR_WILE_EVT_CLOUD_CONNECTED:
            if (deviceZeroCrossingCheck) board_led_rgb_set_color(LED_ALL, LED_BLUE, true);
            break;
        case CTR_WILE_EVT_CLOUD_DISCONNECTED:
            if (deviceZeroCrossingCheck) board_led_rgb_set_color(LED_ALL, LED_RED, true);
            break;
        default:
            break;
    }
    return ESP_OK;
}

void root_device_delete_indicate(void){
    #if defined(CONFIG_ESP32C3_RD_CN_04_V11) || defined(CONFIG_ESP32C3_RD_CN_03_REM_V11)
    // board_led_rgb_set_color(LED_ALL, LED_MINT, true);
    deviceLedFlipNum = 4;
    xTaskCreate(board_led_rgb_flip_task, "board_led_rgb_flip_task", 1024, &deviceLedFlipNum, tskIDLE_PRIORITY, NULL);
    #endif
}

void root_device_prov_complete(void){
    #if defined(CONFIG_ESP32C3_RD_CN_04_V11) || defined(CONFIG_ESP32C3_RD_CN_03_REM_V11)
    if (deviceZeroCrossingCheck) board_led_rgb_set_color(LED_ALL, LED_BLUE, false);
    root_device_state_init();
    #else
    gpio_set_level(LED_R, LED_OFF);
    #endif
}

void root_device_prov_none(void){
    #if defined(CONFIG_ESP32C3_RD_CN_04_V11) || defined(CONFIG_ESP32C3_RD_CN_03_REM_V11)
    if (deviceZeroCrossingCheck) board_led_rgb_set_color(LED_ALL, LED_YELLOW, false);
    // board_led_rgb_set_color(LED_ALL, LED_BLUE, false);
    root_device_state_init();
    // xTaskCreate(board_led_rgb_prov_none_task, "board_led_rgb_prov_none_task", 512, NULL, tskIDLE_PRIORITY, &ledIndicateHandle);
    #else
    xTaskCreate(task_led_slow, "task_led_slow", 512, NULL, tskIDLE_PRIORITY, &ledIndicateHandle);
    #endif
}

esp_err_t root_device_set_state(uint16_t element, uint16_t type, uint8_t *value){
    // return set_device_state(rootEID, type, value, true);
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