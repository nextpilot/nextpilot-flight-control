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
 * @file board_identity.c
 * Implementation of Non Arch specific Board identity API
 */

#include <stdio.h>
#include <string.h>

/**
 * For special cases, specific boards may need to override the UUID, instead of using the generic
 * PX4 GUID (gettable via 'board_get_px4_guid_formated' function). In that case we define the cascaded
 * UUID function getters to incorporate the overridden UUID into the GUID.
 */
#if defined(BOARD_OVERRIDE_UUID) || defined(BOARD_OVERRIDE_MFGUID) || defined(BOARD_OVERRIDE_PX4_GUID)
static const uint16_t soc_arch_id    = PX4_SOC_ARCH_ID;
static const char     board_uuid[17] = BOARD_OVERRIDE_UUID;

void board_get_uuid32(uuid_uint32_t uuid_words) {
    unsigned int len = strlen(board_uuid);

    if (len > PX4_CPU_UUID_BYTE_LENGTH) {
        len = PX4_CPU_UUID_BYTE_LENGTH;
    }

    uint8_t *bp = (uint8_t *)uuid_words;

    for (unsigned int i = 0; i < len; i++) {
        *bp++ = board_uuid[i];
    }

    for (unsigned int i = len; i < PX4_CPU_UUID_BYTE_LENGTH; i++) {
        *bp++ = '0';
    }
}

int board_get_uuid32_formated(char *format_buffer, int size,
                              const char *format,
                              const char *seperator) {
    uuid_uint32_t uuid;
    board_get_uuid32(uuid);
    int offset   = 0;
    int sep_size = seperator ? strlen(seperator) : 0;

    for (unsigned i = 0; (offset < size - 1) && (i < PX4_CPU_UUID_WORD32_LENGTH); i++) {
        offset += snprintf(&format_buffer[offset], size - offset, format, uuid[i]);

        if (sep_size && (offset < size - sep_size - 1) && (i < PX4_CPU_UUID_WORD32_LENGTH - 1)) {
            strncat(&format_buffer[offset], seperator, size - offset);
            offset += sep_size;
        }
    }

    return 0;
}

int board_get_mfguid_formated(char *format_buffer, int size) {
    board_get_uuid32_formated(format_buffer, size, "%02x", NULL);
    return strlen(format_buffer);
}

int board_get_px4_guid_formated(char *format_buffer, int size) {
    int offset  = snprintf(format_buffer, size, "%04x", soc_arch_id);
    size       -= offset;
    return board_get_mfguid_formated(&format_buffer[offset], size);
}
#endif
