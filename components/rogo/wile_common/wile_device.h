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
esp_err_t rgmgt_device_join_handle(rgmsg_t *rogoCmdMsg, uint16_t *blockStart, uint8_t step);
esp_err_t rgmgt_device_add(uint16_t eidAdd, uint8_t *eidInfo, uint8_t *elmInfo, uint8_t *meshDevKey);
esp_err_t rgmgt_device_add_log(uint16_t eidAdd, uint8_t *elmInfo);
esp_err_t rgmgt_device_nwkaddr_to_eid(uint16_t nwkAddr, uint16_t *eid, uint16_t *elm);
uint8_t   rgmgt_device_feature_state_size(uint16_t featureType);
bool      rgmgt_device_feature_take_onoff(uint16_t featureType);
bool      rgmgt_device_take_sensor_log(uint16_t deviceType);
uint16_t  rgmgt_device_feature_log(uint16_t deviceType);
bool      rgmgt_device_feature_log_value(uint16_t feature, uint8_t *value);
uint8_t   rgmgt_device_elm_num(uint8_t *elmInfo, size_t elmInfoLen);
uint8_t   rgmgt_device_get_elmnum(uint8_t *deviceState, size_t stateLen);
uint16_t  rgmgt_device_get_elmoffset(uint8_t elmIdx, uint8_t *deviceState, size_t stateLen);
uint16_t  rgmgt_device_get_elmid(uint8_t elmIdx, uint8_t *deviceState, size_t stateLen);
uint16_t  rgmgt_device_get_elmid_nwk(uint16_t nwkAddr, uint8_t *deviceState, size_t stateLen);
uint16_t  rgmgt_device_get_elmid_idx(uint16_t eid, uint8_t index);
uint8_t   rgmgt_device_get_elmidx(uint16_t elmID, uint8_t *deviceState, size_t stateLen);
uint16_t  rgmgt_device_get_elmtype(uint16_t elmID, uint8_t *deviceState, size_t stateLen);
esp_err_t rgmgt_device_get_state_elm_attr(uint16_t elmIdx, uint16_t attrType, uint8_t *deviceState, size_t stateLen, void **stateOut, size_t *stateOutLen);
esp_err_t rgmgt_device_get_id_mac(uint16_t deviceEid, void **deviceInfo, size_t *deviceInfoLen);
esp_err_t rgmgt_device_get_protocol(uint16_t deviceEid, uint16_t *deviceProtocol);
esp_err_t rgmgt_device_id_to_eid(uint8_t *deviceId, uint16_t *deviceEid);
esp_err_t rgmgt_device_control_handle(rgmsg_t *rogoCmdMsg, uint16_t *blockStart);
void      rgmgt_device_control_task(void *pvParameters);
esp_err_t rgmgt_device_control(uint8_t typeDev, uint16_t eidControl, uint16_t element, uint16_t type, uint8_t *value,
                               uint8_t reportToMqtt, uint8_t checkTrigger, uint16_t smartSrc, uint16_t delay, uint16_t reverse);
esp_err_t rgmgt_device_mesh_control(uint16_t eid, uint16_t elm, uint16_t attr, uint8_t *value);
esp_err_t rgmgt_device_set_state(uint16_t eidTemp, uint16_t deviceElmId, uint16_t type, uint8_t *value, uint8_t toCloud, uint8_t checkTrigger);
esp_err_t rgmgt_device_update_state(uint16_t deviceElmId, uint8_t **deviceState, size_t *stateSize, uint16_t type, uint8_t *value,
                                    uint8_t *isOnOffChange, uint8_t *onOffValue);
esp_err_t rgmgt_device_get_state(uint16_t deviceEid, void **deviceState, size_t *stateSize);
esp_err_t rgmgt_device_set_log(uint16_t deviceEid, uint16_t deviceElmId, uint16_t type, uint8_t *value);
esp_err_t rgmgt_device_get_log(uint16_t deviceEid, uint16_t deviceElm, uint16_t attr, uint16_t year, uint16_t day, uint8_t **deviceLog, size_t *logSize);
esp_err_t rgmgt_device_report_state(uint16_t deviceEid);
esp_err_t rgmgt_device_check_state(void);
esp_err_t rgmgt_device_check_trigger(uint16_t eidTrig, uint16_t deviceElmId, uint8_t isOnOff, uint8_t onOffValue, uint16_t type, uint8_t *value);
esp_err_t rgmgt_device_event_cb(uint16_t event);
void      rgmgt_device_event_handle(esp_event_base_t event_base, int32_t event_id, void* event_data);

bool      rgmgt_device_log_exist(uint16_t eid);

// esp_err_t rgmgt_group(uint8_t CMD_EXT, uint16_t deviceEid, uint16_t deviceElement, uint16_t newGroup, uint16_t oldGroup);
esp_err_t rgmgt_group(uint16_t deviceEid, uint8_t *deviceElms, uint16_t elmSize, uint16_t newGroup, uint16_t oldGroup);
esp_err_t rgmgt_group_control(uint16_t groupControl, uint8_t typeDev, uint16_t type, uint8_t *value);

esp_err_t rgmgt_smart(uint8_t CMD_EXT, uint16_t smartBlock, uint8_t *cmd, uint16_t cmdLen);
bool      rgmgt_smart_exist(uint16_t smartID);
esp_err_t rgmgt_smart_get_eidcmd(uint16_t smartID, uint8_t **smartEidCmd, size_t *smartEidCmdLen);
esp_err_t rgmgt_smart_control(uint8_t trigger, uint16_t smartID, uint8_t *attrTrigger, uint8_t *timeCfg, uint8_t timeCfgLen);
esp_err_t rgmgt_smart_trigger(uint8_t *attrTrigger, uint8_t attrTriggerLen, rgsmt_trig_para_t *smartPara, rgsmt_trig_smart_t* smartData);
esp_err_t rgmgt_smart_trigger_handle(rgmsg_t *rogoCmdMsg, uint16_t *cfm);
esp_err_t rgmgt_smart_trigger_storage(uint16_t smartTrigEid, uint16_t smartTrigId,
                                      rgsmt_trig_para_t *smartTrigParaSrc, rgsmt_trig_smart_t *smartTrigDataSrc,
                                      uint8_t isRemove);
esp_err_t rgmgt_smart_trigger_get_param(uint16_t smartTrigId, uint16_t *smartTrigEid, rgsmt_trig_para_t *smartTrigPara);

#ifdef __cplusplus
}
#endif

#endif

