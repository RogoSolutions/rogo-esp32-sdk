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
#include "sdkconfig.h"
#include "inttypes.h"

#define TAG "BOARD"
#define CTR_ONOFF_ANY 2

bool localDevState = 0;
struct _led_state led_state[3] = {
    {LED_OFF, LED_OFF, LED_R, "red"},
    {LED_OFF, LED_OFF, LED_G, "green"},
    {LED_OFF, LED_OFF, LED_B, "blue"},
};

void board_output_number(esp_ble_mesh_output_action_t action, uint32_t number)
{
    ESP_LOGI(TAG, "Board output number %ld", number);
}

void board_prov_complete(void)
{
    board_led_operation(LED_B, LED_OFF);
}

void board_led_operation(uint8_t pin, uint8_t onoff)
{
    for (int i = 0; i < 3; i++)
    {
        if (led_state[i].pin != pin)
        {
            continue;
        }
        if (onoff == led_state[i].previous)
        {
            ESP_LOGI(TAG, "led %s is already %s",
                     led_state[i].name, (onoff ? "on" : "off"));
            return;
        }
        gpio_set_level(pin, onoff);
        led_state[i].previous = onoff;
        return;
    }

    ESP_LOGE(TAG, "LED is not found!");
}

void board_led_init(void)
{
#if(CONFIG_ESP32C3_PLUG_RD_FPT == 1)
    gpio_reset_pin(RGB_LED_CLK);
    gpio_set_direction(RGB_LED_CLK,GPIO_MODE_OUTPUT);
    gpio_set_level(RGB_LED_CLK, LED_OFF);

    gpio_reset_pin(RGB_LED_DAT);
    gpio_set_direction(RGB_LED_DAT,GPIO_MODE_OUTPUT);
    gpio_set_level(RGB_LED_DAT, LED_OFF);
    
    // gpio_reset_pin(ZRCRSS_PIN);
    // gpio_set_direction(ZRCRSS_PIN,GPIO_MODE_INPUT);
#else

    for (int i = 0; i < 3; i++)
    {
        gpio_reset_pin(led_state[i].pin);
        gpio_set_direction(led_state[i].pin, GPIO_MODE_OUTPUT);
        gpio_set_level(led_state[i].pin, LED_OFF);
        led_state[i].previous = LED_OFF;
    }
#endif
}

void board_control_init(void)
{
    gpio_reset_pin(RELAY_SW);
    gpio_set_direction(RELAY_SW, GPIO_MODE_OUTPUT);
    gpio_set_level(RELAY_SW, SW_OFF);
    uint8_t *devState = NULL;
    uint8_t devStateSize = 0;

    if (root_device_get_state(&devState, &devStateSize) == ESP_OK)
    {
        ESP_LOG_BUFFER_HEX("ROOT DEVICE STATE", devState, devStateSize);
        root_device_control(0x01, FEATURE_ONOFF, devState);
        localDevState = devState[1];
    }
    if (devState == NULL)
        ESP_LOGE("DEVICE", "State NULL");
    if (devState != NULL)
        free(devState);
}

esp_err_t board_init(void)
{
    board_led_init();
    board_sw_init();
    board_control_init();
    return ESP_OK;
}

esp_err_t root_device_local_control(uint8_t mode){
    esp_err_t ret = ESP_OK;
    uint8_t *devState = NULL;
    uint8_t devStateSize = 0;
    if (root_device_get_state(&devState, &devStateSize) == ESP_OK){
        ESP_LOG_BUFFER_HEX("ROOT DEVICE STATE", devState, devStateSize);
        if (mode == CTR_ONOFF_ANY){
            devState[1] = !devState[1];
        }
        else{
            devState[1] = mode;
        }
        root_device_set_state(FEATURE_ONOFF, devState);
        // root_device_control(FEATURE_ONOFF, devState);
    }
    else 
    {
        devState = calloc(2 ,sizeof(uint8_t));
        if (mode == CTR_ONOFF_ANY){
            devState[1] = !localDevState;
        }
        else{
            devState[1] = mode;
        }
        // devState[1] = !localDevState;
    }
    if (devState == NULL){
        ESP_LOGE("DEVICE", "State NULL");
        return ESP_FAIL;
    }

    ret = root_device_control(0x01, FEATURE_ONOFF, devState);
    if (devState != NULL) free(devState);
    return ret;
}

TaskHandle_t btnTask = NULL;
static void button_task(void *arg)
{
    ESP_LOGW("WILE", "Reset button");
    vTaskDelay(100);
    if (gpio_get_level(RESET_SW) != 0) 
    {
        btnTask = NULL;
        vTaskDelete(NULL);

    }    // vTaskDelay(30);

    root_device_local_control(CTR_ONOFF_ANY);

    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (!gpio_get_level(RESET_SW))
    {
        if(xTaskGetTickCount()-xLastWakeTime >=8000)
        {
            root_device_factory_reset();
            esp_restart();
            break;
        }
        vTaskDelay(200);
    }
    // printf("tick count %d ",xTaskGetTickCount()-xLastWakeTime);
    btnTask = NULL;
    vTaskDelete(NULL);
    // vTaskDelete(btnTask);

}

static void IRAM_ATTR button_gpio_isr_handler(void *arg)
{
    gpio_num_t btn = (gpio_num_t)arg;
    if (btn == RESET_SW && gpio_get_level(btn) == 0 && btnTask == NULL)
    {
        xTaskCreate(button_task, "button_task", 1024 * 3, NULL, 10, &btnTask);
    }
    
}
#ifdef CONFIG_ESP32C3_PLUG_RD_FPT
esp_timer_handle_t timer_handler;
uint32_t cnt_time=0;
bool relay_set_status = 0;

