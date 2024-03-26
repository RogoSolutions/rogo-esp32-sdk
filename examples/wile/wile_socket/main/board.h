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

#include "inttypes.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
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
#define LED_R       GPIO_NUM_8
#define LED_G       GPIO_NUM_8
#define LED_B       GPIO_NUM_8
#define RESET_SW    GPIO_NUM_4
#define RELAY_SW    GPIO_NUM_3

#elif defined(CONFIG_ESP32C3_PLUG_RD_FPT)
#define LED_R       GPIO_NUM_8
#define LED_G       GPIO_NUM_8
#define LED_B       GPIO_NUM_8
#define RESET_SW    GPIO_NUM_4
#define FLOAT_SW    GPIO_NUM_6
#define RELAY_SW         GPIO_NUM_3
#define RGB_LED_CLK GPIO_NUM_1
#define RGB_LED_DAT GPIO_NUM_5
#define ZRCRSS_PIN  GPIO_NUM_7
#elif defined(CONFIG_ESP32C3_PLUG_RD_TUYA)
#define LED_R       GPIO_NUM_4
#define LED_G       GPIO_NUM_5
#define LED_B       GPIO_NUM_8
#define RESET_SW    GPIO_NUM_6
#define RELAY_SW         GPIO_NUM_3
#elif defined(CONFIG_ESP32C3_PLUG_RD_TUYA3)
#define LED_R       GPIO_NUM_3
#define LED_G       GPIO_NUM_3
#define LED_B       GPIO_NUM_3
#define RESET_SW    GPIO_NUM_5
#define RELAY_SW    GPIO_NUM_6
#elif defined(CONFIG_ESP32S3_DEV)
#define LED_R GPIO_NUM_47
#define LED_G GPIO_NUM_47
#define LED_B GPIO_NUM_47
#define SW_1  GPIO_NUM_33
#define SW_2  GPIO_NUM_32
#elif defined(CONFIG_ESP32C3_PLUG_RD_CHINA)
#define LED_R       GPIO_NUM_6
#define LED_G       GPIO_NUM_6
#define LED_B       GPIO_NUM_6
#define RESET_SW    GPIO_NUM_1
#define RELAY_SW    GPIO_NUM_7



#endif

#define LED_ON  0
#define LED_OFF 1

#define SW_ON  1
#define SW_OFF 0

#define HIGH  1
#define LOW 0

struct _led_state {
    uint8_t current;
    uint8_t previous;
    uint8_t pin;
    char *name;
};

extern bool localDevState;
extern bool isReset;


void board_output_number(esp_ble_mesh_output_action_t action, uint32_t number);

void board_prov_complete(void);

void board_led_operation(uint8_t pin, uint8_t onoff);

esp_err_t board_init(void);

void board_led_init(void);
void board_sw_init(void);
void board_sw_control(uint8_t pin, uint8_t onoff);

void board_rgb_led_operation(uint8_t color, uint8_t brightness);
void two_wrie_led_bit_bag(uint8_t data);

void set_relay_state(bool state);
bool get_relay_state();

#define WHITE  5
#define ORANGE 4
#define GREEN  2
#define BLUE   3
#define RED    1

#define RED_MAX_VAL     255
#define GREEN_MAX_VAL   255
#define BLUE_MAX_VAL    255

#define LED_BRIGHTNESS_ON 100
#define LED_BRIGHTNESS_OFF 20
#define LED_BRIGHTNESS_UNKNOWN 0xFF

#define ZRCRSS_ON       3900
#define ZRCRSS_OFF      4700
#ifdef __cplusplus
}
#endif


#endif
