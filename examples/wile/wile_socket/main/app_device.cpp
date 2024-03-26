#ifdef __cplusplus
extern "C"
{
#endif
#include "board.h"
#include "app_device.h"
#include "wile_define.h"
#include "sdkconfig.h"
    bool gotIP = 0;
    bool isReset = 0;

    esp_err_t root_device_control(uint16_t element, uint16_t type, uint8_t *value)
    {
        ESP_LOGI("DEVICE", "Root device control");
        uint8_t devState = value[0] << 8 | value[1];
        ESP_LOGW("ROOT DEVICE", "State: %d", devState);

        switch (type)
        {
        case FEATURE_ONOFF:
            localDevState = devState;
            uint8_t ledColor;
            if(PROV_STATE != STEP_CFG_COMPLETE)
            {
                ledColor = ORANGE;
            }   
            else
            {
                if(gotIP) ledColor = BLUE;
                else      ledColor = RED;
                
            }
            
            if (devState == CTR_ONOFF_ON)
            {
                board_sw_control(RELAY_SW, SW_ON);
            #ifdef CONFIG_ESP32C3_PLUG_RD_FPT 
                board_rgb_led_operation(ledColor,LED_BRIGHTNESS_UNKNOWN);
            #else
                gpio_set_level(LED_G, LED_ON);
            #endif

            }
            else if (devState == CTR_ONOFF_OFF)
            {
                board_sw_control(RELAY_SW, SW_OFF);
             #ifdef CONFIG_ESP32C3_PLUG_RD_FPT 
                board_rgb_led_operation(ledColor,LED_BRIGHTNESS_UNKNOWN);
            #else
                gpio_set_level(LED_G, LED_OFF);
            #endif

            }
            break;
        default:
            break;
        }
        return ESP_OK;
    }

    static void task_led_slow(void *pvParameters)
    {
        #ifdef CONFIG_ESP32C3_PLUG_RD_FPT 
            board_rgb_led_operation(ORANGE,LED_BRIGHTNESS_UNKNOWN);
        #else
        while (PROV_STATE != STEP_CFG_COMPLETE)
        {
            vTaskDelay(500);
            // board_led_operation(LED_B, LED_OFF);
            gpio_set_level(LED_R, LED_OFF);
            vTaskDelay(500);
            // board_led_operation(LED_B, LED_ON);
            gpio_set_level(LED_R, LED_ON);
        }
        #endif
        ledIndicateHandle = NULL;
        vTaskDelete(NULL);
    }

    static void task_led_fast(void *pvParameters)
    {
    #if(CONFIG_ESP32C3_PLUG_RD_FPT == 1)
        uint8_t ledColor = *(uint8_t *)pvParameters;
        // ESP_LOGI(TAG," led color %d ",ledColor);
        uint32_t lastTime = xTaskGetTickCount();
        while (xTaskGetTickCount() - lastTime <= 5000)
        {
            vTaskDelay(100);
            board_rgb_led_operation(0,100);//,0);
            vTaskDelay(100);
            board_rgb_led_operation(ledColor,100);//,100);
        }


        // if (ledIndicateHandle != NULL)
        // {
        //     vTaskResume(ledIndicateHandle);
        // }
        // else 
        // {
        //     // board_rgb_led_operation(BLUE, 20);

        // }

    #else

        uint32_t lastTime = xTaskGetTickCount();
        while (xTaskGetTickCount() - lastTime <= 5000)
        {
            vTaskDelay(100);
            gpio_set_level(LED_R, LED_OFF);
            vTaskDelay(100);
            gpio_set_level(LED_R, LED_ON);
        }
        if (ledIndicateHandle != NULL)
        {
            vTaskResume(ledIndicateHandle);
        }
    #endif
        vTaskDelete(NULL);
    }

    void root_device_identify(void)
    {
        uint8_t ledColor = ORANGE;
        xTaskCreate(task_led_fast, "task_led_fast", 1024, (void*)&ledColor, 3, NULL);
        vTaskDelay(1);
    }
    static void task_prv_cmplt(void *pvParameters){
        vTaskDelay(200);
        // uint8_t *devState = NULL;
        // uint8_t devStateSize = 0;
        // if (root_device_get_state((void **)&devState, &devStateSize) == ESP_OK)
        // {
        //     ESP_LOG_BUFFER_HEX("ROOT DEVICE STATE", devState, devStateSize);
        //     root_device_control(FEATURE_ONOFF, devState);
        // }
        // else
        // {
        //     printf("not ok \n");
        // }
                  uint8_t tempState[2]={0};
        tempState[1]= localDevState;
        root_device_set_state(FEATURE_ONOFF, (uint8_t *)tempState);
        vTaskDelete(NULL);

    }

    void root_device_prov_complete(void)
    {
        #if(CONFIG_ESP32C3_PLUG_RD_FPT == 1)
        if(gotIP) board_rgb_led_operation(BLUE,LED_BRIGHTNESS_UNKNOWN);
        else              board_rgb_led_operation(RED,LED_BRIGHTNESS_UNKNOWN);
        #else
        gpio_set_level(LED_R, LED_OFF);
        #endif
        xTaskCreate(task_prv_cmplt, "task_prv_cmplt", 3*1024, NULL, 10, NULL);

// 
                //   uint8_t tempState[2]={0};
        // tempState[1]= localDevState;
        // root_device_set_state(FEATURE_ONOFF, tempState);
        // root_device_control(FEATURE_ONOFF, tempState);

        // vTaskDelay(100);
   
    //     uint8_t *devState = NULL;
    //     uint8_t devStateSize = 0;
    //     if (root_device_get_state(&devState, &devStateSize) == ESP_OK){
    //         ESP_LOG_BUFFER_HEX("ROOT DEVICE STATE", devState, devStateSize);
    //         devState[1] = !devState[1];
    //         root_device_set_state(FEATURE_ONOFF, devState);
    //         root_device_control(FEATURE_ONOFF, devState);
    //   }

    }


    void root_device_network_indicate(esp_event_base_t event_base, int32_t event_id, void* event_data){
        if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        #if (CONFIG_ESP32C3_PLUG_RD_FPT == 1)

            board_rgb_led_operation(BLUE,LED_BRIGHTNESS_UNKNOWN);
            gotIP = 1;
        #else 
            gpio_set_level(LED_B, LED_OFF);
        #endif
        }
        else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        #if (CONFIG_ESP32C3_PLUG_RD_FPT == 1)
            if(PROV_STATE == STEP_CFG_COMPLETE){
                board_rgb_led_operation(RED,LED_BRIGHTNESS_UNKNOWN);
            }
            gotIP = 0;
        #else
            gpio_set_level(LED_B, LED_ON);
        #endif
        }

    }

    esp_err_t rgmgt_device_event_cb(uint16_t event){
        switch (event){
            case CTR_WILE_EVT_WIFI_CONNECTED:
                #if (CONFIG_ESP32C3_PLUG_RD_FPT == 1)
                board_rgb_led_operation(BLUE,LED_BRIGHTNESS_UNKNOWN);
                gotIP = 1;
                #else 
                gpio_set_level(LED_B, LED_OFF);
                #endif
                break;
            case CTR_WILE_EVT_WIFI_DISCONNECTED:
                #if (CONFIG_ESP32C3_PLUG_RD_FPT == 1)
                if(PROV_STATE == STEP_CFG_COMPLETE){
                    board_rgb_led_operation(RED,LED_BRIGHTNESS_UNKNOWN);
                }
                gotIP = 0;
                #else
                gpio_set_level(LED_B, LED_ON);
                #endif
                break;
            case CTR_WILE_EVT_WIFI_CONNECT_FAIL:
                break;
            case CTR_WILE_EVT_CLOUD_CONNECTED:
                break;
            case CTR_WILE_EVT_CLOUD_DISCONNECTED:
                break;
            default:
                break;
        }
        return ESP_OK;
    }
    
    void root_device_prov_none(void)
    {
        // vTaskDelay(100);

        #if(CONFIG_ESP32C3_PLUG_RD_FPT == 1)
        board_rgb_led_operation(ORANGE,LED_BRIGHTNESS_UNKNOWN);//get_relay_state() ? 100 : 20);
        board_rgb_led_operation(ORANGE,LED_BRIGHTNESS_UNKNOWN);//get_relay_state() ? 100 : 20);
        #else 
            xTaskCreate(task_led_slow, "task_led_slow", 512, NULL, tskIDLE_PRIORITY, &ledIndicateHandle);

        #endif
    }

    void root_device_delete_indicate()
    {
        uint8_t ledColor = BLUE;
        isReset = 1;
        xTaskCreate(task_led_fast, "task_led_fast", 1024, &ledColor, 3, NULL);
        vTaskDelay(1);
    }
    esp_err_t root_device_set_state(uint16_t type, uint8_t *value)
    {
        // return set_device_state(rootEID, type, value, true);
        // uint16_t rootElm = rgmgt_device_get_elmid_idx(rootEID, 0x01);
        // return rgmgt_device_set_state(rootEID, rootElm, type, value, true);

        uint8_t *elmInfo = NULL;
        size_t elmInfoLen = 0;
        rgmgt_device_get_state(rootEID, (void **)&elmInfo, &elmInfoLen);
        uint16_t rootElm = rgmgt_device_get_elmid(0x01, elmInfo, elmInfoLen);
        if (elmInfo != NULL) free(elmInfo);
        return rgmgt_device_set_state(rootEID, rootElm, type, value, true, true);
    }

    esp_err_t root_device_get_state(void **state, uint8_t *stateSize)
    {
        uint8_t *elmInfo = NULL;
        size_t elmInfoLen = 0;
        rgmgt_device_get_state(rootEID, (void **)&elmInfo, &elmInfoLen);
        ESP_LOG_BUFFER_HEX("STATE", elmInfo, elmInfoLen);

        uint8_t *rootState = NULL;
        size_t   rootStateLen = 0;
        esp_err_t ret = rgmgt_device_get_state_elm_attr(0x01, FEATURE_ONOFF, elmInfo, elmInfoLen, (void **)&rootState, &rootStateLen);
        if (ret == ESP_OK){
            ESP_LOG_BUFFER_HEX("STATE OUT", rootState, rootStateLen);
            if (*state != NULL) free(*state);
            *stateSize = rgmsg_feature_size(FEATURE_ONOFF);
            *state = malloc(*stateSize);
            *((uint8_t *)*state+1) = rootState[0];
        }
        if (elmInfo != NULL) free(elmInfo);
        if (rootState != NULL) free(rootState);
        return ret;

        // return get_local_device_state(rootEID, state, stateSize);
    }

#ifdef __cplusplus
}
#endif