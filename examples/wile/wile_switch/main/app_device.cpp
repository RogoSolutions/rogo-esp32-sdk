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

#ifdef CONFIG_ESP32C3_RD_CN_04_V11
element_power_state_t elementState[BTN_NUM] = {
    { 1, CTR_ONOFF_OFF },
    { 2, CTR_ONOFF_OFF },
    { 3, CTR_ONOFF_OFF },
    { 4, CTR_ONOFF_OFF },
};
uint8_t deviceLedFlipNum = 2;
TaskHandle_t boardLedIndicateWifiHandle = NULL;
// __NOINIT_ATTR static element_power_state_t elementState[BTN_NUM];
#else
element_power_state_t elementState[BTN_NUM] = {
    { 1, CTR_ONOFF_OFF },
    { 2, CTR_ONOFF_OFF },
    { 3, CTR_ONOFF_OFF },
    { 4, CTR_ONOFF_OFF },
};
uint8_t deviceLedFlipNum = 2;
TaskHandle_t boardLedIndicateWifiHandle = NULL;
#endif

void root_device_state_init(void){
    if (PROV_STATE == STEP_CFG_COMPLETE){
        uint8_t *devState = NULL;
        uint8_t devStateSize = 0;
        root_device_get_state((void **)&devState, &devStateSize);
        uint16_t stateIdx = 0;
        for (uint8_t i=1; i<=BTN_NUM; i++){
            elementState[i-1].state = (uint8_t)(devState[stateIdx] << 8 | devState[stateIdx+1]);
            // board_sw_zc_control(i, elementState[i-1].state);
            root_device_control(i, FEATURE_ONOFF, devState+stateIdx);
            stateIdx += 2;
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

    switch (type){
        case FEATURE_ONOFF:
            if (devState == CTR_ONOFF_ON){
                #ifdef ZERO_CROSS
                board_led_rgb_set_brightness(element, LED_ON);
                board_sw_zc_control(element, SW_ON);
                #else
                board_sw_control(element, SW_ON);
                #endif
            }
            else if (devState == CTR_ONOFF_OFF){
                #ifdef ZERO_CROSS
                board_led_rgb_set_brightness(element, LED_OFF);
                board_sw_zc_control(element, SW_OFF);
                #else
                board_sw_control(element, SW_OFF);
                #endif
            }
            break;
        default:
            break;
    }
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
    if (root_device_get_state((void **)&devState, &devStateSize) == ESP_OK){
        ESP_LOG_BUFFER_HEX("ROOT DEVICE STATE", devState, devStateSize);
        root_device_set_state(element, FEATURE_ONOFF, controlValue);
    }

    if (devState != NULL) free(devState);

    return root_device_control(element, FEATURE_ONOFF, controlValue);
}

#ifndef CONFIG_ESP32C3_RD_CN_04_V11
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
    #ifdef CONFIG_ESP32C3_RD_CN_04_V11
    xTaskCreate(board_led_rgb_prov_run_task, "board_led_rgb_prov_run_task", 512, NULL, tskIDLE_PRIORITY, NULL);
    // deviceLedFlipNum = 6;
    // xTaskCreate(board_led_rgb_flip_task, "board_led_rgb_flip_task", 1024, &deviceLedFlipNum, tskIDLE_PRIORITY, NULL);
    #endif
}

void root_device_network_indicate(esp_event_base_t event_base, int32_t event_id, void* event_data){
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // gpio_set_level(LED_B, LED_OFF);
        // if (PROV_STATE == STEP_CFG_COMPLETE) board_led_rgb_set_color(LED_ALL, LED_BLUE, true);
        if (boardLedIndicateWifiHandle != NULL){
            vTaskDelete(boardLedIndicateWifiHandle);
            boardLedIndicateWifiHandle = NULL;
        }
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        // gpio_set_level(LED_B, LED_ON);
        // if (PROV_STATE == STEP_CFG_COMPLETE) board_led_rgb_set_color(LED_ALL, LED_YELLOW, true);
        if (PROV_STATE == STEP_CFG_COMPLETE && boardLedIndicateWifiHandle == NULL){
            #ifdef CONFIG_ESP32C3_RD_CN_04_V11
            xTaskCreate(board_led_rgb_wifi_drop_task, "board_led_rgb_wifi_drop_task", 512, NULL, tskIDLE_PRIORITY, &boardLedIndicateWifiHandle);
            #endif
        }
    }
}

void root_device_delete_indicate(void){
    #ifdef CONFIG_ESP32C3_RD_CN_04_V11
    // board_led_rgb_set_color(LED_ALL, LED_MINT, true);
    deviceLedFlipNum = 4;
    xTaskCreate(board_led_rgb_flip_task, "board_led_rgb_flip_task", 1024, &deviceLedFlipNum, tskIDLE_PRIORITY, NULL);
    #endif
}

void root_device_prov_complete(void){
    #ifdef CONFIG_ESP32C3_RD_CN_04_V11
    board_led_rgb_set_color(LED_ALL, LED_BLUE, false);
    root_device_state_init();
    #else
    gpio_set_level(LED_R, LED_OFF);
    #endif
}

void root_device_prov_none(void){
    #ifdef CONFIG_ESP32C3_RD_CN_04_V11
    // board_led_rgb_set_color(LED_ALL, LED_MINT, false);
    board_led_rgb_set_color(LED_ALL, LED_BLUE, false);
    root_device_state_init();
    xTaskCreate(board_led_rgb_prov_none_task, "board_led_rgb_prov_none_task", 512, NULL, tskIDLE_PRIORITY, &ledIndicateHandle);
    #else
    xTaskCreate(task_led_slow, "task_led_slow", 512, NULL, tskIDLE_PRIORITY, &ledIndicateHandle);
    #endif
}

esp_err_t root_device_set_state(uint16_t element, uint16_t type, uint8_t *value){
    // return set_device_state(rootEID, type, value, true);
    return rgmgt_device_set_state(rootEID, element, type, value, true);
}

esp_err_t root_device_get_state(void **state, uint8_t *stateSize){
    return get_local_device_state(rootEID, state, stateSize);
}

#ifdef __cplusplus
}
#endif