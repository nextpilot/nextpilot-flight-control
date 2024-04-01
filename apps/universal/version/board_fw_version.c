/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#include <string.h>
#include "board_fw_version.h"

#if !defined(CONFIG_CDCACM_PRODUCTID)
#define CONFIG_CDCACM_PRODUCTID 0
#endif

#if defined(__LINUX)
#include <sys/utsname.h>
#endif

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
enum FIRMWARE_TYPE {
    FIRMWARE_TYPE_DEV     = 0,
    FIRMWARE_TYPE_ALPHA   = 64,
    FIRMWARE_TYPE_BETA    = 128,
    FIRMWARE_TYPE_RC      = 192,
    FIRMWARE_TYPE_RELEASE = 255
};

const char *get_build_uri(void) {
    return FW_BUILD_URI;
}

uint32_t version_tag_to_number(const char *tag) {
    uint32_t version_number = 0;

    int16_t buffer         = -1;
    size_t  buffer_counter = 0;
    size_t  dash_count     = 0;
    size_t  point_count    = 0;
    char    version[3]     = {0, 0, 0};
    int     firmware_type  = FIRMWARE_TYPE_RELEASE;

    for (size_t i = 0; i < strlen(tag); i++) {
        if (tag[i] == '-') {
            dash_count++;

        } else if (tag[i] == '.') {
            point_count++;
        }

        if (tag[i] == 'r' && i < strlen(tag) - 1 && tag[i + 1] == 'c') {
            firmware_type = FIRMWARE_TYPE_RC;

        } else if (tag[i] == 'p') {
            firmware_type = FIRMWARE_TYPE_ALPHA;

        } else if (tag[i] == 't' && i < strlen(tag) - 1 && tag[i + 1] == 'y') {
            firmware_type = FIRMWARE_TYPE_DEV;

        } else if (tag[i] == 't') {
            firmware_type = FIRMWARE_TYPE_BETA;

        } else if (tag[i] == 'v' && i > 0) {
            firmware_type = FIRMWARE_TYPE_DEV;
        }
    }

    if ((dash_count == 1 && point_count == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
        (dash_count == 2 && point_count == 2) ||
        (dash_count == 3 && point_count == 4) ||
        (dash_count == 4 && point_count == 4)) {
        firmware_type = FIRMWARE_TYPE_DEV;
    }

    for (size_t i = 0; i < strlen(tag); i++) {
        if (buffer_counter > 2) {
            continue;
        }

        if (tag[i] >= '0' && tag[i] <= '9') {
            buffer = (buffer == -1) ? 0 : buffer;
            buffer = buffer * 10 + (tag[i] - '0');

        } else {
            if (buffer >= 0) {
                version[buffer_counter] = buffer;
                buffer_counter++;
            }

            buffer = -1;
        }
    }

    if (buffer >= 0) {
        version[buffer_counter] = buffer;
        buffer_counter++;
    }

    if (buffer_counter <= 0) {
        firmware_type = 0x00;
    }

    if (buffer_counter == 3 || buffer_counter == 6) {
        version_number = ((uint8_t)version[0] << 8 * 3) |
                         ((uint8_t)version[1] << 8 * 2) |
                         ((uint8_t)version[2] << 8 * 1) | firmware_type;

    } else {
        version_number = 0;
    }

    return version_number;
}

uint32_t version_tag_to_vendor_version_number(const char *tag) {
    uint32_t version_number = 0;

    int16_t buffer         = -1;
    size_t  buffer_counter = 0;
    char    version[6]     = {0, 0, 0, 0, 0, 0};
    size_t  dash_count     = 0;
    size_t  point_count    = 0;
    int     firmware_type  = FIRMWARE_TYPE_RELEASE;

    for (size_t i = 0; i < strlen(tag); i++) {
        if (tag[i] == '-') {
            dash_count++;

        } else if (tag[i] == '.') {
            point_count++;
        }

        if (tag[i] == 'r' && i < strlen(tag) - 1 && tag[i + 1] == 'c') {
            firmware_type = FIRMWARE_TYPE_RC;

        } else if (tag[i] == 'p') {
            firmware_type = FIRMWARE_TYPE_ALPHA;

        } else if (tag[i] == 't' && i < strlen(tag) - 1 && tag[i + 1] == 'y') {
            firmware_type = FIRMWARE_TYPE_DEV;

        } else if (tag[i] == 't') {
            firmware_type = FIRMWARE_TYPE_BETA;

        } else if (tag[i] == 'v' && i > 0) {
            firmware_type = FIRMWARE_TYPE_DEV;
        }
    }

    if ((dash_count == 1 && point_count == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
        (dash_count == 2 && point_count == 2) ||
        (dash_count == 3 && point_count == 4) ||
        (dash_count == 4 && point_count == 4)) {
        firmware_type = FIRMWARE_TYPE_DEV;
    }

    for (size_t i = 0; i < strlen(tag); i++) {
        if (buffer_counter > 5) {
            continue;
        }

        if (tag[i] >= '0' && tag[i] <= '9') {
            buffer = (buffer == -1) ? 0 : buffer;
            buffer = buffer * 10 + (tag[i] - '0');

        } else {
            if (buffer >= 0) {
                if (buffer_counter + 1 == 4 && tag[i] == '-') {
                    break;
                }

                version[buffer_counter] = buffer;
                buffer_counter++;
            }

            buffer = -1;
        }
    }

    if (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
        version[buffer_counter] = buffer;
        buffer_counter++;
    }

    if (buffer_counter == 6) {
        version_number = ((uint8_t)version[3] << 8 * 3) |
                         ((uint8_t)version[4] << 8 * 2) |
                         ((uint8_t)version[5] << 8 * 1) | firmware_type;

    } else if (buffer_counter == 3) {
        version_number = firmware_type;

    } else {
        version_number = 0;
    }

    return version_number;
}

uint32_t get_firmware_version(void) {
    return version_tag_to_number(FW_GIT_TAG_STR);
}

const char *get_firmware_version_string(void) {
    return FW_GIT_HASH_STR;
}

uint64_t get_firmware_version_binary(void) {
    return FW_GIT_HASH_HEX;
}

const char *get_firmware_git_branch(void) {
    return FW_GIT_BRANCH_STR;
}

const char *get_firmware_oem_version_string(void) {
    return FW_GIT_OEM_TAG_STR;
}

uint32_t get_firmware_vendor_version(void) {
    return version_tag_to_vendor_version_number(FW_GIT_TAG_STR);
}

const char *get_os_name(void) {
#if defined(__RTTHREAD__)
    return "RT-Thread";
#elif defined(__LINUX)
    return "Linux";
#elif defined(__CYGWIN)
    return "Cygwin";
#else
    return "unkown";
#endif
}

uint32_t get_os_version(void) {
#if defined(__LINUX)
    struct utsname name;

    if (uname(&name) == 0) {
        char *c = name.release;

        // cut the part after the first '-'
        while (*c && *c != '-') {
            ++c;
        }

        *c = 0;
        return version_tag_to_number(name.release);

    } else {
        return 0;
    }
#elif defined(__RTTHREAD__)
    return RT_VERSION << 24 | RT_SUBVERSION << 16 | RT_REVISION << 8 | FIRMWARE_TYPE_RELEASE;
    // return version_tag_to_number(OS_GIT_TAG_STR);
#else
#error "get_os_version not implemented for current OS"
#endif
}

const char *get_os_version_string(void) {
    return OS_GIT_HASH_STR;
}

uint64_t get_os_version_binary(void) {
#ifdef OS_GIT_HASH_HEX
    return OS_GIT_HASH_HEX;
#else
    return 0;
#endif
}

const char *get_toolchain_name(void) {
#if defined(__clang__)
    return "Clang/LLVM";
#elif defined(__ICC) || defined(__INTEL_COMPILER)
    return "Intel ICC";
#elif defined(__GNUC__) || defined(__GNUG__)
    return "GNU GCC";
#elif defined(__ARMCC_VERSION)
    return "KEIL";
#elif defined(_MSC_VER)
    return "MS Visual Studio";
#else
    return "Unknown";
#endif
}

const char *get_toolchain_version(void) {
#if defined(__VERSION__)
    return __VERSION__;
#else
    return "Unknown";
#endif
}

const char *get_ecl_lib_version_string(void) {
#ifdef ECL_LIB_GIT_VERSION_STRING
    return ECL_LIB_GIT_VERSION_STRING;
#else
    return (char *)NULL;
#endif
}

uint64_t get_mavlink_lib_version_binary(void) {
#ifdef MAVLINK_LIB_GIT_VERSION_BINARY
    return MAVLINK_LIB_GIT_VERSION_BINARY;
#else
    return 0;
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */
}
