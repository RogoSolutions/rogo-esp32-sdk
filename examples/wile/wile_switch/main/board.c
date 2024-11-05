// Copyright 2024 Rogo Solutions, Jsc.

#include <stdio.h>

#include "driver/gpio.h"
#include "board.h"
#include "board_led.h"

button_state_t btn_state[BTN_NUM] = {
    #ifdef BTN_1
    { 1, BUTTON_NONE_PRESS, BUTTON_NONE_PRESS, NULL, "BTN_1" },
    #endif // BTN_1
    #ifdef BTN_2
    { 2, BUTTON_NONE_PRESS, BUTTON_NONE_PRESS, NULL, "BTN_2" },
    #endif // BTN_1
    #ifdef BTN_3
    { 3, BUTTON_NONE_PRESS, BUTTON_NONE_PRESS, NULL, "BTN_3" },
    #endif // BTN_1
};

switch_state_t sw_state[SW_NUM] = {
    #ifdef SW_1
    { 1, !SWITCH_ACTIVE_LEVEL, SW_1, false, "L1"  },
    #endif
    #ifdef SW_2
    { 2, !SWITCH_ACTIVE_LEVEL, SW_2, false, "L2"  },
    #endif
    #ifdef SW_3
    { 3, !SWITCH_ACTIVE_LEVEL, SW_3, false, "L3"  },
    #endif
};

TaskHandle_t btnTask = NULL;
esp_err_t    board_init(void);
esp_err_t    board_button_init(void);
esp_err_t    board_switch_init(void);
esp_err_t    board_switch_control(uint8_t element, uint8_t onoff);
static void  board_button_event_cb(void *arg, void *data);

esp_err_t board_init(void){
    esp_err_t ret = ESP_OK;
    ret += board_button_init();
    ret += board_led_init();
    ret += board_switch_init();
    return ret;
}

esp_err_t board_button_init(void){
    esp_err_t ret = ESP_OK;

    const button_gpio_config_t board_button_gpio_config[BTN_NUM] = {
        #ifdef BTN_1
        {
            .gpio_num       = BTN_1,
            .active_level   = BUTTON_ACTIVE_LEVEL,
        },
        #endif
        #ifdef BTN_2
        {
            .gpio_num       = BTN_2,
            .active_level   = BUTTON_ACTIVE_LEVEL,
        },
        #endif
        #ifdef BTN_3
        {
            .gpio_num       = BTN_3,
            .active_level   = BUTTON_ACTIVE_LEVEL,
        },
        #endif
    };
    
    button_config_t board_button_config[BTN_NUM] = {
        #ifdef BTN_1
        {
            .type               = BUTTON_TYPE_GPIO,
            .long_press_time    = BUTTON_LONG_PRESS_TIME,
            .short_press_time   = BUTTON_SHORT_PRESS_TIME,
            .gpio_button_config = board_button_gpio_config[0],
        },
        #endif
        #ifdef BTN_2
        {
            .type               = BUTTON_TYPE_GPIO,
            .long_press_time    = BUTTON_LONG_PRESS_TIME,
            .short_press_time   = BUTTON_SHORT_PRESS_TIME,
            .gpio_button_config = board_button_gpio_config[1],
        },
        #endif
        #ifdef BTN_3
        {
            .type               = BUTTON_TYPE_GPIO,
            .long_press_time    = BUTTON_LONG_PRESS_TIME,
            .short_press_time   = BUTTON_SHORT_PRESS_TIME,
            .gpio_button_config = board_button_gpio_config[2],
        },
        #endif
    };

    for (uint8_t i=0; i<BTN_NUM; i++){
        btn_state[i].handler = iot_button_create(&board_button_config[i]);
        if (btn_state[i].handler == NULL) ret = ESP_FAIL;
    }

    if (ret != ESP_OK) return ret;

    for (uint8_t i=0; i<BTN_NUM; i++){
        ret += iot_button_register_cb(btn_state[i].handler, BUTTON_SINGLE_CLICK     , board_button_event_cb, (void *)&(btn_state[i]));
        ret += iot_button_register_cb(btn_state[i].handler, BUTTON_LONG_PRESS_START , board_button_event_cb, (void *)&(btn_state[i]));
        ret += iot_button_register_cb(btn_state[i].handler, BUTTON_LONG_PRESS_UP    , board_button_event_cb, (void *)&(btn_state[i]));
    }

    return ret;
}

esp_err_t board_switch_init(void){
    esp_err_t ret = ESP_OK;

    gpio_config_t gpio_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    for (int i=0; i<SW_NUM; i++){
        gpio_conf.pin_bit_mask = (1ULL << sw_state[i].pin);
        ret += gpio_config(&gpio_conf);
    }

    return ret;
}

static void board_button_event_cb(void *arg, void *data){
    // iot_button_print_event((button_handle_t) arg);
    button_handle_t btn_handler = (button_handle_t) arg;
    button_state_t *btn         = (button_state_t *) data;

    button_event_t btn_event = iot_button_get_event(btn_handler);
    ESP_LOGW("BOARD", "Event %s on button %s", iot_button_get_event_str(btn_event), btn->name);

    switch (btn_event){
        case BUTTON_SINGLE_CLICK:{
            btn->previous = btn->current;
            btn->current  = btn_event;
            root_device_local_control(btn->element, CTR_ONOFF_FLIP);
            break;
        }
        case BUTTON_LONG_PRESS_START:{
            btn->previous = btn->current;
            btn->current  = btn_event;
            uint8_t btnHoldResetNum = 0;
            uint8_t btnHoldNum = 0;
            for (uint8_t i=0; i<BTN_NUM; i++){
                if (btn_state[i].previous == BUTTON_LONG_PRESS_UP){
                    btnHoldResetNum++;
                }
                if (btn_state[i].current == BUTTON_LONG_PRESS_START){
                    btnHoldNum++;
                }
            }
            if (btnHoldNum == BTN_NUM){
                board_led_indicator(DEVICE_ELEMENT_ALL, BLINK_DOUBLE);
                if (btnHoldResetNum == BTN_NUM){
                    ESP_LOGW("DEVICE", "HARD RESET");
                    root_device_factory_reset();
                }
            }
            break;
        }
        default:{
            btn->previous = btn->current;
            btn->current  = btn_event;
            break;
        }
    }
}

esp_err_t board_switch_control(uint8_t element, uint8_t onoff){
    esp_err_t ret = ESP_OK;

    uint8_t swIdx = 0;
    bool    swValid = false;
    for (swIdx = 0; swIdx < SW_NUM; swIdx++){
        if (element == sw_state[swIdx].element){
            swValid = true;
            break;
        }
    }

    if (!swValid) return ESP_ERR_NOT_FOUND;

    if (sw_state[swIdx].current == onoff){
        ESP_LOGI("BOARD", "SW %s already %s", sw_state[swIdx].name, (onoff == SWITCH_ACTIVE_LEVEL ? "on" : "off"));
        return ESP_OK;
    }

    sw_state[swIdx].current = onoff;
    sw_state[swIdx].set = false;
    ret = gpio_set_level(sw_state[swIdx].pin, onoff);
    sw_state[swIdx].set = true;

    return ret;
}