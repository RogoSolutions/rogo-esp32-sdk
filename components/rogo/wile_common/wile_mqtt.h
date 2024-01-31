#pragma once
#ifndef _WILE_MQTT_H_
#define _WILE_MQTT_H_

//IR
// #include "Arduino.h"
// #include "IRremoteESP8266.h"
// #include "IRsend.h"
#include "ir_common.h"
#include "app_device.h"
// IRsend irsend(3);

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <math.h>
#include "esp_netif.h"
#include <lwip/err.h>
#include <lwip/sys.h>

// #include "esp_aes.h"
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"

#include "esp_tls.h"

#include "mqtt_client.h"

#include <cJSON.h>

#include "board.h"
#include "wile_define.h"
#include "wile_init.h"
#include "wile_network.h"
#include "wile_device.h"
// #include "wile_mesh.h"
// #include "wile_gatt.h"

void mqtt_init_task(void *pvParameter);
void mqtt_app_start();
void rogo_cmd_handle(uint8_t *cmd, int cmd_len, char *ipAddr);
int8_t mqtt_send_to_user(char *msg, uint16_t length, uint8_t toCloud);
int8_t mqtt_send_to_location(char *msg, uint16_t length);
void mqtt_send_notify_status(uint8_t status, uint16_t errCode, uint8_t *cmd, char *appID, char *ipAddr, uint16_t port);
int8_t wile_send_to_app(char *appID, uint8_t appIDlen, char *msg, uint16_t length, char *ipAddr, uint16_t port);
void device_control_task(void *pvParameters);
void smart_control_handler(uint8_t trigger, uint16_t smartID, uint8_t *timeCfg, uint8_t timeCfgLen);
void update_device_status(void *pvParameters);
uint8_t set_device_state(uint16_t eidTemp, uint16_t type, uint8_t *value, uint8_t reportToMqtt);
esp_err_t push_notification_device_state(uint16_t eidTemp, uint16_t type, uint8_t *value);
uint8_t get_device_state(uint16_t eidTemp, char *senderID, uint8_t appIdLen, const char *ipAddr, uint16_t port);
uint8_t get_local_device_state(uint16_t eidTemp, void **deviceState, uint8_t *stateSize);
void device_control_handler(uint8_t cmdType, uint8_t *cmd, int cmdLen, char *ipAddr, uint16_t port);
void mesh_state_control(uint16_t eidTemp, uint16_t nodeAddr, uint16_t nodeType, uint16_t type, uint8_t *value, uint8_t reportToMqtt);
uint8_t ac_state_to_data(uint16_t *ac_value, char *eid, uint8_t tempAllow, uint8_t fanAllow);

#ifdef __cplusplus
}
#endif

#endif