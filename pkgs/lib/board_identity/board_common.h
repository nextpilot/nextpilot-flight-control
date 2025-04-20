/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file board_common.h
 *
 * Provide the the common board interfaces
 */

#pragma once

/************************************************************************************
 * Included Files
 ************************************************************************************/
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/************************************************************************************
 * Definitions
 ************************************************************************************/

/************************************************************************************
 * Public Data
 ************************************************************************************/

/* board power button state notification */
typedef enum board_power_button_state_notification_e {
    PWR_BUTTON_IDEL,                       /* Button went up without meeting shutdown button down time */
    PWR_BUTTON_DOWN,                       /* Button went Down */
    PWR_BUTTON_UP,                         /* Button went Up */
    PWR_BUTTON_REQUEST_SHUT_DOWN,          /* Button went up after meeting shutdown button down time */

    PWR_BUTTON_RESPONSE_SHUT_DOWN_PENDING, /* Response from call back board code does nothing the
                                            * expectation is that board_shutdown will be called.
                                            */
    PWR_BUTTON_RESPONSE_SHUT_DOWN_NOW,     /* Response from call back board code does shutdown now. */
} board_power_button_state_notification_e;

/* board call back signature  */
typedef int (*power_button_state_notification_t)(board_power_button_state_notification_e request);

/*  PX4_SOC_ARCH_ID is monotonic ordinal number assigned by PX4 to a chip
 *  architecture. The 2 bytes are used to create a globally unique ID when
 *  prepended to a padded Soc ID.
 */

typedef enum PX4_SOC_ARCH_ID_t {
    PX4_SOC_ARCH_ID_UNUSED = 0x0000,

    PX4_SOC_ARCH_ID_STM32F4      = 0x0001,
    PX4_SOC_ARCH_ID_STM32F7      = 0x0002,
    PX4_SOC_ARCH_ID_KINETISK66   = 0x0003,
    PX4_SOC_ARCH_ID_SAMV7        = 0x0004,
    PX4_SOC_ARCH_ID_NXPIMXRT1062 = 0x0005,

    PX4_SOC_ARCH_ID_STM32H7 = 0x0006,

    PX4_SOC_ARCH_ID_NXPS32K146 = 0x0007,
    PX4_SOC_ARCH_ID_NXPS32K344 = 0x0008,

    PX4_SOC_ARCH_ID_EAGLE = 0x1001,
    PX4_SOC_ARCH_ID_QURT  = 0x1002,

    PX4_SOC_ARCH_ID_RPI  = 0x1004,
    PX4_SOC_ARCH_ID_SIM  = 0x1005,
    PX4_SOC_ARCH_ID_SITL = 0x1006,

    PX4_SOC_ARCH_ID_BBBLUE = 0x1008,

    PX4_SOC_ARCH_ID_VOXL2 = 0x100A,

} PX4_SOC_ARCH_ID_t;

/* UUID
 *
 * Define the types used for board UUID, MFG UID and PX4 GUID
 *
 * A type suitable for holding the byte format of the UUID
 *
 * The original PX4 stm32 (legacy) based implementation **displayed** the
 * UUID as: ABCD EFGH IJKL
 * Where:
 *       A was bit 31 and D was bit 0
 *       E was bit 63 and H was bit 32
 *       I was bit 95 and L was bit 64
 *
 * Since the string was used by some manufactures to identify the units
 * it must be preserved. DEPRECATED - This will be removed in PX4 Release
 * 1.7.0.
 *
 * For new targets moving forward we will use an 18 byte globally unique
 * PX4 GUID in the form:
 *
 *           <ARCH MSD><ARCH LSD>[MNOP] IJKL EFGH ABCD
 *
 *  Where <ARCH MSD><ARCH LSD> are a monotonic ordinal number assigned by
 *  PX4 to a chip architecture (PX4_SOC_ARCH_ID). The 2 bytes are used to
 *  create a globally unique ID when prepended to a padded CPU UUID.
 *
 *  In the case where the MFG UUID is shorter than 16 bytes it will be
 *  padded with 0's starting at offset [2] until
 *  PX4_GUID_BYTE_LENGTH-PX4_CPU_UUID_BYTE_LENGTH -1
 *
 *  I.E. For the STM32
 *  offset:0         1     2  3  4  5  6             -            17
 *    <ARCH MSD><ARCH LSD>[0][0][0][0]<MSD CPU UUID>...<LSD CPU UUID>
 *
 *  I.E. For the Kinetis
 *  offset:0         1         2         -           17
 *    <ARCH MSD><ARCH LSD><MSD CPU UUID>...<LSD CPU UUID>
 */

