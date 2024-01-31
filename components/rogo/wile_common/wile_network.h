#pragma once
#ifndef _WILE_NETWORK_H_
#define _WILE_NETWORK_H_

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

//HTTP
#include <math.h>
#include "esp_netif.h"
#include "esp_netif_ip_addr.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "mdns.h"
#include "netdb.h"

// #include "esp_aes.h"
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"

#include "esp_tls.h"
#include "esp_http_client.h"

#define TAG "WILE"
#define MESH_TAG "MESH"
#define WIFI_TAG "WIFI"

void wifi_scan(void);
void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void http_get_ir(uint16_t devType, uint16_t devNwkAddr, uint16_t devManuFactory);
uint16_t http_get_ir_native(uint16_t devEid, uint8_t *devID, uint16_t devType, uint16_t devProto, uint16_t devNwkAddr, uint16_t devManuFactory);
void check_reset_task(void *pvParameter);
void ota_task(void *pvParameter);
void mDns_init_task(void *pvParameter);
esp_err_t initialise_mdns(void);
void generate_tcp_port();
void tcp_server_task(void *pvParameters);
esp_err_t tcp_send_task(uint8_t *msg, uint16_t length, char *host_ip, uint16_t port);

#ifdef __cplusplus
}
#endif

#endif