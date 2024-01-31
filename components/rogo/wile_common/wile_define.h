#pragma once
#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef CONFIG_OTA_ENABLE
#define CONFIG_OTA_ENABLE                       true
#endif

#ifdef CONFIG_OTA_ENABLE
#ifndef CONFIG_OTA_RECV_TIMEOUT
#define CONFIG_OTA_RECV_TIMEOUT                 8000
#endif
#ifndef CONFIG_OTA_SKIP_COMMON_NAME_CHECK
#endif
#ifndef CONFIG_OTA_SKIP_VERSION_CHECK
#endif
#ifndef CONFIG_OTA_ENABLE_PARTIAL_HTTP_DOWNLOAD
#endif
#ifndef CONFIG_OTA_HTTP_REQUEST_SIZE
#define CONFIG_OTA_HTTP_REQUEST_SIZE            MBEDTLS_SSL_IN_CONTENT_LEN
#endif
#endif

#define STEP_CFG_NONE                       0x00
#define STEP_CFG_DISCOVERY_WIFI             0x01
#define STEP_CFG_WAIT_CONNECT_WIFI          0x02
#define STEP_CFG_GET_WIFI_INFO              0x03
#define STEP_CFG_CONNECT_TO_WIFI            0x04
#define STEP_CFG_GET_CLOUD_CONFIG           0x05
#define STEP_CFG_SET_CLOUD_INFO             0x06
#define STEP_CFG_CONNECT_MQTT               0x07
#define STEP_CFG_WAIT_DEVICE_CONFIG         0x08
#define STEP_CFG_SET_DEVICE_CONFIG          0x0A
#define STEP_CFG_WAIT_COMPLETE_CONFIG       0x0B
#define STEP_CFG_COMPLETE                   0x0F
#define STEP_DEVICE_WAIT_DATA               0x10
#define STEP_DEVICE_GET_DATA                0x11
#define STEP_DEVICE_GET_DATA_COMPLETE       0x12

/* Firmware version */
#define FIRMWARE_MAJOR                      0x00
#define FIRMWARE_MINOR                      0x00
#define FIRMWARE_BUILD                      0x00
#define FIRMWARE_REVISION                   0x00

#define VERSION_TYPE_APP                    0x01
#define VERSION_TYPE_OS                     0x02
#define VERSION_TYPE_FIRMWARE               0x03
#define VERSION_TYPE_SDK                    0xFF

/* Provision status */
#define WILE_PROV_NONE                      0x00
#define WILE_PROV_DONE                      0x01

/* Wile mode */
#define WILE_MODE_MASTERSLAVE               0x01
#define WILE_MODE_MASTER                    0x02
#define WILE_MODE_SLAVE                     0x03
#define WILE_MODE_INDEPENDENT               0x04

/**/
#define CERT_START                          "-----BEGIN CERTIFICATE-----\n"
#define CERT_END                            "-----END CERTIFICATE-----"

#define NVS_PROV_STATE                      "prov"    
#define NVS_ROOT_DEV_ID                     "rootDevID"
#define NVS_ROOT_EID                        "rootEid"
#define NVS_ROOT_GROUP                      "rootGroup"
#define NVS_DEV_MAX_ADDR                    "devMaxAddr"
#define NVS_API_URL                         "apiUrl"
#define NVS_USER_ID                         "userID"
#define NVS_LOCATION_ID                     "locationID"
#define NVS_PARTNER_ID                      "partnerID"
#define NVS_HTTP_CERT                       "httpCert"
#define NVS_MQTT_CERT                       "mqttCert"
#define NVS_MQTT_ENP                        "mqttEnp"
#define NVS_MQTT_URL                        "mqttUrl"
#define NVS_MQTT_PORT                       "mqttPort"
#define NVS_MESH_INFO                       "meshInfo"
#define NVS_OTA_URL                         "otaUrl"
#define NVS_OTA_AUTO_UPDATE                 "otaUpdate"
#define NVS_TIMEZONE                        "timezone"
#define NVS_MESH_SEQ                        "meshSeq"
#define NVS_SMART_TRIGGER                   "smartTrigger"

/**/
#define MSG_HEADER_FULL                     0x00
#define MSG_HEADER_SHORT                    0x01
#define MSG_HEADER_SHORT_LEN                5   
#define MSG_HEADER_FULL_LEN                 23 //11 + 12
#define DEVICE_ID_LEN                       12

#define MSG_SRC_NET                         0x9F
#define MSG_SRC_NET_MQTT                    0x00
#define MSG_SRC_NET_TCP                     0x10
#define MSG_SRC_BLE_IP6                     0xA0
#define MSG_SRC_BLE_GATT                    0xB0
#define MSG_SRC_BLE_MESH                    0xB1

#define DEVICE_SYNC_FULL                    0x00
#define DEVICE_SYNC_FULL_WIFI               0x01
#define DEVICE_SYNC_1ST                     0x02 // deviceId, macAddress, eidInfo
#define DEVICE_SYNC_2ND                     0x03 // elementInfos
#define DEVICE_SYNC_LAST                    0x08 // deviceKey
#define DEVICE_SYNC_LAST_WIFI               0x09 // deviceKey


/* Device name */
#define DEVICE_NAME_IR_CONTROL              "RGWLIR63010001"
#define DEVICE_NAME_VRV_CONTROL             "RGWLAC60010001"
#define DEVICE_NAME_SWITCH_1                "RGWLSW0301010001"  // ROGO WILE SWITCH 1
#define DEVICE_NAME_SWITCH_2                "RGWLSW0302010001"  // ROGO WILE SWITCH 2
#define DEVICE_NAME_SWITCH_3                "RGWLSW0303010001"  // ROGO WILE SWITCH 3
#define DEVICE_NAME_SWITCH_4                "000008000C04009E"  // ROGO WILE SWITCH 4
#define DEVICE_NAME_FPT_PLUG                "RGRDWL0401010001"  // ROGO_ODM_FPT WILE Plug

#define DEVICE_NAME_IR_CONTROL_V2           "726738018C040002"

/* MSG Type */
#define MSG_TYPE_FULL                       0x00
#define MSG_TYPE_SHORT                      0x01

/* Wile MsgCmd */
#define MSG_WILE                            0xC6
#define CMD_WILE_STATUS                     0xFF

#define MSG_SETTING                         0x0F
#define CMD_SETTING_FIRM_OTA                0xFC
#define CMD_SETTING_VER_INFO                0xFD
#define CMD_SETTING_WIFI_SCAN               0x20
#define CMD_SETTING_WIFI_SCAN_RESULT        0x21
#define CMD_SETTING_WIFI_SSID_PWD           0x22
#define CMD_SETTING_CLOUD_INFO              0x80
#define CMD_SETTING_HTTPS_CERT              0x81
#define CMD_SETTING_MQTT_URL                0x84
#define CMD_SETTING_MQTT_CERT               0x85
#define CMD_SETTING_NETWORK_STATUS          0x02

#define MSG_MESH                            0xC2
#define CMD_MESH_KEY_INFO                   0xC9
#define CMD_MESH_KEY_NWK                    0xCA
#define CMD_MESH_KEY_APP                    0xCB

#define MSG_DEVICE                          0x04
#define CMD_DEVICE_SYNC_DEV_JOIN            0x05

/* Wile Block*/
#define BLOCK_FW_VER                        0x71
#define BLOCK_FW_VER_SIZE                   0x08
#define BLOCK_WIFI_SCAN_RESULT              0x04


/* MsgCmdType */
#define MSG_SET_UNACK                       0x09
#define MSG_SET_ACK                         0x0A
#define MSG_CONFIRM_SET_ACK_RECEIVED        0x0B //This use for device confirm received request set ack
#define MSG_GET                             0x0C
#define MSG_NOTIFY_MSG                      0x0D //This is use for msg event status, or get request
#define MSG_NOTIFY_SUCCESS                  0x0E //This use for device confirm received request set ack and execute request success
#define MSG_NOTIFY_FAILURE                  0x0F 

/* MsgCmdSection */
#define CMD_STATE                           0x01
#define CMD_GROUP                           0x03
#define CMD_DEVICE                          0x04
#define CMD_SMART                           0x0B
#define CMD_RULE                            0x0C
#define CMD_TIME                            0x0D
#define CMD_SETTING                         0x0F

#define CMD_SYNC                            (0xBC & 0xFF) //Todo deprecated
#define CMD_RESET                           (0xBE & 0xFF)

//From 192 for protocol section
#define CMD_ANDROID                         (0xC0 & 0xFF) //192
#define CMD_LINUX                           (0xC1 & 0xFF) //194
#define CMD_MESH                            (0xC2 & 0xFF) //194
#define CMD_ZIGBEE                          (0xC3 & 0xFF) //195
#define CMD_MATTER                          (0xC4 & 0xFF) //196
#define CMD_IR                              (0xC5 & 0xFF) //197
#define CMD_WILE                            (0xC6 & 0xFF) //198
#define CMD_MEDIA                           (0xF0 & 0xFF) //240

#define CMD_VENDOR                          (0xF0 & 0xFF) //240
#define CMD_CUSTOM                          (0xFF & 0xFF) //255


/* MsgCmdSectionExtra */
#define CMDs_WILE_STATUS                     0xFF

#define CMDs_STATE_CONTROL                   0x00
#define CMDs_STATE_EVENT                     0x01
#define CMDs_STATE_ELEMENT_ATTR              0x03//return with eid
#define CMDs_STATE_DEVICE_ATTR               0x04//return with eid
#define CMDs_STATE_SENSOR_LOG                0x05
#define CMDs_STATE_DEVICE_REPORT             0x06
#define CMDs_STATE_DEVICE_NOTIFY             0x07
#define CMDs_STATE_DEVICE                    0x0A
#define CMDs_STATE_DEVICE_ATTRIBUTES         0x0B

#define CMDs_RULE_CHECK_TRIGGER_EXT          0x01
#define CMDs_RULE_BIND_TRIGGER               0x05
#define CMDs_RULE_BIND_TRIGGER_WITH_EXT      0x06
#define CMDs_RULE_UNBOUND_TRIGGER            0x07
#define CMDs_RULE_BIND_TIMEJOB               0x08
#define CMDs_RULE_UNBOUND_TIMEJOB            0x09

#define CMDs_SMART_ACTIVE_BY_USER            0x01
#define CMDs_SMART_ACTIVE_BY_TRIGGER         0x02
#define CMDs_SMART_ACTIVE_BY_SCHEDULE        0x03
#define CMDs_SMART_REMOVE_ANNOUNCE           0x04
#define CMDs_SMART_BIND_TRIGGER              0x06
#define CMDs_SMART_UNBOUND_TRIGGER           0x07
#define CMDs_SMART_TRIGGER_MODE              0x08
#define CMDs_SMART_CHECK_TRIGGER_MIX         0x0A
#define CMDs_SMART_TRIGGER_UPDATE_ANNOUNCE   0x0B
#define CMDs_SMART_SCHEDULE_UPDATE_ANNOUNCE  0x0C
#define CMDs_SMART_SCHEDULE_REMOVE_ANNOUNCE  0x0D
#define CMDs_SMART_BIND_CMD                  0x0E
#define CMDs_SMART_UNBOUND_CMD               0x0F
#define CMDs_SMART_CHECK_SYNC                0xFF


#define CMDs_DEVICE_INFO                     0x01
#define CMDs_DEVICE_WAIT_CFG_DONE            0x02
#define CMDs_DEVICE_IDENTIFY                 0x03
#define CMDs_DEVICE_SYNC_DEV_JOINED          0x05
#define CMDs_DEVICE_SYNC_DEV_REMOVED         0x06
#define CMDs_DEVICE_SET_ELM_DEV_TYPE         0x07
#define CMDs_DEVICE_BIND_GRP                 0x0A
#define CMDs_DEVICE_UNBOUND_GRP              0x0B
#define CMDs_DEVICE_BIND_GRP_ELMS            0x0C
// #define CMDs_DEVICE_BIND_GRPCTL              0x0C
// #define CMDs_DEVICE_UNBOUND_GRPCTL           0x0D
#define CMDs_DEVICE_STATUS                   0xF0
#define CMDs_DEVICE_VENDORINFO_SET           (0xFF & 0xFF)


#define CMDs_SETTING_OTA_PROCESS             0xFE
#define CMDs_SETTING_FIRM_OTA                0xFC
#define CMDs_SETTING_VER_INFO                0xFD
#define CMDs_SETTING_WIFI_SCAN               0x20
#define CMDs_SETTING_WIFI_SCAN_RESULT        0x21
#define CMDs_SETTING_WIFI_SSID_PWD           0x22
#define CMDs_SETTING_CLOUD_INFO              0x80
#define CMDs_SETTING_HTTPS_CERT              0x81
#define CMDs_SETTING_MQTT_URL                0x84
#define CMDs_SETTING_MQTT_CERT               0x85
#define CMDs_SETTING_NETWORK_STATUS          0x02
    

#define CMDs_SYNC_ENTITIES_DATA              0x01
#define CMDs_SYNC_ENTITY_ITEM_DATA           0x02
#define CMDs_SYNC_NEW_DEVICE_JOINED          0x10
#define CMDs_SYNC_DEVICE_UPDATING            0x11
#define CMDs_SYNC_DEVICE_REMOVE              0x12
#define CMDs_SYNC_AUTOMATION                 0x13
#define CMDs_SYNC_AUTOMATION_REMOVE          0x14

#define CMDs_TIME_COUNTDOWN_CONTROL_ADD      0x01
#define CMDs_TIME_COUNTDOWN_CONTROL_REMOVE   0x02
//IR
#define CMDs_IR_CHECK_AVAILABLE              0x01
#define CMDs_IR_LEARN_STATUS                 0x02
#define CMDs_IR_PROTOCOL_DETECTED            0x04
#define CMDs_IR_DATA_LEARNED                 0x05
#define CMDs_IR_SEND_IR_CMD_RAW              0x06
#define CMDs_IR_SEND_RAW_DATA                0x0A
#define CMDs_IR_EXEC_PRTC                    0x0B
#define CMDs_IR_EXEC_PRTC_AC                 0x0C

// MESH
#define CMDs_MESH_AVAILABLE_INFO             0xC0
#define CMDs_MESH_UPDATE_DEVICE_KEY          0xC2
#define CMDs_MESH_FORCE_RESET_NODE           0xC3
#define CMDs_MESH_UPDATE_VENDOR_INFO         0xC4
#define CMDs_MESH_SEND_VENDOR_MSG            0xC5
#define CMDs_MESH_KEY_INFO                   0xC9
#define CMDs_MESH_KEY_NWK                    0xCA
#define CMDs_MESH_KEY_APP                    0xCB

#define MESH_MSG_TYPE_BATTERY                0x01
#define MESH_MSG_TYPE_BUTTON                 0x0A
#define MESH_MSG_TYPE_KNOB                   0x0B
#define MESH_MSG_TYPE_MOTION                 0x0C
#define MESH_MSG_TYPE_LUX                    0x0D
#define MESH_MSG_TYPE_DOOR                   0x0E
#define MESH_MSG_TYPE_SMOKE                  0x0F
#define MESH_MSG_TYPE_MOUNT                  0x10
#define MESH_MSG_TYPE_DUST_TEMP              0x11

/* MsgSenderType */
#define SENDER_UNKNOWN                       0x00
#define SENDER_DEVICE_ITSELF                 0x01
#define SENDER_APP                           0x0D
#define SENDER_WEB                           0x0E
#define SENDER_IOS                           0x0F
#define SENDER_ANDROID                       0x10
#define SENDER_DEVICE                        0x11
#define SENDER_SCENARIO                      0x12
#define SENDER_SCENARIO_UUID_V4              0x13
#define SENDER_AUTOMATION                    0x14
#define SENDER_AUTOMATION_UUID_V4            0x15
#define SENDER_SCHEDULE                      0x16
#define SENDER_SCHEDULE_UUID_V4              0x17
#define SENDER_GOOGLE_VOICE                  0x20
#define SENDER_ALEXA                         0x21
#define SENDER_MAIKA                         0x22

/* BlockType */
#define BLOCK_ID_HEX                         0x01
#define BLOCK_ID_STRING                      0x02
#define BLOCK_ID_UUID_V4                     0x03
#define BLOCK_DATA_STRING                    0x04
#define BLOCK_DATA_SHORT                     0x05
#define BLOCK_DATA_INTEGER                   0x06
#define BLOCK_DATA_LONG                      0x07
#define BLOCK_DATA_RAW                       0x09
#define BLOCK_KEY_STRING_OF_BLOCKDATA        0x0A
#define BLOCK_KEY_HEX_OF_BLOCKDATA           0x0B
#define BLOCK_MAC_HEX                        0x20
#define BLOCK_MAC_UUIDV4                     0x21
#define BLOCK_MAC_STRING                     0x22
#define BLOCK_RAM_ROM_MEMORY                 0x2A
#define BLOCK_EID                            0x40 // eid - 64
#define BLOCK_EID_ELM                        0x41 // eid - elm
#define BLOCK_TYPE_EID_ELM                   0x42 // device type - eid - elm
#define BLOCK_EID_GRP                        0x43 // eid - Group
#define BLOCK_EID_PRTC_ELMS                  0x44 // eid - protocol - elms
#define BLOCK_EID_INFO                       0x45 // eid - rootEid - protocol - nwkAddr - manuFactory - deviceType - elmSize - group
#define BLOCK_SMID                           0x50 // smid
#define BLOCK_SMID_EID                       0x52 // smid - eid
#define BLOCK_SMID_TRIGGER                   0x53 // smid - triggerType
#define BLOCK_SMART_CMD_INFO                 0x54 // Smid - EidOwner -  TargetEid - TargetPrtc - FilterDeviceType
#define BLOCK_SMART_TRIGGER_PRIMARY_INFO     0x5A // Type, Smid, eid, elm, condition
#define BLOCK_SMART_TRIGGER_PRIMARY_EXT_INFO 0x5B // Type, Smid, eid, elm, condition
#define BLOCK_SMART_TRIGGER_SECOND_INFO      0x5C // MixType(AND | OR), Eid, RootEid
#define BLOCK_SMART_TRIGGER_MIX_INFO         0x5D
#define BLOCK_SMART_TRIGGER_KEY              0x5F
#define BLOCK_INFO_3RD                       0x60 // linkType - other info
#define BLOCK_VENDOR_INFO                    0x61 // vendorId - companyId
#define BLOCK_MESH_DEVKEY                    0x62 // byte mesh device key
#define BLOCK_LIST_EID                       0x67 // eids - use when user get state gateway devices
#define BLOCK_IR_CMD_HEADER                  0xC0 // linkType - other info
#define BLOCK_IR_CMD_DATA                    0xC1 // linkType - other info
#define BLOCK_IR_RAW                         0xC2 // linkType - other info
#define BLOCK_NWK_IP_ADDR                    0x70
#define BLOCK_PROTOCOL_INFO                  0x72
#define BLOCK_PROTOCOL_EXT_INFO              0x73
#define BLOCK_TIMEJOB                        0x7A // time job
#define BLOCK_TIMECFG                        0x7B // time job
#define BLOCK_TIMEDATE                       0x7C
#define BLOCK_TIMESCHEDULE_DAYWEEK           0x7D
#define BLOCK_TIMEZONE_STR                   0x7E
#define BLOCK_ATTR_INFO                      0x7F
#define BLOCK_ATTR_VALUE                     0x80 //15
#define BLOCK_ELM_ATTR_VALUE                 0x81 //67
#define BLOCK_ELMS                           0x82 //64
#define BLOCK_ATTRS                          0x83 //64
#define BLOCK_ELM_TYPE_ATTRS                 0x84 //64
#define BLOCK_ELM_TYPE_GRPS                  0x85 //64
#define BLOCK_ATTR_VALUE_REVERSE_DELAY_ELM   0x86
#define BLOCK_GRP_ELMS                       0x88 //64
#define BLOCK_CINF                           0x8A //64
#define BLOCK_STATE_AND_LOG_CID              0x8B //65
#define BLOCK_LOG_DATA                       0x8C //65
#define BLOCK_CUSTOM_DATA                    0xDF //223
#define BLOCK_LARGE_DATA_RAW                 0xE0 //224 - 2 byte
#define BLOCK_LARGE_STRING                   0xE1 //225 - 2 byte


/* Device Type */
#define DEVICE_TYPE_OTHER                      -1
#define DEVICE_TYPE_ALL                         0
#define DEVICE_TYPE_LIGHT                       2
#define DEVICE_TYPE_SWITCH                      3
#define DEVICE_TYPE_PLUG                        4
#define DEVICE_TYPE_CURTAINS                    5
#define DEVICE_TYPE_DOORLOCK                    6
#define DEVICE_TYPE_DOORBELL                    8
#define DEVICE_TYPE_MEDIA_BOX                   10
#define DEVICE_TYPE_USB_DONGLE                  11
#define DEVICE_TYPE_REPEATER                    12
#define DEVICE_TYPE_CAMERA                      13
#define DEVICE_TYPE_AC                          16
#define DEVICE_TYPE_TV                          17
#define DEVICE_TYPE_FAN                         18
#define DEVICE_TYPE_MOTOR_CONTROLLER            19
#define DEVICE_TYPE_BUTTON_DASH                 20
#define DEVICE_TYPE_SWITCH_SCENE                21
#define DEVICE_TYPE_TEMP_SENSOR                 30
#define DEVICE_TYPE_DOOR_SENSOR                 31
#define DEVICE_TYPE_SMOKE_SENSOR                32
#define DEVICE_TYPE_MOTION_LUX_SENSOR           33
#define DEVICE_TYPE_MOTION_SENSOR               34
#define DEVICE_TYPE_LUX_SENSOR                  35
#define DEVICE_TYPE_DUST_SENSOR                 36
#define DEVICE_TYPE_AC_CONTROLLER               96
#define DEVICE_TYPE_IR_DEVICE_CONTROLLER        99
#define DEVICE_TYPE_GATE                        100
#define DEVICE_TYPE_GATEWAY                     192
#define DEVICE_TYPE_SENSOR_DEVICE               1001
#define DEVICE_TYPE_OTHER_CONTROL_DEVICE        1002
#define DEVICE_TYPE_IR_DEVICE_OLD               1003

/* Manufacturer */
#define MANU_UNKNOWN                         -1
#define MANU_ROGO                             0
#define MANU_FPT_PLAY                         1
#define MANU_AQARA                            30
#define MANU_TUYA                             40
#define MANU_YALE                             50
#define MANU_RANDONG                          99
#define MANU_DIENQUANG                        100
#define MANU_ROGO_RAL_OEM_FPT_PLAY            101
#define MANU_HIK                              214
#define MANU_HONEYWELL                        215
#define MANU_AOSMITH                          216
#define MANU_APPLE                            254
#define MANU_OTHER                            255
#define MANU_XIAOMI                           500
#define MANU_VIVO                             501
#define MANU_SKYWORTH                         502
#define MANU_HUAWEI                           503
#define MANU_HINSENSE                         504
#define MANU_GALANZ                           505
#define MANU_SAMSUNG                          1000
#define MANU_SONY                             1001
#define MANU_LG                               1002
#define MANU_SHARP                            1003
#define MANU_TCL                              1004
#define MANU_CASPER                           1005
#define MANU_TOSHIBA                          1006
#define MANU_PANASONIC                        1007
#define MANU_LENOVO                           1008
#define MANU_HITACHI                          1009
#define MANU_DAIKIN                           1010
#define MANU_MIDEA                            1011
#define MANU_GREE                             1012
#define MANU_ELECTRONUX                       1013
#define MANU_FUJITSU                          1014
#define MANU_HUYNDAI                          1015
#define MANU_MITSUBISHI_ELECTRIC              1016
#define MANU_SANYO                            1017
#define MANU_BEKO                             1018
#define MANU_PHILIPS                          1019
#define MANU_AQUA                             1020
#define MANU_HAIER                            1021
#define MANU_CORONA                           1022
#define MANU_NEC                              1023
#define MANU_NATIONAL                         1024
#define MANU_YORK                             1025 // USA
#define MANU_CHIGO                            1026 // HONGKONG
#define MANU_JVC                              1027
#define MANU_VIZIO                            1028 // HONGKONG
#define MANU_NAGAKAWA                         1029 // HONGKONG
#define MANU_DAEWOO                           2002
#define MANU_MITSUBISHI                       2003
#define MANU_MITSUBISHI_INDUSTRY              2004
#define MANU_FAN_GENERIC                      2005

/* Rogo Protocol */
#define NORMAL_CTL                            0
#define GROUP_CTL                             1
#define LOCATION_CTL                          2
#define ROGO_GATEWAY                          3
#define USB_ZIGBEE                            4

#define USB_MESH                              5
#define USB_ZIGBEE_MESH                       6

#define ROGO_IR_PRTC                          8
#define ROGO_IR_PRTC_AC                       9
#define ROGO_IR_RAWZIP                        10
#define ROGO_IR_RAW                           11
#define ROGO_WIFI                             12
#define ROGO_WIFI_BLE                         13

//All BluetoothSigbMess will be from 20 -> 100
#define WILE_STANDARD                         20
#define SIGMESH_STANDARD                      30
#define SIGMESH_SWITCH_ROGOV1                 31
#define SIGMESH_ROGO_RD_SWITCHPLUGMOTORV2     32
#define SIGMESH_ROGO_RD_SENSOR                33

#define ZIGBEE_GENERIC                        200
#define ZIGBEE_DIENQUANG                      201

#define IP_CAMERA                             239
#define IR_GENERIC                            240
#define IR_SEI                                241
#define IR_NEC                                242
#define IR_FAN                                243

/* CMD Const */
#define CTR_MAXB                              1000
#define CTR_MINK                              2700
#define CTR_MAXK                              6500
#define CTR_ONOFF_OFF                         0
#define CTR_ONOFF_ON                          1
#define CTR_OPENCLOSE_MODE_CLOSE              0
#define CTR_OPENCLOSE_MODE_OPEN               1
#define CTR_OPENCLOSE_MODE_STOP               2
#define CTR_OPENCLOSE_MODE_MOVING             3
#define CTR_BTN_PRESS_SINGLE                  0
#define CTR_BTN_PRESS_DOUBLE                  1
#define CTR_BTN_PRESS_LONG                    2

#define CTR_DOOR_LOCKED                       0
#define CTR_DOOR_UNLOCKED                     1

#define CTR_AC_MODE_AUTO                      0
#define CTR_AC_MODE_COOLING                   1
#define CTR_AC_MODE_DRY                       2
#define CTR_AC_MODE_HEATING                   3
#define CTR_AC_MODE_FAN                       4

#define CTR_FAN_SPEED_AUTO                    0
#define CTR_FAN_SPEED_LOW                     1
#define CTR_FAN_SPEED_NORMAL                  2
#define CTR_FAN_SPEED_HIGH                    3
#define CTR_FAN_SPEED_MAX                     4
#define CTR_FAN_SPEED_DISABLE                 7

#define CTR_FAN_SWING_AUTO                    0
#define CTR_FAN_SWING_LOW                     1
#define CTR_FAN_SWING_NORMAL                  2
#define CTR_FAN_SWING_HIGH                    3
#define CTR_FAN_SWING_OFF                     255

#define CTR_AC_FEATURE_NUM                    5
#define CTR_LIGHT_FEATURE_NUM                 6

/* CONDITION */
#define COND_ANY                              1
#define COND_EQUAL                            2
#define COND_EXIST                            3
#define COND_BETWEEN                          4
#define COND_LESS_THAN                        5
#define COND_LESS_EQUAL                       6
#define COND_GREATER_THAN                     7
#define COND_GREATER_EQUAL                    8
#define COND_RULE_TO_VALUE                    9   // this use for define
#define COND_CUSTOM                           255

/* SMART TYPE */
#define SMART_TYPE_MIX_OR
#define SMART_TYPE_MIX_AND
#define SMART_TYPE_STAIR_SWITCH                 64
#define SMART_TYPE_NOTIFICATION                 65
#define SMART_TYPE_DETECTED_MOTION              66
#define SMART_TYPE_DETECT_NO_MOTION_BY_EVENT    67
#define SMART_TYPE_DETECT_NO_MOTION_BY_TIME     68
#define SMART_TYPE_REVERSE_ON_OFF               70
#define SMART_TYPE_SYNC_ON_OFF_MULTI_DEVICE     71
#define SMART_TYPE_TRIGGER_ACTION_SWITCH_SCENE  96  // 96 -> 104 for type mapping
#define SMART_TYPE_TRIGGER_ACTION_OTHER         104 // 96 -> 104 for type mapping
#define SMART_TYPE_ZONE_CONTROL                 128

#define SMART_TRIGGER_TYPE_OWNER                0
#define SMART_TRIGGER_TYPE_EXT                  1
#define SMART_TRIGGER_TYPE_EXT_REPORT_DATA      2

/* IoT Feature */
    //GENERIC FEATURES
#define FEATURE_NONE                          0
#define FEATURE_ONOFF                         1
#define FEATURE_OPEN_CLOSE_CTL                2
#define FEATURE_LOCK_UNLOCK                   3

#define FEATURE_BATTERY                       9
#define FEATURE_POSITION                      11

#define FEATURE_MODE                          17
#define FEATURE_FAN_SWING                     18
#define FEATURE_FAN_SPEED                     19
#define FEATURE_TEMP_SET                      20
#define FEATURE_CAMERA_STREAMING              22

    //LIGHT FEATURES
#define FEATURE_BRIGHTNESS                    28
#define FEATURE_KELVIN                        29
#define FEATURE_BRIGHTNESS_KELVIN             30
#define FEATURE_COLOR_HSV                     31
#define FEATURE_COLOR_HSL                     32
#define FEATURE_LEVEL                         33

    //EVENT FEATURES
#define FEATURE_HUMID_EVT                     48
#define FEATURE_TEMP_EVT                      49
#define FEATURE_TEMP_HUMID_EVT                50
#define FEATURE_OPEN_CLOSE_EVT                51
#define FEATURE_MOTION_LUX_EVT                52
#define FEATURE_MOTION_EVT                    53
#define FEATURE_LUX_EVT                       54
#define FEATURE_SMOKE_EVT                     55
#define FEATURE_WALL_MOUNTED_EVT              56
#define FEATURE_BUTTON_PRESS_EVT              60
// #define FEATURE_DUST_PM1_25_10                61
#define FEATURE_KNOB_EVT                      61
#define FEATURE_LEVEL_EVT                     62

