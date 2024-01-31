// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>

#include "driver/gpio.h"
#include "board.h"
#include "board_led.h"

#define TAG "BOARD"

#ifdef CONFIG_ESP32C3_WM_TY2_V05_4SW
struct _led_state led_state[3] = {
    { LED_OFF, LED_OFF, LED_R, "red"   },
    { LED_OFF, LED_OFF, LED_G, "green" },
    { LED_OFF, LED_OFF, LED_B, "blue"  },
};
struct button_state btn_state[BTN_NUM] = {
    { 1, BTN_IDLE, BTN_IDLE, BTN_1, "1_left_max"  },
    { 2, BTN_IDLE, BTN_IDLE, BTN_2, "2_left_mid"  },
    { 3, BTN_IDLE, BTN_IDLE, BTN_3, "3_right_mid" },
    { 4, BTN_IDLE, BTN_IDLE, BTN_4, "4_right_max" },
};
struct switch_state sw_state[BTN_NUM] = {
    { 1, SW_OFF, SW_1, false, "1_left_max"  },
    { 2, SW_OFF, SW_2, false, "2_left_mid"  },
    { 3, SW_OFF, SW_3, false, "3_right_mid" },
    { 4, SW_OFF, SW_4, false, "4_right_max" },
};
#endif

#ifdef CONFIG_ESP32C3_RD_CN_04_V11
struct button_state btn_state[BTN_NUM] = {
    { 1, BTN_IDLE, BTN_IDLE, BTN_1, "1_lower_left"  },
    { 2, BTN_IDLE, BTN_IDLE, BTN_2, "2_upper_left"  },
    { 3, BTN_IDLE, BTN_IDLE, BTN_3, "3_lower_right" },
    { 4, BTN_IDLE, BTN_IDLE, BTN_4, "4_upper_right" },
};
struct switch_state sw_state[BTN_NUM] = {
    { 1, SW_INIT, SW_1, false, "L1" },
    { 2, SW_INIT, SW_2, false, "L2" },
    { 3, SW_INIT, SW_3, false, "L3" },
    { 4, SW_INIT, SW_4, false, "L4" },
};
#endif

#ifdef CONFIG_ESP32C3_DEV
struct _led_state led_state[3] = {
    { LED_OFF, LED_OFF, LED_R, "red"   },
    { LED_OFF, LED_OFF, LED_G, "green" },
    { LED_OFF, LED_OFF, LED_B, "blue"  },
};
struct button_state btn_state[BTN_NUM] = {};
struct switch_state sw_state[BTN_NUM] = {};
#endif

void board_led_operation(uint8_t pin, uint8_t onoff)
{
    #ifdef CONFIG_ESP32C3_RD_CN_04_V11

    #else
    for (int i = 0; i < 3; i++) {
        if (led_state[i].pin != pin) {
            continue;
        }
        if (onoff == led_state[i].previous) {
            ESP_LOGI(TAG, "led %s is already %s",
                     led_state[i].name, (onoff ? "on" : "off"));
            return;
        }
        gpio_set_level(pin, onoff);
        led_state[i].previous = onoff;
        return;
    }
    #endif

    ESP_LOGE(TAG, "LED is not found!");
}

// static void task_led_flip(void *pvParameters)
// {
//     uint8_t flipNum = *((uint8_t *)pvParameters);
//     for (uint8_t i=0; i<flipNum; i++){
//         board_led_rgb_set_brightness(LED_ALL, LED_ON);
//         vTaskDelay(200);
//         board_led_rgb_set_brightness(LED_ALL, LED_OFF);
//         vTaskDelay(200);
//     };
//     for (uint8_t i=0; i<BTN_NUM; i++){
//         if (sw_state[i].current == CTR_ONOFF_ON){
//             board_led_rgb_set_brightness(sw_state[i].element, LED_ON);
//         }
//         else{
//             board_led_rgb_set_brightness(sw_state[i].element, LED_OFF);
//         }
//     }
//     vTaskDelete(NULL);
// }

TaskHandle_t btnTask = NULL;

