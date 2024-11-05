#pragma once
#ifndef _BOARD_LED_H_
#define _BOARD_LED_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "led_indicator.h"

#define BOARD_LED_ON                true
#define BOARD_LED_OFF               false
#if LED_RGB_WS2812
#define BOARD_LED_ON_BRIGHTNESS     LED_STATE_50_PERCENT
#define BOARD_LED_OFF_BRIGHTNESS    LED_STATE_25_PERCENT
#elif LED_GPIO
#define BOARD_LED_ON_BRIGHTNESS     LED_STATE_ON
#define BOARD_LED_OFF_BRIGHTNESS    LED_STATE_25_PERCENT
#else
#define BOARD_LED_ON_BRIGHTNESS     LED_STATE_ON
#define BOARD_LED_OFF_BRIGHTNESS    LED_STATE_25_PERCENT
#endif

#define BOARD_LED_RED_HUE           0
#define BOARD_LED_YELLOW_HUE        40
#define BOARD_LED_GREEN_HUE         120
#define BOARD_LED_PURPLE_HUE        300
#define BOARD_LED_BLUE_HUE          240

typedef struct led_state {
    uint8_t element;

    #if LED_RGB_WS2812
    uint8_t index;
    #endif

    int8_t current;
    int8_t previous;

    #if LED_INDICATOR_MULTI
    led_indicator_handle_t indicator;
    #endif
    char *name;
} led_state_t;

enum {
    BLINK_DOUBLE,
    BLINK_TRIPLE_FAST,
    BLINK_FAST,
    BLINK_BREATH,
    BLINK_COLOR_HSV_RING,
    BLINK_COLOR_RGB_RING,
    #if LED_NUM > 1
    BLINK_FLOWING,
    #endif
    BLINK_MAX,
};

esp_err_t board_led_init(void);
esp_err_t board_led_indicator_state(void);
esp_err_t board_led_indicator(uint8_t element, uint8_t mode);
esp_err_t board_led_indicator_stop(uint8_t element, uint8_t mode);
esp_err_t board_led_indicator_brightness(uint8_t element, uint8_t brightness);
esp_err_t board_led_indicator_color(uint8_t element, uint16_t hue);

#ifdef __cplusplus
}
#endif

#endif