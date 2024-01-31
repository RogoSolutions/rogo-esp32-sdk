#pragma once
#ifndef _WILE_MESH_H_
#define _WILE_MESH_H_

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
#include "esp_ble_mesh_lighting_model_api.h"
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

#include "mesh_bearer_adapt.h"
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

#define TAG "WILE"
#define MESH_TAG "MESH"
#define WIFI_TAG "WIFI"
#define GATTS_TABLE_TAG "GATTS_TABLE"

#define CID_ESP                  0x02E5
#define CID_TELINK               0x0211

#define MESH_IV_UPDATE_SEQ_LIMIT 8000000

extern struct _led_state led_state[3];

// struct bt_mesh_device_network_info mesh_info;

void mesh_prov_task(void * pvParameters);

void ble_mesh_get_dev_uuid(uint8_t *dev_uuid);

esp_err_t ble_mesh_init(void);
int bt_mesh_device_auto_enter_network(struct bt_mesh_device_network_info *info);

int bt_mesh_device_add_app_key(uint16_t net_index, uint16_t app_index, uint8_t *app_key);

int bt_mesh_device_add_group(uint16_t group);

void rogo_mesh_send_onoff(uint16_t nodeAddr, uint16_t nodeState, uint16_t appIdx);
void rogo_mesh_send_ctl(uint16_t nodeAddr, uint16_t lightness, uint16_t temperatrue, uint16_t appIdx);
void rogo_mesh_send_hsv(uint16_t nodeAddr, uint16_t hue, uint16_t saturation, uint16_t value, uint16_t appIdx);
esp_err_t rogo_mesh_delete_node(uint16_t nodeAddr, uint8_t *devKey);
esp_err_t rogo_mesh_add_group(uint16_t nodeAddr, uint8_t *devKey, uint16_t groupAddr, uint16_t elmAddr);

#ifdef __cplusplus
}
#endif

#endif