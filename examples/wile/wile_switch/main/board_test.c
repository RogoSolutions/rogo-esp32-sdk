#include "board_test.h"

#if CONFIG_BOARD_TEST_ENABLE

void board_test_check(void){
    esp_err_t err = ESP_OK;
    nvs_handle_t nvs_handler;
    err = nvs_flash_init_partition(TEST_PARTITION);
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase_partition(TEST_PARTITION));
        err = nvs_flash_init_partition(TEST_PARTITION);
    }
    
    err = nvs_open_from_partition(TEST_PARTITION, TEST_NAMESPACE, NVS_READWRITE, &nvs_handler);
    uint8_t factoryTestState = 0;
    // uint8_t factoryTestTimer = 0;
    // err = nvs_get_u8(nvs_handler, "test_timer", &factoryTestTimer);
    err = nvs_get_u8(nvs_handler, "test_status", &factoryTestState);
    nvs_close(nvs_handler);
    printf("\n");
    ESP_LOGW("FACTORY_TEST", "STATE: %d, ret: %s", factoryTestState, esp_err_to_name(err));

    if (factoryTestState == TEST_STATE_DONE){
        ESP_LOGI("FACTORY_TEST", "TEST DONE RUNNING MAINPROGRAM ");
        return;
    }
    if (err == ESP_ERR_NVS_NOT_FOUND) factoryTestState = TEST_STATE_01;

    ESP_LOGW("FACTORY_TEST", "Entering factory test mode");
    for (uint8_t i=0; i<BTN_NUM; i++){
        gpio_isr_handler_remove(btn_state[i].pin);
    }

    switch (factoryTestState){
        case TEST_STATE_00:
        case TEST_STATE_01:
            board_test_01();
        case TEST_STATE_02:
            board_test_02();
            break;
        default:
            ESP_LOGE("FACTORY_TEST", "Error wrong factory test code %d", factoryTestState);
            return;
    }
    esp_restart();
}

void board_test_01(void){
    uint8_t testTimePassed = 0;
    nvs_handle_t nvs_handler;
    nvs_open_from_partition(TEST_PARTITION, TEST_NAMESPACE, NVS_READONLY, &nvs_handler);
    if (nvs_get_u8(nvs_handler, "test_timer", &testTimePassed) != ESP_OK) testTimePassed = 0;
    nvs_close(nvs_handler);

    board_test_save("test_status", TEST_STATE_01);
    ESP_LOGW("FACTORY_TEST_01", "Time left: %d", TEST_01_DURATION - testTimePassed);

    TickType_t xLastTest     = xTaskGetTickCount();
    TickType_t xLastTestSave = xTaskGetTickCount();

    board_led_rgb_set_color(LED_ALL, LED_RED, false);
    root_device_state_init();

    while (testTimePassed < TEST_01_DURATION){
        // ESP_LOGI("FACTORY_TEST_01", "Time since start %ld", xTaskGetTickCount());

        if (xTaskGetTickCount() - xLastTest >= TEST_01_PERIOD_TEST){
            switch (led_rgb_state[0].current.hue){
                case LED_RED:
                    board_led_rgb_set_color(LED_ALL, LED_GREEN, false);
                    break;
                case LED_GREEN:
                    board_led_rgb_set_color(LED_ALL, LED_BLUE, false);
                    break;
                case LED_BLUE:
                    board_led_rgb_set_color(LED_ALL, LED_RED, false);
                    break;
            }

            for (uint8_t i=0; i<BTN_NUM; i++){
                root_device_local_control(i+1, CTR_ONOFF_FLIP);
            }
            xLastTest = xTaskGetTickCount();
        }

        for (uint8_t i=0; i<BTN_NUM; i++){
            if (gpio_get_level(btn_state[i].pin) == BTN_PRESS){
                board_led_rgb_set_saturation(btn_state[i].element, 0);
            }
            else{
                board_led_rgb_set_saturation(btn_state[i].element, 100);
            }
        }

        if (xTaskGetTickCount() - xLastTestSave >= TEST_01_PERIOD_SAVE){
            testTimePassed++;
            ESP_LOGW("FACTORY_TEST_01", "Time passed: %d", testTimePassed);
            board_test_save("test_timer", testTimePassed);
            xLastTestSave = xTaskGetTickCount();
        }
        vTaskDelay(100);
    }

    board_test_save("test_timer", testTimePassed);
    ESP_LOGW("FACTORY_TEST_01", "Finised");

    board_led_rgb_set_saturation(LED_ALL, 0); // WHITE
    root_device_state_init();

    TickType_t xLastHoldTime = xTaskGetTickCount();
    uint8_t btnPressNum = 0;
    while (xTaskGetTickCount() - xLastHoldTime < 5000){
        btnPressNum = 0;
        for (int i=0; i<BTN_NUM; i++){
            if (gpio_get_level(btn_state[i].pin) == BTN_PRESS) btnPressNum++;
        }
        if (btnPressNum < 2){
            xLastHoldTime = xTaskGetTickCount();
        }
        else{
            ESP_LOGW("FACTORY_TEST_01", "Switching to Test 2 in %lds", 5 - ((xTaskGetTickCount() - xLastHoldTime) / 1000));
        }
        vTaskDelay(1000);
    }
}

void board_test_02(void){
    board_test_save("test_status", TEST_STATE_02);
    board_led_rgb_set_color(LED_ALL, LED_YELLOW, true);
    board_led_rgb_set_saturation(LED_ALL, 100); 
    deviceLedFlipNum = 4;
    xTaskCreate(board_led_rgb_flip_task, "board_led_rgb_flip_task", 1024, &deviceLedFlipNum, tskIDLE_PRIORITY, NULL);
    
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    wifi_init();
    wifi_config_t test_wifi_config = {
        .sta = {
            .ssid = TEST_WIFI_SSID,
            .password = TEST_WIFI_PWD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &test_wifi_config));
    ESP_ERROR_CHECK(esp_wifi_connect());
    vTaskDelay(2000);

    wifi_ap_record_t ap;
    while(esp_wifi_sta_get_ap_info(&ap) != ESP_OK){
        vTaskDelay(1000);
    }
    if (ap.rssi < -60){
        ESP_LOGW("FACTORY_TEST_02", "WIFI RSSI %d", ap.rssi);
        board_led_rgb_set_color(LED_ALL, LED_RED, true);
        while (ap.rssi < -60){
            board_led_rgb_set_brightness(LED_ALL, LED_ON);
            vTaskDelay(500);
            board_led_rgb_set_brightness(LED_ALL, LED_OFF);
            vTaskDelay(500);
            esp_wifi_sta_get_ap_info(&ap);
        }
    }
    ESP_LOGW("FACTORY_TEST_02", "WIFI RSSI %d", ap.rssi);
    
    TickType_t xLastTest = xTaskGetTickCount();
    TickType_t xLastHoldTime = xTaskGetTickCount();
    bool anyPress = true;
    uint8_t btnPressNum = 0;
    while (1){
        if (xTaskGetTickCount() - xLastTest >= TEST_02_PERIOD_TEST){
            switch (led_rgb_state[0].current.hue){
                case LED_YELLOW:
                    board_led_rgb_set_color(LED_ALL, LED_RED, false);
                    break;
                case LED_RED:
                    board_led_rgb_set_color(LED_ALL, LED_GREEN, false);
                    break;
                case LED_GREEN:
                    board_led_rgb_set_color(LED_ALL, LED_BLUE, false);
                    break;
                case LED_BLUE:
                    board_led_rgb_set_color(LED_ALL, LED_RED, false);
                    break;
            }

            for (uint8_t i=0; i<BTN_NUM; i++){
                root_device_local_control(i+1, CTR_ONOFF_FLIP);
            }
            xLastTest = xTaskGetTickCount();
        }
        anyPress = true;
        xLastHoldTime = xTaskGetTickCount();
        while (anyPress){
            anyPress = false;
            btnPressNum = 0;
            for (uint8_t i=0; i<BTN_NUM; i++){
                if (gpio_get_level(btn_state[i].pin) == BTN_PRESS){
                    anyPress = true;
                    btnPressNum++;
                    board_led_rgb_set_saturation(btn_state[i].element, 0);
                    root_device_local_control(btn_state[i].element, CTR_ONOFF_ON);
                }
                else{
                    board_led_rgb_set_saturation(btn_state[i].element, 100);
                }
            }
            if (btnPressNum < 2) xLastHoldTime = xTaskGetTickCount();
            else{
                if ((xTaskGetTickCount() - xLastHoldTime >= 5000)){
                    board_led_rgb_set_color(LED_ALL, LED_YELLOW, true);
                    deviceLedFlipNum = 2;
                    xTaskCreate(board_led_rgb_flip_task, "board_led_rgb_flip_task", 1024, &deviceLedFlipNum, tskIDLE_PRIORITY, NULL);
                    ESP_LOGW("FACTORY_TEST_02", "Finised");
                    board_test_save("test_status", TEST_STATE_DONE);
                    vTaskDelay(1000);
                    return;
                }
            }
            // if ((xTaskGetTickCount() - xLastHoldTime >= 5000) && btnPressNum >= 2) return;
            vTaskDelay(100);
        }
        vTaskDelay(100);
    }
}

void board_test_save(const char *key, uint8_t value){
    nvs_handle_t nvs_handler;
    nvs_open_from_partition(TEST_PARTITION, TEST_NAMESPACE, NVS_READWRITE, &nvs_handler);
    ESP_LOGI("FACTORY_TEST", "Saving %s: 0x%02X", key, value);
    nvs_set_u8(nvs_handler, key, value);
    nvs_commit(nvs_handler);
    nvs_close(nvs_handler);
}

#endif // CONFIG_BOARD_TEST_ENABLE