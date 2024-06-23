/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __PARAM_H__
#define __PARAM_H__

#include <rtthread.h>
#include <rtdbg.h>
#include <stdlib.h>
#include "param_type.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * Notify the system about parameter changes. Can be used for example after several calls to
 * param_set_no_notification() to avoid unnecessary system notifications.
 */
void param_notify_changes();

/**
 * Return the total number of parameters.
 *
 * @return		The number of parameters.
 */
uint16_t param_get_count();

/**
 * Return the actually used number of parameters.
 *
 * @return		The number of parameters.
 */
uint16_t param_get_count_used();

/**
 * Check the param is valid.
 *
 * @return		True if valid.
 */
bool param_in_range(param_t idx);

static inline param_t param_for_index(int index) {
    return (param_t)index;
}

static inline param_t param_get_used_index(param_t index) {
    return index;
}

static inline param_t param_for_used_index(param_t index) {
    return index;
}

///////////////////////////////////////////////////
// 获取meta信息
///////////////////////////////////////////////////

int param_get_info(param_t idx, param_info_t *info);

/**
 * Obtain the name of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		The name assigned to the parameter, or NULL if the handle is invalid.
 */
const char *param_get_name(param_t idx);

/**
 * Obtain the type of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		The type assigned to the parameter.
 */
param_type_t param_get_type(param_t idx);

const char *param_get_type_cstr(param_t idx);

/**
 * Determine the size of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		The size of the parameter's value.
 */
uint8_t param_get_type_size(param_t idx);

/**
 * Copy the (airframe-specific) default value of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param default_val		Where to return the value, assumed to point to suitable storage for the parameter type.
 * @return		Zero if the parameter's deafult value could be returned, nonzero otherwise.
 */
int param_get_default_value(param_t idx, param_value_t *val);

param_flag_t param_get_flag(param_t idx);

bool param_is_volatile(param_t idx);

bool param_reboot_required(param_t idx);

bool param_disarm_required(param_t idx);


////////////////////////////////////////////////////////////////////////////
// 查询/设置状态（get/set status）
////////////////////////////////////////////////////////////////////////////

param_status_t param_get_status(param_t idx);

bool param_value_used(param_t idx);

bool param_value_unsaved(param_t idx);

bool param_value_changed(param_t idx);

void param_mark_changed(param_t idx);

void param_mark_used(param_t idx);

#define param_set_used param_mark_used


///////////////////////////////////////////////////
// 查找参数
///////////////////////////////////////////////////

int param_find_internal(const char *name, bool mark_used);

/**
 * Look up a parameter by name.
 *
 * @param name		The canonical name of the parameter being looked up.
 * @return		A handle to the parameter, or PARAM_INVALID if the parameter does not exist.
 *			This call will also set the parameter as "used" in the system, which is used
 *			to e.g. show the parameter via the RC interface
 */
param_t param_find(const char *name);

/**
 * Look up a parameter by name.
 *
 * @param name		The canonical name of the parameter being looked up.
 * @return		A handle to the parameter, or PARAM_INVALID if the parameter does not exist.
 */
param_t param_find_no_notification(const char *name);


///////////////////////////////////////////////////
// 获取参数
///////////////////////////////////////////////////

int param_get_internal(param_t idx, param_value_t *val, bool mark_used);

/**
 * Copy the value of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param val		Where to return the value, assumed to point to suitable storage for the parameter type.
 * @return		Zero if the parameter's value could be returned, nonzero otherwise.
 */
int param_get(param_t idx, void *val);

float param_get_float(param_t idx);

int32_t param_get_int32(param_t idx);

///////////////////////////////////////////////////
// 设置参数
///////////////////////////////////////////////////

int param_set_internal(param_t idx, const param_value_t *val, bool mark_saved, bool notify_changes);

/**
 * Set the value of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param val		The value to set; assumed to point to a variable of the parameter type.
 * @return		Zero if the parameter's value could be set from a scalar, nonzero otherwise.
 */
