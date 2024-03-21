/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include "board.h"

#if defined(FLASH_APP_VTOR_OFFSET) && FLASH_APP_VTOR_OFFSET > 0
static int ota_app_vtor_reconfig(void) {
#define NVIC_VTOR_MASK 0x3FFFFF80
    /* Set the Vector Table base location by user application firmware definition */
    SCB->VTOR = STM32_FLASH_START_ADRESS | (FLASH_APP_VTOR_OFFSET * 1024);
    return 0;
}
INIT_BOARD_EXPORT(ota_app_vtor_reconfig);
#endif // FLASH_APP_VTOR_OFFSET
