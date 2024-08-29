/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "crc.h"

// According to https://en.wikipedia.org/wiki/Cyclic_redundancy_check
//
// the CRC is based on the CRC-32 ISO 3309 / ANSI X3.66
// implementetation, and the polynomial is supplied as "Reversed".
//
// The CRC algorithm was extracted from the Femtomes driver.
//

static constexpr uint32_t CRC32_POLYNOMIAL = 0xEDB88320;

static uint32_t crc32Value(uint32_t crc) {
    for (int i = 8; i > 0; i--) {
        if (crc & 1) {
            crc = (crc >> 1) ^ CRC32_POLYNOMIAL;

        } else {
            crc >>= 1;
        }
    }

    return crc;
}

uint32_t
calculateCRC32(uint32_t length, uint8_t *buffer, uint32_t crc) {
    while (length-- != 0) {
        crc = ((crc >> 8) & 0x00FFFFFFL) ^ (crc32Value(((uint32_t)crc ^ *buffer++) & 0xff));
    }

    return crc;
}
