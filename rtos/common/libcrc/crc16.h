/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#ifndef __INCLUDE_CRC16_H
#define __INCLUDE_CRC16_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <sys/types.h>
#include <stdint.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Name: crc16part
 *
 * Description:
 *   Continue CRC calculation on a part of the buffer.
 *
 ****************************************************************************/

uint16_t crc16part(const uint8_t *src, size_t len, uint16_t crc16val);

/****************************************************************************
 * Name: crc16
 *
 * Description:
 *   Return a 16-bit CRC of the contents of the 'src' buffer, length 'len'
 *
 ****************************************************************************/

uint16_t crc16calc(const uint8_t *src, size_t len);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_CRC16_H */
