#include "driver/gpio.h"
#include "board.h"
#include "board_led.h"

led_state_t led_state[LED_NUM] = {
    #if LED_NUM > 0
    {
        .element   = 1,
        .current   = BOARD_LED_OFF_BRIGHTNESS,
        .previous  = BOARD_LED_OFF_BRIGHTNESS,
        #if LED_RGB_WS2812
        .index     = 0,
        #endif
        #if LED_INDICATOR_MULTI
        .indicator = NULL,
        #endif
        .name       = "LED_1",
    },
    #endif // LED_NUM = 1

    #if LED_NUM > 1
    {
        .element   = 2,
        .current   = BOARD_LED_OFF_BRIGHTNESS,
        .previous  = BOARD_LED_OFF_BRIGHTNESS,
        #if LED_RGB_WS2812
        .index     = 1,
        #endif
        #if LED_INDICATOR_MULTI
        .indicator = NULL,
        #endif
        .name       = "LED_2",
    },
    #endif // LED_NUM = 2

    #if LED_NUM > 2
    {
        .element   = 3,
        .current   = BOARD_LED_OFF_BRIGHTNESS,
        .previous  = BOARD_LED_OFF_BRIGHTNESS,
        #if LED_RGB_WS2812
        .index     = 2,
        #endif
        #if LED_INDICATOR_MULTI
        .indicator = NULL,
        #endif
        .name       = "LED_3",
    },
    #endif // LED_NUM = 3
};

#if !LED_INDICATOR_MULTI
static led_indicator_handle_t board_led_indicator_handle = NULL;
#endif

#if LED_RGB_WS2812
#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)
#endif

static const blink_step_t board_led_blink_fast[] = {
    {LED_BLINK_BRIGHTNESS, BOARD_LED_ON_BRIGHTNESS , 200},
    {LED_BLINK_BRIGHTNESS, BOARD_LED_OFF_BRIGHTNESS, 200},
    {LED_BLINK_LOOP, 0, 0},
};

static const blink_step_t board_led_blink_double[] = {
    {LED_BLINK_BRIGHTNESS, BOARD_LED_ON_BRIGHTNESS , 300},
    {LED_BLINK_BRIGHTNESS, BOARD_LED_OFF_BRIGHTNESS, 300},
    {LED_BLINK_BRIGHTNESS, BOARD_LED_ON_BRIGHTNESS , 300},
    {LED_BLINK_BRIGHTNESS, BOARD_LED_OFF_BRIGHTNESS, 300},
    {LED_BLINK_STOP, 0, 0},
};

static const blink_step_t board_led_blink_triple_fast[] = {
    {LED_BLINK_BRIGHTNESS, BOARD_LED_ON_BRIGHTNESS , 200},
    {LED_BLINK_BRIGHTNESS, BOARD_LED_OFF_BRIGHTNESS, 200},
    {LED_BLINK_BRIGHTNESS, BOARD_LED_ON_BRIGHTNESS , 200},
    {LED_BLINK_BRIGHTNESS, BOARD_LED_OFF_BRIGHTNESS, 200},
    {LED_BLINK_BRIGHTNESS, BOARD_LED_ON_BRIGHTNESS , 200},
    {LED_BLINK_BRIGHTNESS, BOARD_LED_OFF_BRIGHTNESS, 200},
    {LED_BLINK_STOP, 0, 0},
};

static const blink_step_t board_led_blink_breath[] = {
    /*!< Set Color to blue and brightness to zero by H:240 S:255 V:0 */
    {LED_BLINK_HSV, SET_HSV(240, MAX_SATURATION, 0), 0},
    {LED_BLINK_BREATHE, LED_STATE_ON, 1000},
    {LED_BLINK_BREATHE, LED_STATE_OFF, 1000},
    {LED_BLINK_LOOP, 0, 0},
};