/* Define the PX4 Globally unique ID (GUID) length and format size */
#define PX4_GUID_BYTE_LENGTH 18
#define PX4_GUID_FORMAT_SIZE ((2 * PX4_GUID_BYTE_LENGTH) + 1)

/* DEPRICATED as of 1.7.0 A type suitable for defining the 8 bit format of the CPU UUID */
typedef uint8_t uuid_byte_t[PX4_CPU_UUID_BYTE_LENGTH];

/* DEPRICATED as of 1.7.0  A type suitable for defining the 32bit format of the CPU UUID */
typedef uint32_t uuid_uint32_t[PX4_CPU_UUID_WORD32_LENGTH];

/* A type suitable for defining the 8 bit format of the MFG UID
 * This is always returned as MSD @ index 0 -LSD @ index PX4_CPU_MFGUID_BYTE_LENGTH-1
 */
typedef uint8_t mfguid_t[PX4_CPU_MFGUID_BYTE_LENGTH];

/* A type suitable for defining the 8 bit format of the px4 globally unique
 * PX4 GUID. This is always returned as MSD @ index 0 -LSD @ index
 * PX4_CPU_GUID_BYTE_LENGTH-1
 */
typedef uint8_t px4_guid_t[PX4_GUID_BYTE_LENGTH];

/************************************************************************************
 * Public Functions
 ************************************************************************************/
__BEGIN_DECLS

__EXPORT void board_get_uuid(uuid_byte_t uuid_bytes); // DEPRICATED use board_get_px4_guid

/************************************************************************************
 * Name: board_get_uuid32 DEPRICATED use board_get_px4_guid
 *
 * Description:
 *   All boards either provide a way to read a uuid of PX4_CPU_UUID_WORD32_LENGTH
 *   from PX4_CPU_UUID_ADDRESS in the Soc's address space OR define
 *   BOARD_OVERRIDE_UUID as an array of bytes that is PX4_CPU_UUID_BYTE_LENGTH
 *   On Legacy (stm32) targets the uuid_words format is the result of coping
 *   returning the 32bit words from low memory to high memory. On new targets the result
 *   will be an array of words with the MSW at index 0 and the LSW: at index
 *   PX4_CPU_UUID_WORD32_LENGTH-1.
 *
 * Input Parameters:
 *   uuid_words - a uuid_uint32_t and array of 32 bit words PX4_CPU_UUID_WORD32_
 *   LENGTH in length.
 *
 * Returned Value:
 *   The uuid_words array is populated with the CPU uuid.
 *
 ************************************************************************************/
__EXPORT void board_get_uuid32(uuid_uint32_t uuid_words); // DEPRICATED use board_get_px4_guid

/************************************************************************************
 * Name: board_get_uuid32_formated DEPRICATED use board_get_px4_guid_formated
 *
 * Description:
 *   All boards either provide a way to retrieve a uuid and format it
 *   or define BOARD_OVERRIDE_UUID
 *   This function is used to populate a buffer with the UUID to be a printed
 *   with the optional separator
 *
 * Input Parameters:
 *   format_buffer - A pointer to a bufferer of at least PX4_CPU_UUID_WORD32_FORMAT_SIZE
 *                   that will contain a 0 terminated string formatted as described
 *                   the format string and optional separator.
 *   size          - The size of the buffer (should be atleaset PX4_CPU_UUID_WORD32_FORMAT_SIZE)
 *   format        - The fort mat specifier for the hex digit see CPU_UUID_FORMAT
 *   separator     - Optional pointer to a 0 terminated string or NULL:
 *                   With separator = ":"
 *                               31-00:63-32:95-64
 *                               32383336:412038:33355110
 *                   With separator = " "
 *                               31-00:63-32:95-64
 *                               32383336 412038 33355110
 *                   With separator = NULL
 *                               31-00:63-32:95-64
 *                               3238333641203833355110
 *
 * Returned Value:
 *   The format buffer is populated with a 0 terminated string formatted as described.
 *   Zero (OK) is returned on success;
 *
 ************************************************************************************/
__EXPORT int board_get_uuid32_formated(char *format_buffer, int size,
                                       const char *format,
                                       const char *seperator); // DEPRICATED use board_get_px4_guid_formated

/************************************************************************************
 * Name: board_get_mfguid
 *
 * Description:
 *   All boards either provide a way to retrieve a manufactures Unique ID or
 *   define BOARD_OVERRIDE_UUID.
 *    The MFGUID is returned as an array of bytes in
 *    MSD @ index 0 - LSD @ index PX4_CPU_MFGUID_BYTE_LENGTH-1
 *
 * Input Parameters:
 *   mfgid - mfguid_t and array of bytes PX4_CPU_MFGUID_BYTE_LENGTH in length.
 *
 * Returned Value:
 *   The mfguid_t array is populated with the CPU uuid with the MSD @ index 0
 *   and the LSD @ index PX4_CPU_MFGUID_BYTE_LENGTH-1.
 *
 ************************************************************************************/

int board_get_mfguid(mfguid_t mfgid);

/************************************************************************************
 * Name: board_get_mfguid_formated DEPRICATED use board_get_px4_guid_formated
 *
 * Description:
 *   All boards either provide a way to retrieve a formatted string of the
 *   manufactures unique ID or define BOARD_OVERRIDE_MFGUID
 *
 * Input Parameters:
 *   format_buffer - A pointer to a bufferer of at least PX4_CPU_MFGUID_FORMAT_SIZE
 *                   that will contain a 0 terminated string formatted as 0 prefixed
 *                   lowercase hex. 2 charaters per digit of the mfguid_t.
 *
 * Returned Value:
 *   format_buffer is populated with a 0 terminated string of hex digits. The
 *   return value is the number of printable in the string.
 *   Usually PX4_CPU_MFGUID_FORMAT_SIZE-1
 *
 ************************************************************************************/

int board_get_mfguid_formated(char *format_buffer, int size); // DEPRICATED use board_get_px4_guid_formated

/************************************************************************************
 * Name: board_get_px4_guid
 *
 * Description:
 *   All boards either provide a way to retrieve a PX4 Globally unique ID or
 *   define BOARD_OVERRIDE_UUID.
 *
 *   The form of the GUID is as follows:
 *  offset:0         1         2         -           17
 *    <ARCH MSD><ARCH LSD><MSD CPU UUID>...<LSD CPU UUID>
 *
 *  Where <ARCH MSD><ARCH LSD> are a monotonic ordinal number assigned by
 *  PX4 to a chip architecture (PX4_SOC_ARCH_ID). The 2 bytes are used to
 *  create a globally unique ID when prepended to a padded CPU ID.
 *
 *  In the case where the CPU's UUID is shorter than 16 bytes it will be
 *  padded with 0's starting at offset [2] until
 *  PX4_CPU_MFGUID_BYTE_LENGTH-PX4_CPU_UUID_BYTE_LENGTH -1
 *  I.E. For the STM32
 *  offset:0         1     2  3  4  5  6             -            17
 *    <ARCH MSD><ARCH LSD>[0][0][0][0]<MSD CPU UUID>...<LSD CPU UUID>
 *
 *  I.E. For as CPU with a 16 byte UUID
 *  offset:0         1         2         -           17
 *    <ARCH MSD><ARCH LSD><MSD CPU UUID>...<LSD CPU UUID>
 *
 * Input Parameters:
 *   guid - a px4_guid_t which is byte array of PX4_GUID_BYTE_LENGTH length.
 *
 * Returned Value:
 *   guid is populated as  <ARCH MSD><ARCH LSD><MSD CPU UUID>...<LSD CPU UUID>
 *   the return value is PX4_GUID_BYTE_LENGTH
 *
 ************************************************************************************/

int board_get_px4_guid(px4_guid_t guid);

/************************************************************************************
 * Name: board_get_px4_guid_formated
 *
 * Description:
 *   All boards either provide a way to retrieve a formatted string of the
 *   manufactures Unique ID or define BOARD_OVERRIDE_PX4_GUID
 *
 * Input Parameters:
 * format_buffer - A buffer to receive the 0 terminated formatted px4
 *                 guid string.
 * size          - Size of the buffer provided. Normally this would
 *                 be PX4_GUID_FORMAT_SIZE.
 *                 If the size is less than PX4_GUID_FORMAT_SIZE the string
 *                 returned will be truncated from the MSD end and even
 *                 in length.
 *
 * Returned Value:
 *   The number of printable characters. This value will be even and one less the
 *   the size passed in.
 *
 ************************************************************************************/

int board_get_px4_guid_formated(char *format_buffer, int size);

__END_DECLS
