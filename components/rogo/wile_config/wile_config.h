#pragma once
#ifndef _WILE_CONFIG_H_
#define _WILE_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "wile_init.h"
#include "sdkconfig.h"

const char* rgmgt_config_device_id(void);
esp_err_t   rgmgt_config_nvs_enc(void);

#ifdef __cplusplus
}
#endif

#endif