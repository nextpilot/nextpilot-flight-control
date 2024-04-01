/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#ifndef __BOARD_FW_VERSION_H__
#define __BOARD_FW_VERSION_H__

#include <rtthread.h>
#include <stdint.h>
#include "board_fw_version_autogen.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Convert a version tag string to a number
 * @param tag version tag in one of the following forms:
 *            - vendor: v1.4.0-0.2.0
 *            - dev: v1.4.0-rc3-7-g7e282f57
 *            - rc: v1.4.0-rc4
 *            - beta: v1.4.0-beta1
 *            - release: v1.4.0
 *            - linux: 7.9.3
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
uint32_t version_tag_to_number(const char *tag);

/**
 * Convert a version tag string to a vendor version number
 * @param tag version tag in one of the following forms:
 *            - vendor: v1.4.0-0.2.0
 *            - dev: v1.4.0-rc3-7-g7e282f57
 *            - rc: v1.4.0-rc4
 *            - beta: v1.4.0-beta1
 *            - release: v1.4.0
 *            - linux: 7.9.3
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
uint32_t version_tag_to_vendor_version_number(const char *tag);

/**
 * get the Firmware version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
uint32_t get_firmware_version(void);

/**
 * get the Firmware vendor version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
uint32_t get_firmware_vendor_version(void);

/**
 * Firmware version as human readable string (git tag)
 */
const char *get_firmware_version_string(void);

/**
 * Firmware version in binary form (first part of the git tag)
 */
uint64_t get_firmware_version_binary(void);

/**
 * get the git branch name (can be empty, for example if HEAD points to a tag)
 */
const char *get_firmware_git_branch(void);

/**
 * get the git oem version tag (can be empty, no particular format)
 */
const char *get_firmware_oem_version_string(void);

/**
 * name of the operating system
 * @return human readable string
 */
const char *get_os_name(void);

/**
 * operating system version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
uint32_t get_os_version(void);

/**
 * Operating system version as human readable string (git tag)
 * @return string or NULL if not defined
 */
const char *get_os_version_string(void);

/**
 * Operating system version in binary form (first part of the git tag)
 * @return this is not available on all OSes and can return 0
 */
uint64_t get_os_version_binary(void);

/**
 * Toolchain name used to compile
 */
const char *get_toolchain_name(void);

/**
 * Toolchain version used to compile (no particular format)
 */
const char *get_toolchain_version(void);

/**
 * ECL lib version as human readable string (git tag)
 */
const char *get_ecl_lib_version_string(void);

/**
 * MAVLink lib version in binary form (first part of the git tag)
 */
uint64_t get_mavlink_lib_version_binary(void);

/**
 * get the build URI (used for crash logging)
 */
const char *get_build_uri(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __BOARD_FW_VERSION_H__