static void button_task(void* arg)
{
    struct button_state *btn = (struct button_state *) arg;
    // ESP_LOGW("WILE", "Button Press: GPIO_%d, level: %d", btn->pin, gpio_get_level(btn->pin));
    vTaskDelay(50);
    if (gpio_get_level(btn->pin) != BTN_PRESS){
        btnTask = NULL;
        vTaskDelete(NULL);
    }

    uint8_t btnPressNum = 0;
    for (int i=0; i<BTN_NUM; i++){
        if (gpio_get_level(btn_state[i].pin) == BTN_PRESS){
            btn_state[i].current = BTN_PRESS;
            btnPressNum++;
        }
        else{
            btn_state[i].current = BTN_IDLE;
        }
    }

    #ifdef CONFIG_ESP32C3_WM_TY2_V05_4SW
    if (btn->pin == BTN_4){
        // esp_restart();
        root_device_factory_reset();
    }
    #endif
    #ifdef CONFIG_ESP32C3_RD_CN_04_V11
    if (btn->pin == BTN_3){
        // esp_restart();
        // root_device_factory_reset();
    }
    #endif

    for (int i=0; i<BTN_NUM; i++){
        if (btn_state[i].current == BTN_PRESS){
            ESP_LOGW("WILE", "Button Press: %s", btn_state[i].name);
            root_device_local_control(btn_state[i].element, CTR_ONOFF_FLIP);
            // btn_state[i].previous = btn_state[i].current;
            btn_state[i].current = BTN_IDLE;
        }
    }

    if (btnPressNum > 1){
        TickType_t xLastWakeTime = xTaskGetTickCount();
        while (gpio_get_level(btn->pin) == BTN_PRESS){
            if(xTaskGetTickCount()-xLastWakeTime >= 5000){
                // root_device_factory_reset();
                #ifdef CONFIG_ESP32C3_RD_CN_04_V11
                deviceLedFlipNum = 2;
                xTaskCreate(board_led_rgb_flip_task, "board_led_rgb_flip_task", 1024, &deviceLedFlipNum, tskIDLE_PRIORITY, NULL);
                #endif
                for(int i=0; i<BTN_NUM; i++){
                    if(gpio_get_level(btn_state[i].pin) == BTN_PRESS){
                        if (btn_state[i].previous == BTN_HOLD){
                            ESP_LOGW("BOARD", "HOLD 2");
                            btn_state[i].current = BTN_HOLD_2;
                        }
                        else if (btn_state[i].previous == BTN_HOLD_2){
                            ESP_LOGW("BOARD", "HOLD 3");
                            btn_state[i].current = BTN_HOLD_3;
                        }
                        else{
                            ESP_LOGW("BOARD", "HOLD 1");
                            btn_state[i].current = BTN_HOLD;
                        }
                    }
                }
                break;
            }
            vTaskDelay(200);
        }
    }
    bool doReset = false;
    for(int i=0; i<BTN_NUM; i++){
        btn_state[i].previous = btn_state[i].current;
        if (btn_state[i].current == BTN_HOLD_3) doReset = true;
    }
    if (doReset) root_device_factory_reset();
    btnTask = NULL;
    vTaskDelete(NULL);
}

static void IRAM_ATTR button_gpio_isr_handler(void* arg){
    struct button_state *btn = (struct button_state *) arg;
    if (gpio_get_level(btn->pin) == BTN_PRESS && btnTask == NULL){
        xTaskCreate(button_task, "button_task", 1024*3, (void *)btn, 10, &btnTask);
    }
}

// esp_timer_handle_t switch_timer_handler;

// static void board_sw_relay_control(void *arg)
// {
//     // gpio_set_level(SW1, localDevState);
//     int8_t element = 0;
//     gpio_set_level(sw_state[element].pin, sw_state[element].current);
//     esp_timer_stop(switch_timer_handler);
//     ESP_LOGI("Zero Crossing"," Relay latched ");
// }

// esp_timer_create_args_t switch_timer_args = {
//     .callback = &board_sw_relay_control,
//     .arg = NULL,
//     .name = "Switch Relay Timer"
// };

