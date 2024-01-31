#pragma once
#ifndef _WILE_GATT_H_
#define _WILE_GATT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#include "board.h"
// #include "ble_mesh_fast_prov_operation.h"
// #include "ble_mesh_fast_prov_client_model.h"
// #include "ble_mesh_fast_prov_server_model.h"
// #include "ble_mesh_example_init.h"

#include "esp_system.h"
#include "esp_bt.h"

#ifdef CONFIG_BT_BLUEDROID_ENABLED
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#endif

#ifdef CONFIG_BT_NIMBLE_ENABLED
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#endif

// #include "mesh_bearer_adapt.h"
#include <errno.h>
#include "adv.h"
#include "scan.h"
#include "mesh.h"
#include "crypto.h"
#include "access.h"
#include "foundation.h"
#include "mesh_main.h"

//MQTT
#include "mqtt_client.h"
#include "esp_tls.h"

// APP
#include "ir_common.h"
// #include "wile_mesh.h"

#define TAG "WILE"
#define MESH_TAG "MESH"
#define WIFI_TAG "WIFI"
#define GATTS_TABLE_TAG "GATTS_TABLE"

/* Attributes State Machine */
enum
{
    IDX_SVC,

    IDX_CHAR_DATAIN,
    IDX_CHAR_VAL_DATAIN,

    IDX_CHAR_DATAOUT,
    IDX_CHAR_VAL_DATAOUT,
    IDX_CHAR_CFG_DATAOUT,

    HRS_IDX_NB,
};

// #define LED_ON  1
// #define LED_OFF 0

#define PROFILE_NUM                 1
#define PROFILE_APP_IDX             0
#define ESP_APP_ID                  0x00
#define SVC_INST_ID                 0

/* The max length of characteristic value. When the GATT client performs a write or prepare write operation,
*  the data length must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
*/
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500
#define PREPARE_BUF_MAX_SIZE        1024
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))

#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)

struct bt_mesh_device_network_info {
    uint8_t  net_key[16];
    uint16_t net_idx;
    uint8_t  flags;
    uint32_t iv_index;
    uint16_t unicast_addr;
    uint8_t  dev_key[16];
    struct bt_mesh_app_keys app_keys[CONFIG_BLE_MESH_APP_KEY_COUNT];
    uint16_t app_idx;
    uint16_t group_addr[CONFIG_BLE_MESH_MODEL_GROUP_COUNT];
};

#ifdef CONFIG_BT_BLUEDROID_ENABLED
void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

void gatts_profile_event_handler(esp_gatts_cb_event_t event,
					esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
#endif // CONFIG_BT_BLUEDROID_ENABLED

#ifdef CONFIG_BT_NIMBLE_ENABLED
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_svr_init(void);
int wile_gatt_svr_init(void);
void nimble_advertise(void);
#endif

void wile_gatt_send_indicate(uint16_t value_len, uint8_t *value, bool need_confirm);

#ifdef __cplusplus
}
#endif

#endif