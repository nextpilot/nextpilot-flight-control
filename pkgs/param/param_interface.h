/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __PARAM_INTERFACE_H__
#define __PARAM_INTERFACE_H__

#include "param_common.h"

typedef struct param_interface_ops_s {
    int (*init)();
    param_t (*find)(const char *name, bool mark_used);
    uint16_t (*get_count)();
    int (*get_info)(param_t idx, param_info_t *info);
    int (*get_value)(param_t idx, param_value_t *val, bool mark_used);
    int (*set_value)(param_t idx, const param_value_t *val, bool mark_saved);
    int (*get_status)(param_t idx, param_status_t *status);
    int (*set_status)(param_t idx, const param_status_t *status);
    int (*reset)(param_t idx, bool mark_saved);
} param_interface_ops_t;

typedef struct param_interface_s {
    const char            *name;
    uint8_t                type;
    param_interface_ops_t *ops;
} param_interface_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int param_interface_register(param_interface_t *api);

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
bool     param_in_range(param_t idx);

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
 * Determine the size of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		The size of the parameter's value.
 */
uint8_t param_get_size(param_t idx);

/**
 * Obtain the type of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		The type assigned to the parameter.
 */
param_type_t param_get_type(param_t idx);
const char  *param_get_type_cstr(param_t idx);
param_flag_t param_get_flag(param_t idx);

/**
 * Copy the (airframe-specific) default value of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param default_val		Where to return the value, assumed to point to suitable storage for the parameter type.
 * @return		Zero if the parameter's deafult value could be returned, nonzero otherwise.
 */
int param_get_default_value(param_t idx, param_value_t *val);

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
// 设置/查询状态
///////////////////////////////////////////////////
param_status_t param_get_status(param_t idx);
int            param_set_status(param_t idx, const param_status_t *status);

/**
 * Test whether a parameter's value has been changed but not saved.
 *
 * @return		If true, the parameter's value has not been saved.
 */
bool param_value_unsaved(param_t idx);

/**
 * Wether a parameter is in use in the system.
 *
 * @return		True if it has been written or read
 */
bool param_value_used(param_t idx);
bool param_value_changed(param_t idx);

/**
 * Mark a parameter as used. Only marked parameters will be sent to a GCS.
 * A call to param_find() will mark a param as used as well.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 */
int param_set_used(param_t idx);
// int  param_set_saved(param_t idx);
// int  param_set_saved_ulog(param_t idx);

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
int      param_calc_hash();
int      param_cacl_hash_specific();

#ifdef __cplusplus
}
#endif //__cplusplus __cplusplus

#endif // __PARAM_INTERFACE_H__
