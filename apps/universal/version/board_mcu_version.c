/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#include "board.h"
#include "board_mcu_version.h"

/**
 * @file board_mcu_version.c
 * Implementation of STM32 based SoC version API
 */

// https://blog.csdn.net/qq_42992084/article/details/115772881

/* magic numbers from reference manual */

enum MCU_REV {
    MCU_REV_STM32F4_REV_A = 0x1000,
    MCU_REV_STM32F4_REV_Z = 0x1001,
    MCU_REV_STM32F4_REV_Y = 0x1003,
    MCU_REV_STM32F4_REV_1 = 0x1007,
    MCU_REV_STM32F4_REV_3 = 0x2001,
    MCU_REV_STM32F4_REV_5 = 0x2003,
    MCU_REV_STM32F4_REV_B = MCU_REV_STM32F4_REV_5,
    MCU_REV_STM32H7_REV_X = MCU_REV_STM32F4_REV_3,
    MCU_REV_STM32H7_REV_V = 0x2003
};

/* Define any issues with the Silicon as lines separated by \n
 * omitting the last \n
 */
#define STM32_F4_ERRATA "This device can only utilize a maximum of 1MB flash safely!"

// STM DocID018909 Rev 8 Sect 38.18 and DocID026670 Rev 5 40.6.1 (MCU device ID code)
#define REVID_MASK 0xFFFF0000
#define DEVID_MASK 0xFFF

#define STM32H74xx_75xx   0x450
#define STM32F74xxx_75xxx 0x449
#define STM32F76xxx_77xxx 0x451
#define STM32F40x_41x     0x413
#define STM32F42x_43x     0x419
#define STM32F103_LD      0x412
#define STM32F103_MD      0x410
#define STM32F103_HD      0x414
#define STM32F103_XLD     0x430
#define STM32F103_CON     0x418

int board_mcu_version(char *rev, const char **revstr, const char **errata) {
#if defined(SOC_FAMILY_STM32)
    const char *chip_errata = NULL;

    // uint32_t abc = getreg32(DBGMCU_BASE);

    int32_t      chip_version = HAL_GetDEVID(); // abc & DEVID_MASK;
    enum MCU_REV revid        = HAL_GetREVID(); //    (abc & REVID_MASK) >> 16;

    switch (chip_version) {
    case STM32H74xx_75xx:
        *revstr = "STM32H7[4|5]xxx";
        break;

    case STM32F74xxx_75xxx:
        *revstr = "STM32F74xxx";
        break;

    case STM32F76xxx_77xxx:
        *revstr = "STM32F76xxx";
        break;

    case STM32F42x_43x:
        *revstr = "STM32F42x";
        /* Set possible errata */
        chip_errata = STM32_F4_ERRATA;
        break;

    case STM32F103_LD:
        *revstr = "STM32F1xx Low";
        break;

    case STM32F103_MD:
        *revstr = "STM32F1xx Med";
        break;

    case STM32F103_HD:
        *revstr = "STM32F1xx Hi";
        break;

    case STM32F103_XLD:
        *revstr = "STM32F1xx XL";
        break;

    case STM32F103_CON:
        *revstr = "STM32F1xx Con";
        break;

    default:
        *revstr = "STM32F???";
        break;
    }

    switch (revid) {
    case MCU_REV_STM32F4_REV_A:
        *rev = 'A';
        break;

    case MCU_REV_STM32F4_REV_Z:
        *rev = 'Z';
        break;

    case MCU_REV_STM32F4_REV_Y:
        *rev = 'Y';
        break;

    case MCU_REV_STM32F4_REV_1:
        *rev = '1';
        break;

    case MCU_REV_STM32F4_REV_3:
        *rev        = chip_version == STM32H74xx_75xx ? 'X' : '3';
        chip_errata = NULL;
        break;

    case MCU_REV_STM32F4_REV_5:
        // MCU_REV_STM32F4_REV_B shares the same REV_ID
        // MCU_REV_STM32H7_REV_V shares the same REV_ID
        *rev        = chip_version == STM32H74xx_75xx ? 'V' : '5';
        chip_errata = NULL;
        break;

    default:
        // todo add rev for 103 - if needed
        *rev  = '?';
        revid = -1;
        break;
    }

    if (errata) {
        *errata = chip_errata;
    }

    return revid;
#else
    return -1;
#endif
}

void board_get_uuid32(uint32_t uid[3]) {
#if defined(SOC_FAMILY_STM32)
    uid[0] = HAL_GetUIDw0();
    uid[1] = HAL_GetUIDw1();
    uid[2] = HAL_GetUIDw2();
#else
    uid[0] = 0;
    uid[1] = 0;
    uid[2] = 0;
#endif
}

int board_get_mcu_info(mcu_info_t *info) {
#if defined(SOC_FAMILY_STM32)
    info->name = BOARD_CHIP_NAME;
    // 获取uid
    // rt_memcpy(&info->guid[0], (void *)UID_BASE, 12);
    info->guid[0] = HAL_GetUIDw0();
    info->guid[1] = HAL_GetUIDw1();
    info->guid[2] = HAL_GetUIDw2();
    // 获取dbgmcu_idcode
    // rt_memcpy(&info->dbgmcu_idcode, (void *)DBGMCU_BASE, 4);
    info->devid = HAL_GetDEVID();
    info->revid = HAL_GetREVID();
    // 获取falsh size
    info->flash_size = *((uint32_t *)FLASHSIZE_BASE);
    // rt_memcpy(&info->flash_size, (void *)FLASHSIZE_BASE, 4);
    return 0;
#elif defined(BOARD_CHIP_NAME)
    info->name       = BOARD_CHIP_NAME;
    info->guid[0]    = 0;
    info->guid[1]    = 0;
    info->guid[2]    = 0;
    info->devid      = 0;
    info->revid      = 0;
    info->flash_size = 0;
    return 0;
#else
    return -1;
#endif // UID_BASE
}