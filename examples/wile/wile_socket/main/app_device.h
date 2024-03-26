#pragma once
#ifndef _APP_DEVICE_H_
#define _APP_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "wile_init.h"
#include "wile_message.h"

esp_err_t root_device_control(uint16_t element, uint16_t type, uint8_t *value);
void      root_device_identify(void);
void      root_device_network_indicate(esp_event_base_t event_base, int32_t event_id, void* event_data);
void      root_device_delete_indicate(void);
void      root_device_prov_complete(void);
void      root_device_prov_none(void);
esp_err_t root_device_set_state(uint16_t type, uint8_t *value);
esp_err_t root_device_get_state(void **state, uint8_t *stateSize);

// void root_device_prov_complete_sync_state();
#ifdef __cplusplus
}
#endif

#endif