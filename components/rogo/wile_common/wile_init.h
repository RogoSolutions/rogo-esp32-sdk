#pragma once
#ifndef _WILE_PROVISION_H_
#define _WILE_PROVISION_H_

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
#include "esp_timer.h"

#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
// #include "driver/temp_sensor.h" // 4.4
#include "driver/temperature_sensor.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

// #include "board.h"
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
#include "nimble/ble.h"
#ifndef CONFIG_IDF_TARGET_ESP32C6
#include "esp_nimble_hci.h"
#endif
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
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

//SNTP
#include "esp_sntp.h"

//MQTT
#include "mqtt_client.h"
#include "esp_tls.h"

//OTA
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"

#include "wile_define.h"
// #include "wile_mesh.h"

#define TAG "WILE"
#define MESH_TAG "MESH"
#define WIFI_TAG "WIFI"

// #define DEVICE_NAME                  DEVICE_NAME_IR_CONTROL
// #define DEVICE_NAME                  DEVICE_NAME_VRV_CONTROL
// #define DEVICE_NAME                  DEVICE_NAME_FPT_PLUG
// #ifdef CONFIG_DEVICE_NAME
// #define DEVICE_NAME                  CONFIG_DEVICE_NAME
// #endif

// #ifndef CONFIG_DEVICE_NAME
// #define DEVICE_NAME                  "WILE_DEV_NODE"
// #endif

#define WILE_PROVISION_STATE         0x01
#define WILE_WIFI_SCAN               0x02
#define WILE_WIFI_CONNECT            0x03
#define WILE_MESH_INFO               0x04

#define WILE_DISCONNECT              0x00
#define WILE_CONNECTED               0x01
#define WILE_FAIL                    0x10

/* WiLe direct message */
#define DMSG_FIRMWARE_VERSION        0x01
#define DMSG_INFO                    0x02 // Wile mode, EID,...
#define DMSG_WIFI                    0x03
#define DMSG_API                     0x04 // code, url,...
#define DMSG_MESH                    0x05
#define DMSG_GROUP                   0x06
#define DMSG_TIME                    0x07
#define DMSG_SMART                   0x08
#define DMSG_DEVICE                  0x09 // control
#define DMSG_VENDOR                  0xC0
#define DMSG_RESET                   0xFF

#define DMSG_RESPONSE                0x00

#define DMSG_TYPE_GET                0x00
#define DMSG_TYPE_SET                0x01
#define DMSG_TYPE_OTHER              0xC0

#ifdef CONFIG_PRODUCTION_MODE
// #define LOG_LOCAL_LEVEL              ESP_LOG_WARN
#endif

// extern uint8_t prov_state[5];
extern uint8_t PROV_STATE;

// extern TaskHandle_t ledFast;
// extern TaskHandle_t ledSlow;
extern TaskHandle_t ledIndicateHandle;
// extern TaskHandle_t otaTask;

extern const uint8_t wileMode;
// extern uint8_t provStatus;

extern uint16_t tid;
extern uint8_t bleConnected;
extern uint8_t wifiConnected;
extern bool    wifiStarted;
extern uint16_t tcpPort;

extern char *httpCert;
extern uint16_t httpCertLength;
extern char *mqttCert;
extern uint16_t mqttCertLength;
extern char *otaUrl;
extern char *timezoneStr;
extern struct timeval tv_now;
extern struct tm timeInfoNow;
extern struct tm timeLocalNow;
extern uint64_t lastTimeActivate;
extern int8_t otaReady;
extern uint8_t otaAutoUpdate;

// extern char *mqtt_rogo_pem_start;

extern struct bt_mesh_device_network_info mesh_info;

extern uint8_t rootDevID[DEVICE_ID_LEN];
extern uint8_t devID[DEVICE_ID_LEN];
extern char rootDevIdStr[DEVICE_ID_LEN*2+1];
// extern uint8_t eid[2];
// extern uint8_t rootEid[2];
extern uint16_t rootEID;
extern uint16_t rootGroup;