blink_step_t const *board_led_mode[] = {
    [BLINK_DOUBLE]              = board_led_blink_double,
    [BLINK_TRIPLE_FAST]         = board_led_blink_triple_fast,
    [BLINK_FAST]                = board_led_blink_fast,
    [BLINK_BREATH]              = board_led_blink_breath,
    [BLINK_COLOR_HSV_RING]      = NULL, // color_hsv_ring_blink,
    [BLINK_COLOR_RGB_RING]      = NULL, // color_rgb_ring_blink,
    #if LED_NUM > 1
    [BLINK_FLOWING]             = NULL, // flowing_blink,
    #endif
    [BLINK_MAX]                 = NULL,
};

esp_err_t board_led_init(void);
esp_err_t board_led_indicator_init(void);
esp_err_t board_led_indicator_state(void);
esp_err_t board_led_indicator(uint8_t element, uint8_t mode);
esp_err_t board_led_indicator_brightness(uint8_t element, uint8_t brightness);
esp_err_t board_led_indicator_color(uint8_t element, uint16_t hue);

esp_err_t board_led_init(void){
    esp_err_t ret = ESP_OK;
    ret = board_led_indicator_init();

    if (ret != ESP_OK) return ret;

    board_led_indicator_brightness(DEVICE_ELEMENT_ALL, BOARD_LED_OFF_BRIGHTNESS);

    return ret;
}

esp_err_t board_led_indicator_state(void){
    esp_err_t ret = ESP_OK;

    #if !LED_INDICATOR_MULTI

    #if LED_RGB_WS2812
    for (uint8_t i=0; i<LED_NUM; i++){
        ret += led_indicator_set_brightness(board_led_indicator_handle, INSERT_INDEX(led_state[i].index, led_state[i].current));
    }
    #else
    ret = ESP_ERR_NOT_SUPPORTED;
    #endif

    #else // LED_INDICATOR_MULTI

    for (uint8_t i=0; i<LED_NUM; i++){
        ret = led_indicator_set_brightness(led_state[i].indicator, led_state[i].current);
    }

    #endif // LED_INDICATOR_MULTI

    return ret;
}

