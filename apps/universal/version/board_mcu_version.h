/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#ifndef __BOARD_MCU_VERSION_H__
#define __BOARD_MCU_VERSION_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char *name;
    // 唯一设备ID，UID_BASE，共12个字节
    uint32_t guid[3];
    // 器件ID，DBGMCU_IDCODE，共4个字节
    uint32_t devid;
    uint32_t revid;
    // union {
    //     uint32_t dbgmcu_idcode;
    //     struct {
    //         uint32_t devid : 12;
    //         uint32_t _reserved : 4;
    //         uint32_t revid : 16;
    //     };
    // };
    // flash大小，FLASHSIZE_BASE
    uint32_t flash_size;
} mcu_info_t;

int  board_mcu_version(char *rev, const char **revstr, const char **errata);
int  board_get_mcu_info(mcu_info_t *);
void board_get_uuid32(uint32_t uid[3]);

#ifdef __cplusplus
}
#endif

#endif // __BOARD_MCU_VERSION_H__