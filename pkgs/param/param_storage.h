/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __PARAM_STORAGE_H__
#define __PARAM_STORAGE_H__

#include <rtthread.h>
#include "param_common.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct {
    uint32_t check;
    char     magic[4]; // "zhan"
    uint64_t utc;      // 保存时间戳
    uint16_t count;    // 参数个数
} param_header_t;

typedef struct {
    char          name[16];
    param_type_t  type;
    param_value_t value;
} param_payload_t;

typedef struct param_storage_s param_storage_t;

typedef struct {
    int (*init)();
    int (*open)(param_storage_t *dev, const char *name, uint8_t flag);
    int (*read)(param_storage_t *dev, int32_t offset, void *buff, uint32_t size);
    int (*write)(param_storage_t *dev, int32_t offset, void *buff, uint32_t size);
    int (*close)(param_storage_t *dev);
} param_storage_ops_t;

struct param_storage_s {
    const char          *name;
    uint8_t              type;
    param_storage_ops_t *ops;
    union {
        int         fid;
        rt_device_t dev;
    };
};

int param_storage_register(param_storage_t *dev);

/**
 * Import parameters from a file, discarding any unrecognized parameters.
 *
 * This function merges the imported parameters with the current parameter set.
 *
 * @param fd		File descriptor to import from (-1 selects the FLASH storage).
 * @return		Zero on success, nonzero if an error occurred during import.
 *			Note that in the failure case, parameters may be inconsistent.
 */
int param_export_internal(const char *devname, param_filter_func filter);

/**
 * Export changed parameters to a file.
 * Note: this method requires a large amount of stack size!
 *
 * @param filename	Path to the default parameter file.
 * @param filter	Filter parameters to be exported. The method should return true if
 * 			the parameter should be exported. No filtering if nullptr is passed.
 * @return		Zero on success, nonzero on failure.
 */
int param_import_internal(const char *devname, param_filter_func filter);

/**
 * Load parameters from a file.
 *
 * This function resets all parameters to their default values, then loads new
 * values from a file.
 *
 * @param fd		File descriptor to import from (-1 selects the FLASH storage).
 * @return		Zero on success, nonzero if an error occurred during import.
 *			Note that in the failure case, parameters may be inconsistent.
 */
int param_reset_and_import(const char *devname);

/**
 * Load parameters from the default parameter file.
 *
 * @return		Zero on success.
 */
int param_load_default();

/**
 * Save parameters to the default file.
 * Note: this method requires a large amount of stack size!
 *
 * This function saves all parameters with non-default values.
 *
 * @return		Zero on success.
 */
int param_save_default();

/**
 * Enable/disable the param autosaving.
 * Re-enabling with changed params will not cause an autosave.
 * @param enable true: enable autosaving, false: disable autosaving
 */
void param_control_autosave(bool enable);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __PARAM_STORAGE_H__
