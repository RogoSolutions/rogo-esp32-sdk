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
#include "iot_button.h"

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
#define ELM_NUM             1
#define BTN_NUM             ELM_NUM
#define LED_NUM             ELM_NUM
#define SW_NUM              ELM_NUM
#define LED_GPIO            false
#define LED_RGB_GPIO        false
#define LED_RGB_WS2812      true
#define LED_INDICATOR_MULTI false
#define BUTTON_ACTIVE_LEVEL 0
#define LED_1               GPIO_NUM_8
#define BTN_1               GPIO_NUM_9
#define SW_1                GPIO_NUM_2
#elif defined(CONFIG_ESP32C3_ROGO_DEV)
#define ELM_NUM             1
#define BTN_NUM             ELM_NUM
#define LED_NUM             ELM_NUM
#define SW_NUM              ELM_NUM
#define LED_GPIO            true
#define LED_RGB_GPIO        false
#define LED_RGB_WS2812      false
#define LED_INDICATOR_MULTI true
#define BUTTON_ACTIVE_LEVEL 0
#define LED_ACTIVE_LEVEL    0
#define LED_1               GPIO_NUM_8
#define BTN_1               GPIO_NUM_9
#define SW_1                GPIO_NUM_2
#elif defined(CONFIG_ESP32C3_ROGO_SWITCH_3)
#define ELM_NUM             3
#define BTN_NUM             ELM_NUM
#define LED_NUM             ELM_NUM
#define SW_NUM              ELM_NUM
#define LED_GPIO            true
#define LED_INDICATOR_MULTI true
#define LED_ACTIVE_LEVEL    0
#define BUTTON_ACTIVE_LEVEL 0
#define LED_1               GPIO_NUM_20
#define LED_2               GPIO_NUM_10
#define LED_3               GPIO_NUM_2
#define LED_4               GPIO_NUM_21
#define BTN_1               GPIO_NUM_0
#define BTN_2               GPIO_NUM_7
#define BTN_3               GPIO_NUM_8
#define BTN_4               GPIO_NUM_9
#define SW_1                GPIO_NUM_6
#define SW_2                GPIO_NUM_5
#define SW_3                GPIO_NUM_4
#define SW_4                GPIO_NUM_1
#elif defined(CONFIG_ESP32C6_DEV)
#define ELM_NUM             1
#define BTN_NUM             ELM_NUM
#define LED_NUM             ELM_NUM
#define SW_NUM              ELM_NUM
#define LED_GPIO            false
#define LED_RGB_GPIO        false
#define LED_RGB_WS2812      true
#define LED_INDICATOR_MULTI false
#define BUTTON_ACTIVE_LEVEL 0
#define LED_1               GPIO_NUM_8
#define BTN_1               GPIO_NUM_9
#define SW_1                GPIO_NUM_2
#elif defined(CONFIG_ESP32S3_DEV)
#define ELM_NUM             1
#define BTN_NUM             ELM_NUM
#define LED_NUM             ELM_NUM
#define SW_NUM              ELM_NUM
#define LED_GPIO            false
#define LED_RGB_GPIO        false
#define LED_RGB_WS2812      true
#define LED_INDICATOR_MULTI false
#define BUTTON_ACTIVE_LEVEL 0
#define LED_1               GPIO_NUM_38
#define BTN_1               GPIO_NUM_0
#define SW_1                GPIO_NUM_2
#endif

#ifndef LED_INDICATOR_MULTI
#define LED_INDICATOR_MULTI false
#endif

#ifndef LED_ACTIVE_LEVEL
#define LED_ACTIVE_LEVEL            1
#endif

#ifndef SWITCH_ACTIVE_LEVEL
#define SWITCH_ACTIVE_LEVEL         1
#endif

#ifndef BUTTON_ACTIVE_LEVEL
#define BUTTON_ACTIVE_LEVEL         0
#endif

#ifndef BUTTON_LONG_PRESS_TIME
#define BUTTON_LONG_PRESS_TIME      5000
#endif

#ifndef BUTTON_SHORT_PRESS_TIME
#define BUTTON_SHORT_PRESS_TIME     100
#endif

typedef struct button_state {
    uint8_t element;
    uint8_t current;
    uint8_t previous;
    button_handle_t handler;
    char *name;
} button_state_t;

typedef struct switch_state {
    uint8_t element;
    uint8_t current;
    uint8_t pin;
    bool set;
    char *name;
} switch_state_t;

esp_err_t board_init(void);
esp_err_t board_switch_control(uint8_t element, uint8_t onoff);

#ifdef __cplusplus
}
#endif

#endif
