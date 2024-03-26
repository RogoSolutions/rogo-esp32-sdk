#pragma once
#ifndef _WILE_MESSAGE_H_
#define _WILE_MESSAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "wile_define.h"
#include "wile_init.h"
#include "wile_mqtt.h"
#include "wile_device.h"

// typedef struct {
//     uint8_t src;
//     uint8_t *srcMsg;
//     uint16_t srcMsgLen;
//     void *arg;
// } rgmsg_t;

void rgmsg_create_header(struct rogoIotMsg *msg, uint8_t msgType, uint8_t msgSection, uint8_t msgCmd, uint8_t msgData0, uint8_t msgData1);
void rgmsg_set_section(struct rogoIotMsg *msg, uint8_t section, uint8_t sectionExt);
void rgmsg_set_msgdata(struct rogoIotMsg *msg, uint8_t msgData0, uint8_t msgData1);

void rgmsg_add_full_header(struct rogoIotMsg *msg, uint8_t *sender);
void rgmsg_add_full_header_default(struct rogoIotMsg *msg);

void rgmsg_add_block(struct rogoIotMsg *msg, uint8_t blockType, uint16_t blockLen, uint8_t *data, int16_t srcEid);
void rgmsg_add_block_state(struct rogoIotMsg *msg, uint16_t eid, uint8_t elmIdx, uint16_t stateType, uint8_t *deviceState, size_t deviceStateLen);
uint8_t rgmsg_feature_size(uint16_t featureType);

esp_err_t rgmsg_handle(uint8_t src, uint8_t *srcMsg, uint16_t srcMsgLen, void *arg, uint16_t argLen);
esp_err_t rgmsg_handle_response(struct rogoIotMsg *rogoCmdMsgResponse, rgmsg_t *rogoCmdMsg);

esp_err_t rgblk_peek_type(rgmsg_t *rogoCmdMsg, uint16_t *blockStart, uint8_t *blockType);
esp_err_t rgblk_read_data(uint8_t blockType, rgmsg_t *rogoCmdMsg, uint16_t *blockStart, uint8_t **data, uint16_t *dataLen);
esp_err_t rgblk_read_data_short(uint8_t blockType, rgmsg_t *rogoCmdMsg, uint16_t *blockStart, uint16_t *data);
esp_err_t rgblk_read_eid_info(rgmsg_t *rogoCmdMsg, uint16_t *blockStart, devEidInfo_t *devInfo);
void      rgblk_offset(uint8_t *blockData, uint16_t *blockStart);

esp_err_t rgmsg_send_notification(uint16_t eid, uint16_t elm, uint16_t type, uint8_t *value);

void save_config_to_nvs(void *data, uint16_t length, uint8_t type, const char *key);
void add_device_info_to_nvs(uint16_t eid, uint8_t *deviceID, uint8_t *mac);
esp_err_t add_device_to_nvs(uint8_t *eidInfo, uint8_t *elmInfo, uint8_t *meshDevKey);
esp_err_t group_to_nvs(uint16_t eid, uint16_t group, uint16_t oldGroup);
esp_err_t smart_to_nvs(uint16_t eid, uint16_t smartID, uint8_t smartDel, uint8_t *cmd, uint8_t cmdLen);
esp_err_t schedule_to_nvs(uint16_t smartID, uint16_t schTime, uint8_t removeSch);

esp_err_t read_config_from_nvs(const char *key, uint8_t type, void *out, uint16_t *length);

void time_minute_callback(void* arg);
void task_rogo_schedule(void *pvParameters);

uint16_t nwk_addr_to_eid(uint16_t nwkAddr);
esp_err_t rogo_mesh_msg_handle(uint32_t opcode, uint16_t nodeAddr, uint8_t *msg, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif