#pragma once
#ifndef _BOARD_TEST_H_
#define _BOARD_TEST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "board.h"
#include "board_led.h"

#if CONFIG_BOARD_TEST_ENABLE
#define TEST_PARTITION           "test_rd"
#define TEST_NAMESPACE           "test_param"

#define TEST_01_DURATION         30    // 30 minutes
#define TEST_01_PERIOD_TEST      3000  // 3 seconds
#define TEST_01_PERIOD_SAVE      60000 // 60 seconds
#define TEST_02_PERIOD_TEST      1000  // 1 seconds

#define TEST_WIFI_SSID           "FPT_CT"
#define TEST_WIFI_PWD            "123456789"

#define TEST_STATE_00            0x00
#define TEST_STATE_01            0x01
#define TEST_STATE_02            0x02
#define TEST_STATE_DONE          0xFF

void board_test_check(void);
void board_test_01(void);
void board_test_02(void);
void board_test_save(const char *key, uint8_t value);
#endif // CONFIG_BOARD_TEST_ENABLE

#ifdef __cplusplus
}
#endif

#endif