esp_err_t board_led_indicator_init(void){
    esp_err_t ret = ESP_OK;

    #if LED_INDICATOR_MULTI
    #if LED_GPIO
    led_indicator_ledc_config_t board_ledc_config[LED_NUM] = {
        #ifdef LED_1
        { LED_ACTIVE_LEVEL, false, LEDC_TIMER_0, LED_1, LEDC_CHANNEL_0 },
        #endif // LED_1
        #ifdef LED_2
        { LED_ACTIVE_LEVEL, false, LEDC_TIMER_0, LED_2, LEDC_CHANNEL_1 },
        #endif // LED_2
        #ifdef LED_3
        { LED_ACTIVE_LEVEL, false, LEDC_TIMER_0, LED_3, LEDC_CHANNEL_2 },
        #endif // LED_3
    };
    const led_indicator_config_t board_led_indicator_config[LED_NUM] = {
        #ifdef LED_1
        {
            .mode                         = LED_LEDC_MODE,
            .led_indicator_ledc_config    = &board_ledc_config[0],
            .blink_lists                  = board_led_mode, 
            .blink_list_num               = BLINK_MAX
        },
        #endif
        #ifdef LED_2
        {
            .mode                         = LED_LEDC_MODE,
            .led_indicator_ledc_config    = &board_ledc_config[1],
            .blink_lists                  = board_led_mode, 
            .blink_list_num               = BLINK_MAX
        },
        #endif
        #ifdef LED_3
        {
            .mode                         = LED_LEDC_MODE,
            .led_indicator_ledc_config    = &board_ledc_config[2],
            .blink_lists                  = board_led_mode, 
            .blink_list_num               = BLINK_MAX
        },
        #endif
    };
    #endif

    for (uint8_t i=0; i<LED_NUM; i++){
        led_state[i].indicator = led_indicator_create(&board_led_indicator_config[i]);
        if (led_state[i].indicator == NULL) ret = ESP_FAIL;
    }

    #else // !LED_INDICATOR_MULTI

    #if LED_RGB_WS2812
    led_strip_config_t board_led_strip_config = {
        .strip_gpio_num = LED_1,                        // The GPIO that connected to the LED strip's data line
        .max_leds = LED_NUM,                            // The number of LEDs in the strip,
        .led_model = LED_MODEL_WS2812,                  // LED strip model
        .flags.invert_out = false,                      // whether to invert the output signal
    };
    // LED strip backend configuration: RMT
    led_strip_rmt_config_t board_led_rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
        .resolution_hz = LED_STRIP_RMT_RES_HZ, // RMT counter clock frequency
        .flags.with_dma = false,               // DMA feature is available on ESP target like ESP32-S3
    };

    led_indicator_strips_config_t board_led_indicator_strips_config = {
        .led_strip_cfg = board_led_strip_config,
        .led_strip_driver = LED_STRIP_RMT,
        .led_strip_rmt_cfg = board_led_rmt_config,
    };

    // LED_RGB_WS2812
    #elif LED_GPIO
    led_indicator_ledc_config_t board_ledc_config = {
        .is_active_level_high = LED_ACTIVE_LEVEL,
        .timer_inited = false,
        .timer_num = LEDC_TIMER_0,
        .gpio_num = LED_1,
        .channel = LEDC_CHANNEL_0,
    };
    // LED_GPIO
    #endif

    const led_indicator_config_t board_led_indicator_config = {
        #if LED_RGB_WS2812
        .mode = LED_STRIPS_MODE,
        .led_indicator_strips_config = &board_led_indicator_strips_config,
        #elif LED_GPIO
        .mode = LED_LEDC_MODE,
        .led_indicator_ledc_config = &board_ledc_config,
        #endif
        .blink_lists = board_led_mode,
        .blink_list_num = BLINK_MAX,
    };

    board_led_indicator_handle = led_indicator_create(&board_led_indicator_config);

    if (board_led_indicator_handle == NULL){
        ESP_LOGE("BOARD", "led_indicator_create fail!");
        ret = ESP_FAIL;
    }

    #endif // LED_INDICATOR_MULTI

    return ret;
}

esp_err_t board_led_indicator(uint8_t element, uint8_t mode){
    esp_err_t ret = ESP_OK;

    #if !LED_INDICATOR_MULTI

    if (element == DEVICE_ELEMENT_ALL){
        ret = led_indicator_start(board_led_indicator_handle, mode);
    }
    else{
        ret = ESP_ERR_INVALID_ARG;
    }

    #else // LED_INDICATOR_MULTI

    if (element == DEVICE_ELEMENT_ALL){
        for (uint8_t i=0; i<LED_NUM; i++){
            ret += led_indicator_start(led_state[i].indicator, mode);
        }
    }
    else{
        for (uint8_t i=0; i<LED_NUM; i++){
            if (led_state[i].element == element){
                ret = led_indicator_start(led_state[i].indicator, mode);
                break;
            }
        }
    }

    #endif // LED_INDICATOR_MULTI

    return ret;
}

esp_err_t board_led_indicator_stop(uint8_t element, uint8_t mode){
    esp_err_t ret = ESP_OK;

    #if !LED_INDICATOR_MULTI

    if (element == DEVICE_ELEMENT_ALL){
        ret = led_indicator_stop(board_led_indicator_handle, mode);
    }
    else{
        ret = ESP_ERR_INVALID_ARG;
    }

    #else // LED_INDICATOR_MULTI

    if (element == DEVICE_ELEMENT_ALL){
        for (uint8_t i=0; i<LED_NUM; i++){
            ret += led_indicator_stop(led_state[i].indicator, mode);
        }
    }
    else{
        for (uint8_t i=0; i<LED_NUM; i++){
            if (led_state[i].element == element){
                ret = led_indicator_stop(led_state[i].indicator, mode);
                break;
            }
        }
    }

    #endif // LED_INDICATOR_MULTI

    return ret;
}

