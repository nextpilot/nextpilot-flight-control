/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#ifndef __INCLUDE_CRC64_H
#define __INCLUDE_CRC64_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <sys/types.h>
#include <stdint.h>

#ifdef CONFIG_HAVE_LONG_LONG

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/*
 * CRC64_CHECK is the CRC64 of the string "123456789" without the null byte.
 *
 *   const uint8_t checkbuf[] =
 *   {
 *     '1', '2', '3', '4', '5', '6', '7', '8', '9'
 *   };
 *
 *   assert(crc64calc(checkbuf, sizeof(checkbuf)) == CRC64_CHECK);
 */

/* CRC-64/WE */

#define CRC64_POLY   ((uint64_t)0x42f0e1eba9ea3693ull)
#define CRC64_INIT   ((uint64_t)0xffffffffffffffffull)
#define CRC64_XOROUT ((uint64_t)0xffffffffffffffffull)
#define CRC64_CHECK  ((uint64_t)0x62ec59e3f1a4f00aull)

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
 * Name: crc64part
 *
 * Description:
 *   Continue CRC calculation on a part of the buffer.
 *
 ****************************************************************************/

uint64_t crc64part(const uint8_t *src, size_t len, uint64_t crc64val);

/****************************************************************************
 * Name: crc64
 *
 * Description:
 *   Return a 64-bit CRC of the contents of the 'src' buffer, length 'len'.
 *
 ****************************************************************************/

uint64_t crc64calc(const uint8_t *src, size_t len);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* CONFIG_HAVE_LONG_LONG */
#endif /* __INCLUDE_CRC64_H */
