// Copyright Rogo Solutions, Inc.

#ifdef __cplusplus
extern "C"
{
#endif

#include "board.h"
#include "wile_config.h"
#include "wile_init.h"
#include "wile_message.h"
#include "app_device.h"
#include "sdkconfig.h"

void app_main(void)
{
    esp_err_t err;
    err = board_init();
    if (err) {
        ESP_LOGE("DEVICE", "board_init failed (err %d)", err);
        return;
    }

    #if CONFIG_WILE_ENABLE
    ESP_LOGI("WILE", "Initializing...");
    err = rgmgt_wile_init();
    if (err){
        ESP_LOGE("WILE", "INIT FAIL: err: 0x%04X", err);
        return;
    }
    if (PROV_STATE == STEP_CFG_NONE){
        root_device_prov_none();
    }
    else if (PROV_STATE == STEP_CFG_COMPLETE){
        root_device_prov_complete();
    }
    #else // !WILE_ENABLE
    root_device_prov_none();
    #endif
    root_device_state_init();
}

#ifdef __cplusplus
}
#endif