void board_sw_control(uint8_t element, uint8_t onoff){
    // gpio_set_level(pin, onoff);
    uint8_t swIdx = 0;
    for (swIdx = 0; swIdx < BTN_NUM; swIdx++){
        if (element == sw_state[swIdx].element) break;
    }

    if (sw_state[swIdx].current == onoff){
        ESP_LOGW("BOARD", "SW %s already %s", sw_state[swIdx].name, (onoff ? "on" : "off"));
        return;
    }

    sw_state[swIdx].current = onoff;
    sw_state[swIdx].set = false;
    gpio_set_level(sw_state[swIdx].pin, onoff);
    sw_state[swIdx].set = true;
}

#ifdef ZERO_CROSS
#ifdef ZERO_CROSS_HALF_PERIOD
uint16_t acHalfPeriod = 0;
void sw_zerocross_task(void *arg){
    struct switch_state *sw = (struct switch_state *) arg;
    // ESP_LOGW("SW", "GPIO: %d", sw->pin);

    int64_t lastCrossing = 0;
    int64_t currentCrossing = 0;
    uint8_t acPhase = 0;
    uint8_t sampleCnt = 0;

check_zero_cross:
    acPhase = gpio_get_level(ZERO_CROSS);
    while (sampleCnt < ZERO_SAMPLING){
        if (gpio_get_level(ZERO_CROSS) == acPhase){
            sampleCnt++;
        }
        else{
            acPhase = gpio_get_level(ZERO_CROSS);
            sampleCnt = 0;
        }
    }
    sampleCnt = 0;
    while (sampleCnt < ZERO_SAMPLING){
        if (gpio_get_level(ZERO_CROSS) == !acPhase){
            sampleCnt++;
            if (sampleCnt == 1) lastCrossing = esp_timer_get_time();
        }
        else sampleCnt = 0;
    }
    acPhase = gpio_get_level(ZERO_CROSS);

    sampleCnt = 0;
    while (sampleCnt < ZERO_SAMPLING){
        if (gpio_get_level(ZERO_CROSS) == !acPhase){
            sampleCnt++;
            if (sampleCnt == 1) currentCrossing = esp_timer_get_time();
        }
        else sampleCnt = 0;
    }

    uint16_t acHalfPeriodTemp = currentCrossing - lastCrossing;

    if (acHalfPeriodTemp < 8000 || acHalfPeriodTemp > 11000) goto check_zero_cross;
    if (acHalfPeriod == 0) acHalfPeriod = acHalfPeriodTemp;
    else                   acHalfPeriod = (acHalfPeriod+acHalfPeriodTemp)/2;

    int64_t relayTime = esp_timer_get_time();
    uint64_t relayDelayTime = acHalfPeriod - (relayTime - currentCrossing) - 4500;
    while (esp_timer_get_time() - relayTime < relayDelayTime) {}
    gpio_set_level(sw->pin, sw->current);
    sw->set = true;

    // esp_timer_start_once(switch_timer_handler, relayTime);

    ESP_LOGI("AC", "Period: %dus", acHalfPeriod);
    vTaskDelete(NULL);
}
#else
uint16_t acPeriod = 0;
void sw_zerocross_task(void *arg){
    struct switch_state *sw = (struct switch_state *) arg;
    // ESP_LOGW("SW", "GPIO: %d", sw->pin);

    int64_t lastCrossing = 0;
    int64_t currentCrossing = 0;
    uint8_t acPhase = 1;
    uint8_t sampleCnt = 0;
    uint8_t checkCrossCnt = 0;
    uint16_t acPeriodTemp = 0;
    bool crossCheck = false;
    TickType_t xLastWakeTime = xTaskGetTickCount();

// check_zero_cross:
    while ((acPeriodTemp < 15000 || acPeriodTemp > 21000) && checkCrossCnt <= 5){
        checkCrossCnt++;
        while (sampleCnt < ZERO_SAMPLING){
            if (gpio_get_level(ZERO_CROSS) == acPhase){
                sampleCnt++;
            }
            else{
                sampleCnt = 0;
            }
            if (xTaskGetTickCount()-xLastWakeTime >= 100) goto sw_zerocross_task_end;
        }
        sampleCnt = 0;
        while (sampleCnt < ZERO_SAMPLING){
            if (gpio_get_level(ZERO_CROSS) == !acPhase){
                sampleCnt++;
                if (sampleCnt == 1) lastCrossing = esp_timer_get_time();
            }
            else sampleCnt = 0;
            if (xTaskGetTickCount()-xLastWakeTime >= 100) goto sw_zerocross_task_end;
        }
        acPhase = !acPhase;

        sampleCnt = 0;
        while (sampleCnt < ZERO_SAMPLING){
            if (gpio_get_level(ZERO_CROSS) == !acPhase){
                sampleCnt++;
            }
            else sampleCnt = 0;
        }
        acPhase = !acPhase;
        sampleCnt = 0;
        while (sampleCnt < ZERO_SAMPLING){
            if (gpio_get_level(ZERO_CROSS) == !acPhase){
                sampleCnt++;
                if (sampleCnt == 1) currentCrossing = esp_timer_get_time();
            }
            else sampleCnt = 0;
        }
        acPeriodTemp = currentCrossing - lastCrossing;
        if (acPeriodTemp >= 15000 && acPeriodTemp <= 21000) crossCheck = true;
        // uint16_t acPeriodTemp = currentCrossing - lastCrossing;
        // if (acPeriodTemp < 15000 || acPeriodTemp > 21000) goto check_zero_cross;
    }

sw_zerocross_task_end:
    if (crossCheck){
        if (acPeriod == 0) acPeriod = acPeriodTemp;
        else               acPeriod = (acPeriod+acPeriodTemp)/2;

        int64_t relayTime = esp_timer_get_time();
        uint64_t relayDelayTime = 0;
        if (sw->current == CTR_ONOFF_ON){
            relayDelayTime = acPeriod/2 - (relayTime - currentCrossing) - SW_TIME_ON;
        }
        else{
            relayDelayTime = acPeriod/2 - (relayTime - currentCrossing) - SW_TIME_OFF;
        }
        // uint64_t relayDelayTime = acPeriod/2 - (relayTime - currentCrossing) - SW_RELAY_TIME;
        while (esp_timer_get_time() - relayTime < relayDelayTime) {}
    }
    else{
        ESP_LOGE("BOARD", "ZeroCross Fail");
        if (PROV_STATE == STEP_CFG_COMPLETE) board_led_rgb_set_color(LED_ALL, LED_RED, true);
    }
    gpio_set_level(sw->pin, sw->current);

    #ifdef OSCILLOSCOPE_TEST
    gpio_set_level(OSCILLOSCOPE_TEST_TRIG, SW_ON);
    #endif

    sw->set = true;

    #ifdef OSCILLOSCOPE_TEST
    vTaskDelay(1);
    gpio_set_level(OSCILLOSCOPE_TEST_TRIG, SW_OFF);
    #endif

    // esp_timer_start_once(switch_timer_handler, relayTime);

    ESP_LOGI("AC", "Period: %dus", acPeriod);
    vTaskDelete(NULL);
}
#endif // ZERO_CROSS_HALF_PERIOD