int param_set(param_t idx, const void *val);

/**
 * Set the value of a parameter, but do not notify the system about the change.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param val		The value to set; assumed to point to a variable of the parameter type.
 * @return		Zero if the parameter's value could be set from a scalar, nonzero otherwise.
 */
int param_set_no_notification(param_t idx, const void *val);

static inline int param_set_float(param_t idx, float val) {
    return param_set(idx, &val);
}

static inline int param_set_int32(param_t idx, int32_t val) {
    return param_set(idx, &val);
}

///////////////////////////////////////////////////
// 重置参数
///////////////////////////////////////////////////

int param_reset_internal(param_t idx, bool notify);

/**
 * Reset a parameter to its default value.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		Zero on success, nonzero on failure
 */
int param_reset(param_t idx);

/**
 * Reset a parameter to its default value, but do not notify the system about the change.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		Zero on success, nonzero on failure
 */
int param_reset_no_notification(param_t idx);

void param_reset_all_internal(bool notify);

/**
 * Reset all parameters to their default values.
 */
void param_reset_all();

void param_reset_all_no_notification();

/**
 * Reset all parameters to their default values except for excluded parameters.
 *
 * @param excludes			Array of param names to exclude from resetting. Use a wildcard
 *							at the end to exclude parameters with a certain prefix.
 * @param num_excludes		The number of excludes provided.
 */
void param_reset_excludes(const char *excludes[], int num_excludes);

/**
 * Reset only specific parameters to their default values.
 *
 * @param resets Array of param names to reset. Use a wildcard at the end to reset parameters with a certain prefix.
 * @param num_resets The number of passed reset conditions in the resets array.
 */
void param_reset_specific(const char *resets[], int num_resets);

/**
 * Apply a function to each parameter.
 *
 * Note that the parameter set is not locked during the traversal. It also does
 * not hold an internal state, so the callback function can block or sleep between
 * parameter callbacks.
 *
 * @param func		The function to invoke for each parameter.
 * @param arg		Argument passed to the function.
 * @param only_changed	If true, the function is only called for parameters whose values have
 *			been changed from the default.
 * @param only_used	If true, the function is only called for parameters which have been
 *			used in one of the running applications.
 */
void param_foreach(void (*func)(void *arg, param_t param), void *arg, bool only_changed, bool only_used);

// 计算hash值
/**
 * Generate the hash of all parameters and their values
 *
 * @return		CRC32 hash of all param_ids and values
 */
uint32_t param_hash_check(void);

/////////////////////////////////////////////////////////////
// 参数导入/导出
/////////////////////////////////////////////////////////////

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


void param_notify_autosave();


/**
 * Enable/disable the param autosaving.
 * Re-enabling with changed params will not cause an autosave.
 * @param enable true: enable autosaving, false: disable autosaving
 */
void param_control_autosave(bool enable);

#ifdef __cplusplus
}
#endif //__cplusplus

#ifdef __cplusplus
static inline int get_parameter_value(const char *option, int &value) {
    value = 0;

    /* check if this is a param name */
    if (rt_strncmp("p:", option, 2) == 0) {
        const char *param_name = option + 2;

        /* user wants to use a param name */
        param_t param_handle = param_find(param_name);

        if (param_handle != PARAM_INVALID) {
            if (param_get_type(param_handle) != PARAM_TYPE_INT32) {
                return -1;
            }

            int32_t pwm_parm;
            int     ret = param_get(param_handle, &pwm_parm);

            if (ret != 0) {
                return ret;
            }

            value = pwm_parm;

        } else {
            LOG_E("param '%s' not found", param_name);
            return -1;
        }

    } else {
        char *ep;
        value = strtol(option, &ep, 0);

        if (*ep != '\0') {
            return -1;
        }
    }

    return 0;
}
#endif // __cplusplus


#endif //__PARAM_H__
