/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#ifndef __INCLUDE_CRC8_H
#define __INCLUDE_CRC8_H

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
 * Name: crc8part
 *
 * Description:
 *   Continue CRC calculation on a part of the buffer using the polynomial
 *   x^8+x^6+x^3+x^2+1 (Koopman, et al. "0xA6" poly).
 *
 ****************************************************************************/

uint8_t crc8part(const uint8_t *src, size_t len, uint8_t crc8val);

/****************************************************************************
 * Name: crc8
 *
 * Description:
 *   Return an 8-bit CRC of the contents of the 'src' buffer, length 'len'
 *   using the polynomial x^8+x^6+x^3+x^2+1 (Koopman, et al. "0xA6" poly).
 *
 ****************************************************************************/

uint8_t crc8calc(const uint8_t *src, size_t len);

/****************************************************************************
 * Name: crc8ccitt
 *
 * Description:
 *   Return an 8-bit CRC of the contents of the 'src' buffer, length 'len'
 *   using the polynomial x^8+x^2+x^1+1 (aka "0x07" poly).
 *
 ****************************************************************************/

uint8_t crc8ccitt(const uint8_t *src, size_t len);

/****************************************************************************
 * Name: crc8part
 *
 * Description:
 *   Continue CRC calculation on a part of the buffer using the polynomial
 *   x^8+x^2+x^1+1 (aka "0x07" poly).
 *
 ****************************************************************************/

uint8_t crc8ccittpart(const uint8_t *src, size_t len, uint8_t crc8val);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_CRC8_H */