void board_sw_zc_control(int8_t element, uint8_t onoff){
    uint8_t swIdx = 0;
    for (swIdx = 0; swIdx < BTN_NUM; swIdx++){
        if (element == sw_state[swIdx].element) break;
    }

    if (sw_state[swIdx].current == onoff){
        ESP_LOGW("BOARD", "SW %s already %s", sw_state[swIdx].name, (onoff ? "on" : "off"));
        return;
    }

    sw_state[swIdx].current = onoff;
    sw_state[swIdx].set = false;

    // gpio_isr_handler_add(ZERO_CROSS, zero_crossing_freq_count, (void *)ZERO_CROSS);
    xTaskCreate(sw_zerocross_task, "sw_zerocross_task", 1024*2, &(sw_state[swIdx]), configMAX_PRIORITIES - 1, NULL);

    // sw_state[element].set = true;
}
#endif // ZERO_CROSS

static void board_led_init(void)
{
    #ifdef CONFIG_ESP32C3_RD_CN_04_V11
    board_led_rgb_init();
    #else
    for (int i = 0; i < 3; i++) {
        gpio_reset_pin(led_state[i].pin);
        gpio_set_direction(led_state[i].pin, GPIO_MODE_OUTPUT);
        gpio_set_level(led_state[i].pin, LED_OFF);
        led_state[i].previous = LED_OFF;
    }
    #endif
}

void board_switch_init(void){
    gpio_config_t gpio_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    for (int i=0; i<BTN_NUM; i++){
        // gpio_reset_pin(sw_state[i].pin);
        // gpio_set_direction(sw_state[i].pin, GPIO_MODE_OUTPUT);

        // gpio_conf.mode = GPIO_MODE_OUTPUT;
        gpio_conf.pin_bit_mask = (1ULL << sw_state[i].pin);
        // gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        // gpio_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        // gpio_conf.intr_type = GPIO_INTR_DISABLE;
        gpio_config(&gpio_conf);

        // gpio_set_level(sw_state[i].pin, SW_OFF);
    }

    #ifdef OSCILLOSCOPE_TEST
    gpio_conf.pin_bit_mask = (1ULL << OSCILLOSCOPE_TEST_TRIG);
    gpio_config(&gpio_conf);
    #endif

    uint8_t *devState = NULL;
    uint8_t devStateSize = 0;
    if (root_device_get_state((void *)&devState, &devStateSize) == ESP_OK){
        ESP_LOG_BUFFER_HEX("ROOT DEVICE STATE", devState, devStateSize);
        root_device_control(0x01, FEATURE_ONOFF, devState);
    }
    // if (devState == NULL) ESP_LOGE("DEVICE", "State NULL");
    if (devState != NULL) free(devState);
}

void board_button_init(void){
    gpio_config_t gpio_conf;
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);

    #ifdef CONFIG_ESP32C3_RD_CN_04_V11
    // gpio_reset_pin(TOUCH_EN);
    gpio_conf.mode = GPIO_MODE_OUTPUT;
    gpio_conf.pin_bit_mask = (1ULL << TOUCH_EN);
    gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&gpio_conf);
    gpio_set_level(TOUCH_EN, 0);

    for (int i=0; i<BTN_NUM; i++){
        // gpio_reset_pin(btn_state[i].pin);
        gpio_conf.intr_type = GPIO_INTR_POSEDGE;
        gpio_conf.mode = GPIO_MODE_INPUT;
        gpio_conf.pin_bit_mask = (1ULL << btn_state[i].pin);
        gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        gpio_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_config(&gpio_conf);
        // gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
        gpio_isr_handler_add(btn_state[i].pin, button_gpio_isr_handler, (void *)&(btn_state[i]));
    }

    #else
    for (int i=0; i<BTN_NUM; i++){
        // gpio_reset_pin(btn_state[i].pin);
        gpio_conf.intr_type = GPIO_INTR_NEGEDGE;
        gpio_conf.mode = GPIO_MODE_INPUT;
        gpio_conf.pin_bit_mask = (1ULL << btn_state[i].pin);
        gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_config(&gpio_conf);
        // gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
        gpio_isr_handler_add(btn_state[i].pin, button_gpio_isr_handler, (void *)&(btn_state[i]));
    }
    #endif
}

#ifdef ZERO_CROSS
void board_zerocross_init(void){
    gpio_config_t gpio_conf;
    // gpio_reset_pin(ZERO_CROSS);
    gpio_conf.intr_type = GPIO_INTR_NEGEDGE;
    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.pin_bit_mask = (1ULL << ZERO_CROSS);
    #ifdef ZERO_CROSS_HALF_PERIOD
    gpio_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    #else
    gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    #endif
    gpio_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&gpio_conf);

    #if SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER
    ESP_LOGI("GPIO", "Enable glitch filter");
    gpio_glitch_filter_handle_t glitch_filter;
    gpio_pin_glitch_filter_config_t glitch_filter_cfg = {
        .clk_src = GLITCH_FILTER_CLK_SRC_DEFAULT,
        .gpio_num = ZERO_CROSS,
    };
    gpio_new_pin_glitch_filter(&glitch_filter_cfg, &glitch_filter);
    gpio_glitch_filter_enable(glitch_filter);
    #endif

    // esp_timer_create(&switch_timer_args, &switch_timer_handler);
}
#endif

esp_err_t board_init(void){
    board_led_init();
    board_button_init();
    #ifdef ZERO_CROSS
    board_zerocross_init();
    #endif
    board_switch_init();
    root_device_state_init();
    return ESP_OK;
}