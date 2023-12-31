/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "hal/mmu_hal.h"
#include "hal/cache_hal.h"
#include "esp_cache.h"
#include "esp_private/critical_section.h"

static const char *TAG = "cache";

#if SOC_CACHE_WRITEBACK_SUPPORTED
DEFINE_CRIT_SECTION_LOCK_STATIC(s_spinlock);
#endif  //#if SOC_CACHE_WRITEBACK_SUPPORTED


esp_err_t esp_cache_msync(void *addr, size_t size, int flags)
{
    ESP_RETURN_ON_FALSE_ISR(addr, ESP_ERR_INVALID_ARG, TAG, "null pointer");
    ESP_RETURN_ON_FALSE_ISR(mmu_hal_check_valid_ext_vaddr_region(0, (uint32_t)addr, size, MMU_VADDR_DATA), ESP_ERR_INVALID_ARG, TAG, "invalid address");

#if SOC_CACHE_WRITEBACK_SUPPORTED
    if ((flags & ESP_CACHE_MSYNC_FLAG_UNALIGNED) == 0) {
        uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_TYPE_DATA);

        ESP_RETURN_ON_FALSE_ISR(((uint32_t)addr % data_cache_line_size) == 0, ESP_ERR_INVALID_ARG, TAG, "start address isn't aligned with the data cache line size (%d)B", data_cache_line_size);
        ESP_RETURN_ON_FALSE_ISR((size % data_cache_line_size) == 0, ESP_ERR_INVALID_ARG, TAG, "size isn't aligned with the data cache line size (%d)B", data_cache_line_size);
        ESP_RETURN_ON_FALSE_ISR((((uint32_t)addr + size) % data_cache_line_size) == 0, ESP_ERR_INVALID_ARG, TAG, "end address isn't aligned with the data cache line size (%d)B", data_cache_line_size);
    }

    uint32_t vaddr = (uint32_t)addr;

    esp_os_enter_critical_safe(&s_spinlock);
    cache_hal_writeback_addr(vaddr, size);
    esp_os_exit_critical_safe(&s_spinlock);

    if (flags & ESP_CACHE_MSYNC_FLAG_INVALIDATE) {
        esp_os_enter_critical_safe(&s_spinlock);
        cache_hal_invalidate_addr(vaddr, size);
        esp_os_exit_critical_safe(&s_spinlock);
    }
#endif

    return ESP_OK;
}
