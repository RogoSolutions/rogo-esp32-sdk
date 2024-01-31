#ifndef _APP_IR_H_
#define _APP_IR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "wile_define.h"
#include "wile_init.h"
#include "wile_message.h"
#include "wile_mqtt.h"

    extern uint8_t irSendDone;
    extern void ir_begin();
    extern void ir_send_raw(uint16_t *rawIR, uint16_t irRawLength);
    int8_t ir_send_prtc(int16_t protocol, uint64_t code);
    extern void ir_raw_block_handler(uint8_t *cmd, uint16_t irHeaderBlock, uint16_t irDataBlock);
    extern void ir_full_raw_block_handler(uint8_t *cmd, uint16_t irDataRawBlock);
    extern uint8_t ir_send_value(const uint64_t data, const uint16_t nbits);
    extern uint8_t ir_send_daikin_152(uint8_t on, uint8_t temp);
    extern void ir_recv_begin();
    uint8_t ir_decode_from_raw(uint16_t *raw, uint16_t raw_len);
    int8_t ir_decode(int16_t *protocol, char **code, uint16_t **rawData, uint16_t *rawLen);
    void ir_proto_meaning(int16_t protocol, int16_t *type, int16_t *manu);
    void ir_send_ac(int16_t protocol, uint8_t power, uint8_t mode, uint8_t temp, uint8_t fan, uint8_t swing);
    void ir_set_ac_state(uint16_t type, uint16_t value);
    uint16_t ac_state_to_value(uint8_t tempAllow, uint8_t fanAllow);
    void ir_decode_task(void *pvParameters);
    void ir_decode_idle_task(void *pvParameters);
    // EXTERN API
    extern uint8_t send_ir_raw_nvs(char *eid, char *dataName);
    extern void send_ir_prtc_nvs(void *pvParameters);
    extern void ac_state_send(void *pvParameters);
    extern void ac_state_send_test(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif