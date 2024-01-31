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

#pragma once
#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "soc/soc_caps.h"
#include "driver/gpio_filter.h"
#include "esp_ble_mesh_defs.h"

#include "app_device.h"

#ifdef CONFIG_ESP_WROOM_32
#define LED_R GPIO_NUM_25
#define LED_G GPIO_NUM_26
#define LED_B GPIO_NUM_27
#elif defined(CONFIG_ESP_WROVER)
#define LED_R GPIO_NUM_0
#define LED_G GPIO_NUM_2
#define LED_B GPIO_NUM_23
#define SW_1  GPIO_NUM_33
#define SW_2  GPIO_NUM_32
#elif defined(CONFIG_ESP32C3_DEV)
#define BTN_NUM     0
#define LED_R       GPIO_NUM_8
#define LED_G       GPIO_NUM_8
#define LED_B       GPIO_NUM_8
#define BTN_1       GPIO_NUM_1
#define BTN_2       GPIO_NUM_20
#define BTN_3       GPIO_NUM_3
#define BTN_4       GPIO_NUM_10
#define SW_1        GPIO_NUM_2
#define SW_2        GPIO_NUM_4
#define SW_3        GPIO_NUM_5
#define SW_4        GPIO_NUM_6
#elif defined(CONFIG_ESP32C3_WM_TY2_V05_4SW)
#define BTN_NUM     4
#define LED_NUM     4
#define LED_R       GPIO_NUM_8
#define LED_G       GPIO_NUM_8
#define LED_B       GPIO_NUM_8
#define BTN_1       GPIO_NUM_1
#define BTN_2       GPIO_NUM_20
#define BTN_3       GPIO_NUM_3
#define BTN_4       GPIO_NUM_10  // GPIO_NUM_10
#define SW_1        GPIO_NUM_2
#define SW_2        GPIO_NUM_4
#define SW_3        GPIO_NUM_5
#define SW_4        GPIO_NUM_6
#elif defined(CONFIG_ESP32C3_RD_CN_04_V11)
#define BTN_NUM      4
#define LED_NUM      8
#define LED_RGB_CLK  GPIO_NUM_10
#define LED_RGB_DATA GPIO_NUM_3
#define BTN_1        GPIO_NUM_19
#define BTN_2        GPIO_NUM_7
#define BTN_3        GPIO_NUM_18
#define BTN_4        GPIO_NUM_6
#define SW_1         GPIO_NUM_0
#define SW_2         GPIO_NUM_1
#define SW_3         GPIO_NUM_2
#define SW_4         GPIO_NUM_4
#define TOUCH_EN     GPIO_NUM_8
#define ZERO_CROSS   GPIO_NUM_5
#define SW_RELAY_TIME      5600
#define SW_TIME_ON   3800
#define SW_TIME_OFF  5600
// #define SW_TIME_ON   5000
// #define SW_TIME_OFF  5600
// #define ZERO_CROSS_HALF_PERIOD  1
// #define OSCILLOSCOPE_TEST       1
#define OSCILLOSCOPE_TEST_TRIG  GPIO_NUM_9
#elif defined(CONFIG_ESP32S3_DEV)
#define LED_R GPIO_NUM_47
#define LED_G GPIO_NUM_47
#define LED_B GPIO_NUM_47
#define SW_1  GPIO_NUM_33
#define SW_2  GPIO_NUM_32
#endif

#ifdef CONFIG_ESP32C3_WM_TY2_V05_4SW
#define SW_ON       0
#define SW_OFF      1
#else
#define SW_ON       1
#define SW_OFF      0
#endif

#define SW_INIT     2

#ifdef CONFIG_ESP32C3_RD_CN_04_V11
#define LED_ON          100
#define LED_OFF         20
#define LED_FLIP        0
#define BTN_IDLE        0
#define BTN_PRESS       1
#else
#define LED_ON          0
#define LED_OFF         1
#define LED_FLIP        0
#define BTN_IDLE        1
#define BTN_PRESS       0
#endif

#define BTN_HOLD        2
#define BTN_HOLD_2      3
#define BTN_HOLD_3      4

#define ZERO_SAMPLING   10  // Sample number on zero crossing pin

struct _led_state {
    uint8_t current;
    uint8_t previous;
    uint8_t pin;
    char *name;
};

extern struct button_state{
    uint8_t element;
    uint8_t current;
    uint8_t previous;
    uint8_t pin;
    char *name;
} btn_state[BTN_NUM];

extern struct switch_state{
    uint8_t element;
    uint8_t current;
    uint8_t pin;
    bool set;
    char *name;
} sw_state[BTN_NUM];

void board_prov_complete(void);

void board_led_operation(uint8_t pin, uint8_t onoff);

esp_err_t board_init(void);

void board_sw_control(uint8_t element, uint8_t onoff);

#ifdef ZERO_CROSS
void board_sw_zc_control(int8_t element, uint8_t onoff);
#endif

void board_test(void);

#ifdef __cplusplus
}
#endif

#endif
