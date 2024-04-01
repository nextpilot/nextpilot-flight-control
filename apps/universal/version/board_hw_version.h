/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#ifndef __BOARD_HW_VERSION_H__
#define __BOARD_HW_VERSION_H__

#include <rtconfig.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BOARD_TYPE_NAME
#define BOARD_TYPE_NAME "unkown"
#endif // BOARD_TYPE_NAME

#ifndef BOARD_CHIP_NAME
#define BOARD_CHIP_NAME "unkown"
#endif // BOARD_CHIP_NAME

const char *board_get_hw_type_name();
int         board_get_hw_version();
int         board_get_hw_revision();

/**
 * get the board name as string (including the version if there are multiple)
 */
static inline const char *get_board_name(void) {
    return BOARD_TYPE_NAME;
}

/**
 * get the board sub type
 */
static inline const char *get_board_sub_type(void) {
    return board_get_hw_type_name();
}

/**
 * get the board HW version
 */
static inline int get_board_hw_version(void) {
    return board_get_hw_version();
}

/**
 * get the board HW revision
 */
static inline int get_board_hw_revision(void) {
    return board_get_hw_revision();
}

/**
 * get the board version (last 8 bytes should be silicon ID, if any)
 */
static inline uint32_t get_board_version(void) {
#if defined(CONFIG_CDCACM_PRODUCTID)
    return CONFIG_CDCACM_PRODUCTID;
#else
    return ((get_board_hw_version() & 0xff) << 8) | (get_board_hw_revision() & 0xff);
#endif
}

#ifdef __cplusplus
}
#endif

#endif // __BOARD_HW_VERSION_H__