extern uint16_t devNwkAddr;
extern uint16_t devManuFactory;
extern uint16_t devType;

extern uint8_t locationID[12];
extern uint32_t mqttPort;

extern uint32_t meshSeq;
extern uint8_t devTaskRunning;
extern QueueHandle_t devControlParaQueue;
extern QueueHandle_t rgmgtDevControlParaQueue;
#define RG_DEV_CONTROL_QUEUE_SIZE           16
#define RG_SMART_EVENT_CHECK_TIME           100
extern EventGroupHandle_t rgmgtSmartControlEvt;
extern uint8_t *devControlPara;

// extern char *mqttEnp;
// extern char *mqttUrl;
// extern char *partnerID;
// extern char *userID;

extern struct rogoBlockString{
    char *value;
    uint8_t len;
} mqttUrl, mqttEnp, partnerID, userID, locationIDstr, apiUrl;

struct rogoIotMsg{
    uint8_t *buf;
    uint16_t len;
};

struct rogoIotCmd{
    uint8_t *cmd;
    char *ipAddr;
};

// struct u16Data{
//     uint16_t *data;
//     uint16_t len;
// };

typedef struct {
    uint8_t src;
    uint8_t *srcMsg;
    uint16_t srcMsgLen;
    void *arg;
} rgmsg_t;

typedef struct devEidInfo{
    uint16_t eid;
    uint16_t rootEid;
    uint16_t protocol;
    uint16_t nwkAddr;
    uint16_t manufactory;
    uint16_t type;
    uint16_t elmNum;
    uint16_t group;
} devEidInfo_t;

typedef struct rgdev_control_para{
    uint16_t deviceType;
    uint16_t eid;
    uint16_t element;
    uint16_t delay;
    uint16_t reverse;
    uint8_t  report;
    uint8_t  trigger; // is check trigger smart
    uint16_t smart; // control from smart
    uint16_t feature;
    uint8_t  *featureValue;
} rgdev_control_para_t;

typedef struct rgsmt_trig_para{
    uint16_t elm;
    uint16_t attr;
    uint16_t value[3];
    uint8_t cond;
    uint8_t en;
    uint16_t smart;
    uint16_t cfm;
} rgsmt_trig_para_t;

typedef struct rgsmt_trig_smart{
    uint16_t type;
    uint8_t  key[8];
    uint8_t  timeCfg[4];
    uint16_t timeStart;
    uint16_t timeStop;
    uint8_t  weekDay;
    uint8_t  zone;
} rgsmt_trig_smart_t;

typedef struct rgdev_log_part{
    uint16_t day;
    uint16_t year;
    uint16_t elm;
    uint16_t attr;
} rgdev_log_part_t;


struct lightState{
    int16_t power;
    int16_t brightness;
    int16_t kelvin;
    int16_t colorH;
    int16_t colorS;
    int16_t colorL;
};

struct acState{
    uint8_t power;
    uint8_t mode;
    uint8_t temp;
    uint8_t fan;
    uint8_t swing;
};

struct smokeState{
    bool smoke;
    bool mount;
    uint8_t battery;
};

struct doorState{
    bool door;
    bool mount;
    uint8_t battery;
};

extern int16_t irProtocol;

#ifdef CONFIG_ESP_TEMPERATURE_ENABLE
extern temperature_sensor_handle_t espTemperature;
#endif

esp_err_t rgmgt_wile_init(void);
void wifi_init(void);
esp_err_t wile_bluetooth_init(void);
esp_err_t wile_ble_mesh_init(void);
esp_err_t wile_dev_control_init(void);
void esp32_temp_sensor_init(void);
void read_config_data(void);
void sntp_time_init(void);
void rogo_schedule_init(void);
void root_device_factory_reset(void);

#ifdef __cplusplus
}
#endif

#endif