esp_err_t board_led_indicator_brightness(uint8_t element, uint8_t brightness){
    esp_err_t ret = ESP_FAIL;

    #if !LED_INDICATOR_MULTI

    if (element == DEVICE_ELEMENT_ALL){
        for (uint8_t i=0; i<LED_NUM; i++){
            led_state[i].previous = led_state[i].current;
            led_state[i].current  = brightness;
        }
        ret = led_indicator_set_brightness(board_led_indicator_handle, INSERT_INDEX(MAX_INDEX, brightness));
    }
    else{
        #if LED_RGB_WS2812
        for (uint8_t i=0; i<LED_NUM; i++){
            if (led_state[i].element == element){
                led_state[i].previous = led_state[i].current;
                led_state[i].current  = brightness;
                ret = led_indicator_set_brightness(board_led_indicator_handle, INSERT_INDEX(led_state[i].index, brightness));
                break;
            }
        }
        #else
        ret = ESP_ERR_NOT_SUPPORTED;
        #endif
    }

    #else // LED_INDICATOR_MULTI

    if (element == DEVICE_ELEMENT_ALL){
        for (uint8_t i=0; i<LED_NUM; i++){
            led_state[i].previous = led_state[i].current;
            led_state[i].current  = brightness;
            ret += led_indicator_set_brightness(led_state[i].indicator, brightness);
        }
    }
    else{
        for (uint8_t i=0; i<LED_NUM; i++){
            if (led_state[i].element == element){
                led_state[i].previous = led_state[i].current;
                led_state[i].current  = brightness;
                ret = led_indicator_set_brightness(led_state[i].indicator, brightness);
                break;
            }
        }
    }

    #endif // LED_INDICATOR_MULTI

    return ret;
}

esp_err_t board_led_indicator_color(uint8_t element, uint16_t hue){
    esp_err_t ret = ESP_OK;

    #if !LED_INDICATOR_MULTI
    if (element == DEVICE_ELEMENT_ALL){
        ret = led_indicator_set_hsv(board_led_indicator_handle,
                                    SET_IHSV(MAX_INDEX, hue, MAX_SATURATION, GET_BRIGHTNESS(led_state[0].current)));
    }
    else{
        #if LED_RGB_WS2812
        for (uint8_t i=0; i<LED_NUM; i++){
            if (led_state[i].element == element){
                ret = led_indicator_set_hsv(board_led_indicator_handle,
                                            SET_IHSV(led_state[i].index, hue, MAX_SATURATION, GET_BRIGHTNESS(led_state[i].current)));
                break;
            }
        }
        #else
        ret = ESP_ERR_NOT_SUPPORTED;
        #endif
    }

    #else // LED_INDICATOR_MULTI

    #if LED_RGB_WS2812
    if (element == DEVICE_ELEMENT_ALL){
        for (uint8_t i=0; i<LED_NUM; i++){
            ret += led_indicator_set_hsv(led_state[i].indicator,
                                         SET_IHSV(MAX_INDEX, hue, MAX_SATURATION, GET_BRIGHTNESS(led_state[0].current)));
        }
    }
    else{
        for (uint8_t i=0; i<LED_NUM; i++){
            if (led_state[i].element == element){
                ret = led_indicator_set_hsv(led_state[i].indicator,
                                            SET_IHSV(MAX_INDEX, hue, MAX_SATURATION, GET_BRIGHTNESS(led_state[0].current)));
                break;
            }
        }
    }

    #else
    ret = ESP_ERR_NOT_SUPPORTED;
    #endif // LED_RGB_WS2812

    #endif // LED_INDICATOR_MULTI

    return ret;
}