void timer_callback(void *param)
{
    gpio_set_level(RELAY_SW, localDevState);
    esp_timer_delete(timer_handler);
    ESP_LOGI("Zero Crossing"," Relay latched ");
    relay_set_status = 1;
}
static void IRAM_ATTR zero_crossing_freq_count(void *arg)
{
    static uint8_t cnt = 0;
    static uint32_t ac_period_us = 0;// = esp_timer_get_time();

    cnt++;

    // else if ()
    if(cnt == 0xff){
        esp_timer_start_once(timer_handler,localDevState ? ac_period_us - ZRCRSS_ON : ac_period_us - ZRCRSS_OFF);
        gpio_isr_handler_remove(ZRCRSS_PIN);
        cnt_time = ac_period_us;
        cnt = 0;


    }
    else if (cnt == 2)
    {   
        ac_period_us =(uint32_t) esp_timer_get_time() - ac_period_us;
        if(ac_period_us > 5000 && ac_period_us < 15000 )
        {
            cnt = 0xfe;

            // ESP_ERROR_CHECK(esp_timer_start_once(timer_handler, ac_period_us - 3900 ));
            // gpio_isr_handler_remove(ZRCRSS_PIN);
            // cnt_time = ac_period_us;
        }
        else{
        cnt = 0;
        }
    }
    else 
    {
        // cnt_time[0] =esp_timer_get_time();
        ac_period_us = esp_timer_get_time();
    }
        // cnt++;

}
#endif
void board_sw_init(void)
{
    gpio_config_t gpio_conf;
    gpio_conf.intr_type = GPIO_INTR_NEGEDGE;
    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.pin_bit_mask = (1ULL << RESET_SW);
    gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&gpio_conf);
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(RESET_SW, button_gpio_isr_handler, (void *)RESET_SW);

    #ifdef CONFIG_ESP32C3_PLUG_RD_FPT
    gpio_conf.intr_type = GPIO_INTR_NEGEDGE;
    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.pin_bit_mask = (1ULL << ZRCRSS_PIN);
    gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&gpio_conf);
    #endif
    // gpio_install_isr_service(0);
    // gpio_isr_handler_add(ZRCRSS_PIN, zero_crossing_freq_count, (void *)ZRCRSS_PIN);
}

void board_sw_control(uint8_t pin, uint8_t onoff)
{   
    //  localDevState = onoff;
    // if(onoff)
    // {
        #ifdef CONFIG_ESP32C3_PLUG_RD_FPT
        esp_timer_create_args_t my_timer_args = {.callback = &timer_callback,.name = "My Timer"};
        esp_timer_create(&my_timer_args, &timer_handler);
        relay_set_status = 0;    
        gpio_isr_handler_add(ZRCRSS_PIN, zero_crossing_freq_count, (void *)ZRCRSS_PIN);
        vTaskDelay(100);
        ESP_LOGI(TAG,"tick count timer %ld \n",cnt_time);
        
        if(!relay_set_status)
        {
            ESP_LOGE(TAG,"Zero crossing error, shooting blind");
            gpio_set_level(pin, onoff);
            gpio_isr_handler_remove(ZRCRSS_PIN);
        }
        #else 
            gpio_set_level(pin, onoff);
        #endif
    // }
    // else 
    // {
    //     gpio_set_level(pin, onoff);
    // }

}

/// RGE led operation
#ifdef CONFIG_ESP32C3_PLUG_RD_FPT
void two_wrie_led_bit_bag(uint8_t data)
{
    for (int8_t bit = 7; bit >= 0; bit--)
    {
        // set data bit on pin
        gpio_set_level(RGB_LED_DAT, (data & 0x80) == 0x80 ? HIGH : LOW);
        // set clock high as data is ready
        gpio_set_level(RGB_LED_CLK, HIGH);
        data <<= 1;
        // set clock low as data pin is changed
        gpio_set_level(RGB_LED_CLK, LOW);
    }
}

void board_rgb_led_operation(uint8_t color, uint8_t brightness)
{
    if(brightness == LED_BRIGHTNESS_UNKNOWN)
    {
        brightness = localDevState ? LED_BRIGHTNESS_ON : LED_BRIGHTNESS_OFF;
        if(isReset)
        {
            return;
        }
    }
    uint8_t red_val = 0;
    uint8_t blue_val = 0;
    uint8_t green_val = 0;
    switch (color)
    {
    case ORANGE:
        red_val = RED_MAX_VAL * brightness / 100;
        green_val = 165 * brightness / 100;
        break;
    case BLUE:
        blue_val = BLUE_MAX_VAL * brightness / 100;
        break;
    case RED:
        red_val = RED_MAX_VAL * brightness / 100;
        break;
    case GREEN:
        green_val = GREEN_MAX_VAL * brightness / 100;
        break;
    case WHITE:
        blue_val  = BLUE_MAX_VAL * brightness / 100;
        red_val   = RED_MAX_VAL * brightness / 100;
        green_val = GREEN_MAX_VAL * brightness / 100;
        break;
    default:
        break;
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        two_wrie_led_bit_bag(0x00);
    }
    two_wrie_led_bit_bag(0xFF);

    two_wrie_led_bit_bag( blue_val);
    two_wrie_led_bit_bag(green_val);
    two_wrie_led_bit_bag(red_val);

    for (uint8_t i = 0; i < 4; i++)
    {
        two_wrie_led_bit_bag(0xFF);
    }
}
#endif