#pragma once
#ifndef _WILE_DEVICE_H_
#define _WILE_DEVICE_H_

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

#include "board.h"
#include "wile_define.h"
#include "wile_init.h"
#include "wile_network.h"
#include "wile_message.h"
// #include "wile_mesh.h"
// #include "wile_gatt.h"

esp_err_t rgmgt_device_remove(uint16_t eidTemp);
esp_err_t rgmgt_device_add(uint8_t *eidInfo, uint8_t *elmInfo, uint8_t *meshDevKey);
uint8_t   rgmgt_device_elm_num(uint8_t *elmInfo, size_t elmInfoLen);
uint16_t  rgmgt_device_get_elmid(uint8_t index, uint8_t *elmInfo, size_t elmInfoLen);
uint16_t  rgmgt_device_get_elmid_idx(uint16_t eid, uint8_t index);
uint8_t   rgmgt_device_get_elmidx(uint16_t elementID, uint8_t *elmInfo, size_t elmInfoLen);
void      rgmgt_device_control_task(void *pvParameters);
esp_err_t rgmgt_device_control(uint8_t typeDev, uint16_t eidControl, uint16_t element, uint16_t type, uint8_t *value, uint8_t reportToMqtt, uint16_t delay, uint16_t reverse);
esp_err_t rgmgt_device_set_state(uint16_t eidTemp, uint16_t deviceElmId, uint16_t type, uint8_t *value, uint8_t reportToMqtt);
esp_err_t rgmgt_device_check_v0(void);

// esp_err_t rgmgt_group(uint8_t CMD_EXT, uint16_t deviceEid, uint16_t deviceElement, uint16_t newGroup, uint16_t oldGroup);
esp_err_t rgmgt_group(uint16_t deviceEid, uint8_t *deviceElms, uint16_t elmSize, uint16_t newGroup, uint16_t oldGroup);
esp_err_t rgmgt_group_control(uint16_t groupControl, uint8_t typeDev, uint16_t type, uint8_t *value);

esp_err_t rgmgt_smart(uint8_t CMD_EXT, uint16_t smartBlock, uint8_t *cmd, uint16_t cmdLen);
esp_err_t rgmgt_smart_control(uint8_t trigger, uint16_t smartID, uint8_t *attrTrigger, uint8_t *timeCfg, uint8_t timeCfgLen);
esp_err_t rgmgt_smart_trigger(uint8_t *attrTrigger, uint8_t attrTriggerLen, rgsmt_trig_para_t *smartPara, rgsmt_trig_smart_t* smartData);
esp_err_t rgmgt_smart_trigger_handle(rgmsg_t *rogoCmdMsg);
esp_err_t rgmgt_smart_trigger_storage(uint16_t smartTrigEid, uint16_t smartTrigId,
                                      rgsmt_trig_para_t *smartTrigParaSrc, rgsmt_trig_smart_t *smartTrigDataSrc,
                                      uint8_t isRemove);

#ifdef __cplusplus
}
#endif

#endif

