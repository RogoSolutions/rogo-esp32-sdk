// Copyright Rogo Solutions, Inc.

#if CONFIG_VFS_SUPPORT_TERMIOS
// remove defines added by arduino-esp32 core/esp32/binary.h which are
// duplicated in sys/termios.h which may be included by esp_vfs.h
#undef  B011
#undef  B110
#undef  B1000000
#endif

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
    ESP_LOGI("SOCKET", "Initializing...");
    
    err = rgmgt_wile_init();
    if (err){
        ESP_LOGE("WILE", "INIT FAIL: err: 0x%04X", err);
        return;
    }

    err = board_init();
    if (err) {
        ESP_LOGE("SOCKET", "board_init failed (err %d)", err);
        return;
    }

    if (PROV_STATE == STEP_CFG_NONE){
        root_device_prov_none();
    }

}

#ifdef __cplusplus
}
#endif