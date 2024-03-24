/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#include <rtconfig.h>
#include <board.h>
#include <stdlib.h>

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev stm32_onchip_flash_32k;
extern const struct fal_flash_dev stm32_onchip_flash_128k;
extern const struct fal_flash_dev stm32_onchip_flash_256k;

/* flash device table */
#define FAL_FLASH_DEV_TABLE           \
    {                                 \
        &stm32_onchip_flash_32k,      \
            &stm32_onchip_flash_128k, \
            &stm32_onchip_flash_256k, \
    }
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                               \
    {                                                                                \
        {FAL_PART_MAGIC_WROD, "boot", "onchip_flash_32k", 0, 4 * 32 * 1024, 0},      \
            {FAL_PART_MAGIC_WROD, "param", "onchip_flash_128k", 0, 128 * 1024, 0},   \
            {FAL_PART_MAGIC_WROD, "app", "onchip_flash_256k", 0, 7 * 256 * 1024, 0}, \
    }
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */
