/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-5      SummerGift   first version
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <rtthread.h>
#include <stm32f7xx.h>
#include "drv_common.h"
#include "drv_gpio.h"
//
#include "board_config.h"
#include "board_dma_config.h"
#include "board_pwm_config.h"
#include "board_uart_config.h"
#include "board_spi_config.h"
#include "board_usbd_config.h"
#include "board_sdio_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STM32_FLASH_START_ADRESS ((uint32_t)0x08000000)
#define STM32_FLASH_SIZE         (1024 * 2048)
#define STM32_FLASH_END_ADDRESS  ((uint32_t)(STM32_FLASH_START_ADRESS + STM32_FLASH_SIZE))

#define STM32_SRAM_SIZE (512)
#define STM32_SRAM_END  (0x20000000 + STM32_SRAM_SIZE * 1024)

#if defined(__ARMCC_VERSION)
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN (&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section = "CSTACK"
#define HEAP_BEGIN (__segment_end("CSTACK"))
#else
extern int __bss_end;
#define HEAP_BEGIN (&__bss_end)
#endif

#define HEAP_END STM32_SRAM_END

void SystemClock_Config(void);
int  main();

#ifdef __cplusplus
}
#endif

#endif