#define FEATURE_MOTOR_CALIB_TIME_INFO         226
#define FEATURE_MOTOR_TYPE_INFO               227
#define FEATURE_VENDOR                        225

#define FEATURE_REVERSE_ONOFF                 310
#define FEATURE_SYNC_ONOFF_NORMAL             311
#define FEATURE_SYNC_ONOFF_REVERSING          312
#define FEATURE_SYNC_ONOFF_STAIR_SWITCH       313
#define FEATURE_SWITCHING_VALUE_BY_TIME       314

#define FEATURE_BRIGHTNESS_UP                 320
#define FEATURE_BRIGHTNESS_DOWN               321
#define FEATURE_KELVIN_WARM_UP                322
#define FEATURE_KELVIN_WARM_DOWN              323
#define FEATURE_PUSH_NOTIFICATION             330

#define FEATURE_SIMULATOR_REMOTE_ANDROID_TV   332

    //OTHER VALUE & FEATURE
#define FEATURE_IR                            81
#define FEATURE_IR_KEY                        82
#define FEATURE_IR_RAW                        83
#define FEATURE_AC                            257
#define FEATURE_AC_SHORT                      258
#define FEATURE_AC_EXTRA                      259

// Wile in-device feature
#define FEATURE_SMOKE_BATTERY                 65000
#define FEATURE_MOUNT_BATTERY                 65001

#define FEATURE_SINGLE_PRESS_EVT_DEPRECATED   90 //Deprecated, end when old automation remove
#define FEATURE_LONG_PRESS_EVT_DEPRECATED     91 //Deprecated, end when old automation remove
#define FEATURE_DOUBLE_PRESS_EVT_DEPRECATED   92 //Deprecated, end when old automation remove
#define FEATURE_ONOFF_SWITCH_ROGO_V1_DEPRECATED 800 // Will be deprecated, end when old automation remove
#define FEATURE_ONOFF_SWITCH_ROGO_V2_DEPRECATED 801 // Will be deprecated, end when old automation remove
#define FEATURE_CMD_FORCE_RESET_MESH_DEVICE_DEPRECATED  462 // Will be deprecated, end when old automation remove

/* Error code */
#define ERR_RG_UNDERFINE                             0
#define ERR_RG_NULL_ENTITY                           1
#define ERR_RG_NULL_ID                               2
#define ERR_RG_TIMEOUT                               20
#define ERR_RG_NOTSUPPORT                            21
#define ERR_RG_DUPLICATED                            22
#define ERR_RG_IOTMSG_NOTREADY                       23
#define ERR_RG_DATA_SPAM                             24
#define ERR_RG_DEVICE_NOTFOUND                       10
#define ERR_RG_GROUP_NOTFOUND                        11
#define ERR_RG_LOCATION_NOTFOUND                     12
#define ERR_RG_SMART_NOTFOUND                        14
#define ERR_RG_TARGET_TYPE_NOT_VALID                 30
#define ERR_RG_NWK_NOT_READY                         128
#define ERR_RG_NWK_MGMT_NOT_AVAILABLE                129
#define ERR_RG_OUT_MEMORY                            255
#define ERR_RG_HTTP_400                              400
#define ERR_RG_HTTP_404                              404
#define ERR_RG_HTTP_EXCEPTION                        20

/* WIFI status */
#define WIFI_NONE                                    0
#define WIFI_CONNECTING                              1
#define WIFI_CONNECTED                               2
#define WIFI_DISCONNECTING                           3
#define WIFI_DISCONNECTED                            4
#define WIFI_LINKLOSS                                5
#define WIFI_CLOSE                                   8
#define WIFI_RETRY_CONNECTING                        9
#define WIFI_PASSWORD_WRONG                          20
#define WIFI_SSID_NOTFOUND                           21

#define TIME_CFG_TYPE_GENERIC_TIME                   0
#define TIME_CFG_TYPE_MIN_TIME                       1 //That mean need check condition minimum time
#define TIME_CFG_TYPE_REVERSE_TIME                   2 //That mean how long to reverse
#define TIME_CFG_TYPE_WAITING_TIME                   3 //That mean how long to do something
#define TIME_CFG_TYPE_OVERRIDE_TIME                  4

#endif