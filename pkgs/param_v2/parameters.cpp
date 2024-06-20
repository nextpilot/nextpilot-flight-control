/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "param.interface"
#define LOG_LVL LOG_LVL_INFO

#include <rtthread.h>
#include <rtdbg.h>
#ifdef PKG_USING_HRTIMER
#   include <hrtimer.h>
#endif // PKG_USING_HRTIMER
#ifdef PKG_USING_UORB_
#   include <uORB.h>
#   include <topics/parameter_update.h>
#endif // PKG_USING_UORB_
#include <utarray.h>
#include "param_private.h"
#include "param.h"


static int                 reader_lock_holders = 0;
static struct rt_semaphore reader_lock_holders_lock;
static struct rt_semaphore param_sem;

static void param_lock_reader() {
    rt_sem_take(&reader_lock_holders_lock, RT_WAITING_FOREVER);

    ++reader_lock_holders;

    if (reader_lock_holders == 1) {
        rt_sem_take(&param_sem, RT_WAITING_FOREVER);
    }

    rt_sem_release(&reader_lock_holders_lock);
}

static void param_unlock_reader() {
    rt_sem_take(&reader_lock_holders_lock, RT_WAITING_FOREVER);

    --reader_lock_holders;

    if (reader_lock_holders == 0) {
        // the last reader releases the lock
        rt_sem_release(&param_sem);
    }

    rt_sem_release(&reader_lock_holders_lock);
}

static void param_lock_writer() {
    rt_sem_take(&param_sem,RT_WAITING_FOREVER);
}

static void param_unlock_writer() {
    rt_sem_release(&param_sem);
}

///////////////////////////////////////////////////////////////////////////////////////
// UT_array接口定义
///////////////////////////////////////////////////////////////////////////////////////
// 存储被修改的值
UT_array *_utarray_param_values = nullptr;

typedef struct param_wbuf_s {
    param_value_t value;
    // param_status_t status;
    param_t index;
} utarray_param_value_t;

const UT_icd _utarray_param_values_icd = {sizeof(utarray_param_value_t), nullptr, nullptr, nullptr};

// 存储被修改的状态
UT_array *_utarray_param_status = nullptr;

typedef struct utarray_param_status_s {
    param_status_t status;
    param_t        index;
} utarray_param_status_t;

const UT_icd _utarray_param_status_icd = {sizeof(utarray_param_status_t), nullptr, nullptr, nullptr};

static int utarray_compare_params(const void *a, const void *b) {
    struct param_wbuf_s *pa = (struct param_wbuf_s *)a;
    struct param_wbuf_s *pb = (struct param_wbuf_s *)b;

    if (pa->index < pb->index) {
        return -1;
    }

    if (pa->index > pb->index) {
        return 1;
    }

    return 0;
}

static struct param_wbuf_s *param_find_changed(param_t idx) {
    if (_utarray_param_values) {
        struct param_wbuf_s key {};

        key.index = idx;
        return (struct param_wbuf_s *)utarray_find(_utarray_param_values, &key, utarray_compare_params);
    }

    return nullptr;
}

// mode = 1表示如果不存在则创建一个
static utarray_param_status_t *utarray_find_status(param_t idx, int mode = 0) {
    if (!param_in_range(idx)) {
        return nullptr;
    }

    // 状态数组是否创建
    if (!_utarray_param_status && mode != 0) {
        // 创建状态数组
        utarray_new(_utarray_param_status, &_utarray_param_status_icd);
        // 还是无效就是初始化失败了
        if (!_utarray_param_status) {
            LOG_E("fail to create param status array");
            return nullptr;
        }
    }

    if (!_utarray_param_status) {
        return nullptr;
    }

    // 状态数组中查找
    utarray_param_status_t key{};
    key.index                 = idx;
    utarray_param_status_t *s = (utarray_param_status_t *)utarray_find(_utarray_param_status, &key, utarray_compare_params);
    if (!s && mode != 0) {
        // 往数组里面push一个
        key.status.value = 0;
        utarray_push_back(_utarray_param_status, &key);
        utarray_sort(_utarray_param_status, utarray_compare_params);
        s = (utarray_param_status_t *)utarray_find(_utarray_param_status, &key, utarray_compare_params);
        // 还是没找到就不对了
        if (!s) {
            LOG_E("status storage slot invalid");
        }
    }

    return s;
}

///////////////////////////////////////////////////////////////////////////////////////
// 多种param接口注册和查询
///////////////////////////////////////////////////////////////////////////////////////
#define PARAM_MAX_API_COUNT 3
static param_interface_t *__param_api_table__[PARAM_MAX_API_COUNT] = {NULL, NULL, NULL};

int param_interface_register(param_interface_t *api) {
    if (!api) {
        return -1;
    }

    bool success = false;
    for (int i = 0; i < PARAM_MAX_API_COUNT; i++) {
        if (__param_api_table__[i] == NULL) {
            __param_api_table__[i] = api;
            success                = true;
            break;
        }
    }

    return success ? 0 : -1;
}

static param_interface_t *param_get_whois(param_t *idx) {
    uint16_t offset = 0;
    for (size_t i = 0; i < PARAM_MAX_API_COUNT; i++) {
        param_interface_t *api = __param_api_table__[i];
        if (api) {
            if ((*idx) < offset + api->ops->get_count()) {
                (*idx) -= offset;
                return api;
            }
            offset += api->ops->get_count();
        }
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////
// 参数更新通知
////////////////////////////////////////////////////////////////////////////

void param_notify_changes() {
#ifdef PKG_USING_UORB_
    static uint32_t           param_instance = 0;
    struct parameter_update_s pup;
    pup.instance = param_instance++;
#   ifdef PKG_USING_HRTIMER
    pup.timestamp = hrt_absolute_time();
#   else
    pup.timestamp = rt_tick_get() * 1000ULL;
#   endif // PKG_USING_HRTIMER
    orb_publish(ORB_ID(parameter_update), NULL, &pup);
#endif    // PKG_USING_UORB
    LOG_D("notify param updated");
}

////////////////////////////////////////////////////////////////////////////
// 参数个数统计
////////////////////////////////////////////////////////////////////////////

uint16_t param_get_count() {
    uint16_t count = 0;
    for (size_t i = 0; i < PARAM_MAX_API_COUNT; i++) {
        param_interface_t *api = __param_api_table__[i];
        if (api) {
            count += api->ops->get_count();
        }
    }
    return count;
}

param_t param_get_count_used() {
    param_t count = 0;

    if (_utarray_param_status) {
        utarray_param_status_s *s = nullptr;
        while ((s = (utarray_param_status_t *)utarray_next(_utarray_param_values, s)) != nullptr) {
            if (s->status.actived) {
                count++;
            }
        }
    }

    return count;
}

bool param_in_range(param_t idx) {
    return idx < param_get_count();
}

////////////////////////////////////////////////////////////////////////////
// 获取meta信息（get info）
////////////////////////////////////////////////////////////////////////////

int param_get_info(param_t idx, param_info_t *info) {
    param_interface_t *api = param_get_whois(&idx);

    if (api) {
        return api->ops->get_info(idx, info);
    }

    return -1;
}

const char *param_get_name(param_t idx) {
    param_info_t info;
    int          ret = param_get_info(idx, &info);
    if (ret == 0) {
        return info.name;
    } else {
        return nullptr;
    }
}

param_type_t param_get_type(param_t idx) {
    param_info_t info;
    int          ret = param_get_info(idx, &info);
    if (ret == 0) {
        return info.type;
    } else {
        return PARAM_TYPE_UNKNOWN;
    }
}

const char *param_get_type_cstr(param_t idx) {
    return param_type_cstr(param_get_type(idx));
}

param_flag_t param_get_flag(param_t idx) {
    param_info_t info;
    int          ret = param_get_info(idx, &info);
    if (ret == 0) {
        return info.flag;
    } else {
        param_flag_t flag = {.value = 0};
        return flag;
    }
}

int param_get_default_value(param_t idx, param_value_t *val) {
    param_info_t info;
    int          ret = param_get_info(idx, &info);
    if (ret == 0) {
        (*val) = info.value;
        return 0;
    } else {
        return -1;
    }
}

////////////////////////////////////////////////////////////////////////////
// 查询/设置状态（get/set status）
////////////////////////////////////////////////////////////////////////////

param_status_t param_get_status(param_t idx) {
    param_status_t status = {.value = 0};

    rt_enter_critical();
    if (_utarray_param_status) {
        utarray_param_status_t key{};

        key.index                    = idx;
        utarray_param_status_s *pbuf = (utarray_param_status_t *)utarray_find(_utarray_param_status, &key, utarray_compare_params);

        if (pbuf) {
            status = pbuf->status;
        }
    }
    rt_exit_critical();

    return status;
}

bool param_value_used(param_t idx) {
    return param_get_status(idx).actived;
}

bool param_value_unsaved(param_t idx) {
    return param_get_status(idx).unsaved;
}

bool param_value_changed(param_t idx) {
    return param_get_status(idx).changed;
}

int param_set_status_internal(param_t idx, const param_status_t *status, int action = 0) {
    if (!param_in_range(idx)) {
        return -1;
    }

    if (!status) {
        LOG_I("invalid status point");
        return -1;
    }

    int ret = -1;

    // 临界区保护
    rt_enter_critical();
    utarray_param_status_t *s = utarray_find_status(idx, 1);
    if (s) {
        ret = 0;
        // 根据action设置状态
        if (action == 0) {        // set
            s->status.value = status->value;
        } else if (action == 1) { // bit set
            s->status.value |= status->value;
        } else if (action == 2) { // bit clear
            s->status.value &= (~(status->value));
        } else {
            ret = -1;
            LOG_I("invalid action value: %d", action);
        }
    }
    // 退出临界区
    rt_exit_critical();


    return ret;
}

int param_set_status(param_t idx, const param_status_t *status) {
    return param_set_status_internal(idx, status, 0);
}

int param_set_used(param_t idx) {
    param_status_t status{};
    status.value   = 0;
    status.actived = true;
    return param_set_status_internal(idx, &status, 1);
}

////////////////////////////////////////////////////////////////////////////
// 查找/查询/设置参数（get/set param）
////////////////////////////////////////////////////////////////////////////

int param_find_internal(const char *name, bool mark_used) {
    param_t idx    = PARAM_INVALID;
    int     offset = 0;

    for (size_t i = 0; i < PARAM_MAX_API_COUNT; i++) {
        param_interface_t *api = __param_api_table__[i];

        if (api) {
            idx = api->ops->find(name);
            if (idx != PARAM_INVALID) {
                if (mark_used) {
                    param_set_used(idx + offset);
                }
                LOG_D("find param %s", name);
                return idx + offset;
            }

            offset += api->ops->get_count();
        }
    }

    // LOG_D("find %s fail", name);
    return PARAM_INVALID;
}

param_t param_find(const char *name) {
    param_t idx = param_find_internal(name, true);
    if (idx == PARAM_INVALID) {
        LOG_W("can't find %s", name);
    }
    return idx;
}

param_t param_find_no_notification(const char *name) {
    return param_find_internal(name, false);
}

static const void *param_get_value_ptr(param_t idx) {
    if (!param_in_range(idx)) {
        return nullptr;
    }

    /* work out whether we're fetching the default or a written value */
    struct param_wbuf_s *s = param_find_changed(idx);
    if (s) {
        return &(s->value);
    }

    param_interface_t *api = param_get_whois(&idx);
    if (api) {
        return api->ops->get_value_ptr(idx);
    }

    LOG_E("not param api found");

    return nullptr;
}

int param_get_internal(param_t idx, param_value_t *val, bool mark_used) {
    if (!param_in_range(idx)) {
        return -1;
    }

    if (!val) {
        return -1;
    }

    // 是否已经修改了
    rt_enter_critical();
    struct param_wbuf_s *s = param_find_changed(idx);
    if (s) {
        *val = s->value;
        rt_exit_critical();
        return 0;
    }
    rt_exit_critical();


    // 没有修改则获取默认值
    param_interface_t *api = param_get_whois(&idx);
    if (api) {
        param_info_t info;
        if (api->ops->get_info(idx, &info) == 0) {
            *val = info.value;
            return 0;
        }
    }

    LOG_E("not param api found");
    return -1;
}

int param_get(param_t idx, void *val) {
    return param_get_internal(idx, (param_value_t *)val, false);
}

float param_get_float(param_t idx) {
    param_value_t val;
    int           ret = param_get_internal(idx, &val, false);
    if (ret == 0) {
        return val.f32;
    } else {
        return 0.0f / 0.0f;
    }
}

int32_t param_get_int32(param_t idx) {
    param_value_t val;
    int           ret = param_get_internal(idx, &val, false);
    if (ret == 0) {
        return val.i32;
    } else {
        return INT32_MAX;
    }
}

int param_set_internal(param_t idx, const param_value_t *val, bool mark_saved, bool notify) {
    if (!param_in_range(idx)) {
        return -1;
    }

    if (!val) {
        return -1;
    }

    int                     ret = -1;
    struct param_wbuf_s    *s   = nullptr;
    utarray_param_status_t *t   = nullptr;

    // 进入临界区保护
    rt_enter_critical();

    // 初始化param_values动态数组
    if (!_utarray_param_values) {
        utarray_new(_utarray_param_values, &_utarray_param_values_icd);
    }
    // 还是空指针就不对了
    if (!_utarray_param_values) {
        LOG_E("fail to allocate modified values array");
        goto __exit;
    }

    // 在数组中查找是否已经存在
    s = param_find_changed(idx);
    // 如果没有找到则压入
    if (!s) {
        /* construct a new parameter */
        struct param_wbuf_s buf {};

        buf.index = idx;
        /* add it to the array and sort */
        utarray_push_back(_utarray_param_values, &buf);
        utarray_sort(_utarray_param_values, utarray_compare_params);

        /* find it after sorting */
        s = param_find_changed(idx);
    }
    // 如果还是找不到就不对了哦
    if (!s) {
        LOG_I("error _utarray_param_values storage slot invalid");
        goto __exit;
    }
    // 将保存新数据
    s->value = *val;

    t = utarray_find_or_new_status(idx);
    if (t) {
        t->status.changed         = true;
        t->status.unsaved         = !mark_saved;
        t->status.unsaved_to_ulog = true;
    }

    LOG_D("set param %s", param_get_name(idx));

    ret = 0;

__exit:
    rt_exit_critical();

    if (ret == 0) {
        // 发布param更新消息
        if (notify) {
            param_notify_changes();
        }
        //触发param自动保存
        if (!mark_saved) {
            param_notify_autosave();
        }
    }

    return ret;
}

int param_set(param_t idx, const void *val) {
    return param_set_internal(idx, (const param_value_t *)val, false, true);
}

int param_set_no_notification(param_t idx, const void *val) {
    return param_set_internal(idx, (const param_value_t *)val, false, false);
}

////////////////////////////////////////////////////////////////////////////
// 重置参数（reset param）
////////////////////////////////////////////////////////////////////////////

int param_reset_internal(param_t idx, bool notify) {
    if (!param_in_range(idx)) {
        return -1;
    }

    param_wbuf_s           *s = nullptr;
    utarray_param_status_t *t = nullptr;

    rt_enter_critical();

    // 数组中查找是否存在，存在则删除
    s = param_find_changed(idx);
    if (s) {
        int pos = utarray_eltidx(_utarray_param_values, s);
        utarray_erase(_utarray_param_values, pos, 1);
    }

    // 数组中查找是否存在，存在则重置
    t = utarray_find_status(idx);
    if (t) {
        t->status.changed         = false;
        t->status.unsaved         = true;
        t->status.unsaved_to_ulog = true;
    }

    rt_exit_critical();


    if (s != nullptr && notify) {
        param_notify_changes();
        param_notify_autosave();
    }

    return 0;
}

int param_reset(param_t idx) {
    return param_reset_internal(idx, true);
}

int param_reset_no_notification(param_t idx) {
    return param_reset_internal(idx, false);
}

void param_reset_all_internal(bool notify) {
    rt_enter_critical();
    if (_utarray_param_values) {
        utarray_free(_utarray_param_values);
    }
    _utarray_param_values = nullptr;

    // 重置状态
    if (_utarray_param_status) {
        utarray_param_status_s *s = nullptr;
        while ((s = (utarray_param_status_t *)utarray_next(_utarray_param_values, s)) != nullptr) {
            s->status.changed = false;
        }
    }
    rt_exit_critical();

    // 重置结束之后发送param_update消息
    if (notify) {
        param_notify_autosave();
        param_notify_changes();
    }

    LOG_I("reset all params");
}

void param_reset_all() {
    param_reset_all_internal(true);
}

void param_reset_all_no_notification() {
    param_reset_all_internal(false);
}

void param_reset_excludes(const char *excludes[], int num_excludes) {
    bool found = false;
    for (param_t idx = 0; idx < param_get_count(); idx++) {
        const char *name    = param_get_name(idx);
        bool        exclude = false;
        for (int kk = 0; kk < num_excludes; kk++) {
            int len = rt_strlen(excludes[kk]);

            if ((excludes[kk][len - 1] == '*'
                 && rt_strncmp(name, excludes[kk], len - 1) == 0)
                || rt_strcmp(name, excludes[kk]) == 0) {
                exclude = true;
                break;
            }
        }

        if (!exclude) {
            // 重置过程中不发布param_update消息
            param_reset_internal(idx, false);
            found = true;
        }
    }

    if (found) {
        param_notify_changes();
        param_notify_autosave();
    }
}

void param_reset_specific(const char *resets[], int num_resets) {
    bool found = false;
    for (param_t idx = 0; idx < param_get_count(); idx++) {
        const char *name  = param_get_name(idx);
        bool        reset = false;

        for (int kk = 0; kk < num_resets; kk++) {
            int len = rt_strlen(resets[kk]);

            if ((resets[kk][len - 1] == '*'
                 && rt_strncmp(name, resets[kk], len - 1) == 0)
                || rt_strcmp(name, resets[kk]) == 0) {
                reset = true;
                break;
            }
        }

        if (reset) {
            param_reset_internal(idx, false);
            found = true;
        }
    }

    if (found) {
        param_notify_changes();
        param_notify_autosave();
    }
}

void param_foreach(void (*func)(void *arg, param_t idx), void *arg, bool only_changed, bool only_used) {
    param_t idx;

    for (idx = 0; param_in_range(idx); idx++) {
        /* if requested, skip unchanged values */
        if (only_changed && param_value_changed(idx)) {
            continue;
        }

        if (only_used && !param_value_used(idx)) {
            continue;
        }

        func(arg, idx);
    }
}

uint32_t param_hash_check(void) {
    uint32_t hash = 0;

    rt_enter_critical();
    for (param_t idx = 0; idx < param_get_count(); idx++) {
        if (!param_value_used(idx) /*|| param_is_volatile(idx)*/) {
            continue;
        }

        const char *name  = param_get_name(idx);
        const void *value = param_get_value_ptr(idx);

        hash = crc32part((const uint8_t *)name, rt_strlen(name), hash);
        hash = crc32part((const uint8_t *)value, param_get_size(idx), hash);
    }
    rt_exit_critical();

    return hash;
}

/////////////////////////////////////////////////////////////
// 导入/导出参数
/////////////////////////////////////////////////////////////
#define PARAM_MAX_DEV_COUNT 2
static param_storage_t *__param_storage__[PARAM_MAX_DEV_COUNT] = {NULL, NULL};

RT_WEAK uint32_t crc32part(const void *buff, int size, uint32_t crc) {
    for (int i = 0; i < size; i++) {
        crc += ((uint8_t *)buff)[i];
    }
    return crc;
}

int param_storage_register(param_storage_t *dev) {
    if (!dev) {
        return -1;
    }

    bool success = false;
    for (int i = 0; i < PARAM_MAX_DEV_COUNT; i++) {
        if (__param_storage__[i] == NULL) {
            __param_storage__[i] = dev;
            success              = true;
            break;
        }
    }

    return success ? 0 : -1;
}

int param_export_internal(const char *devname, param_filter_func filter) {
    if (!_utarray_param_values) {
        return -1;
    }

    param_storage_t *dev = __param_storage__[0];

    if (!dev) {
        return -1;
    }

    // 先打开设备
    if (dev->ops->open(dev, devname, 'w') != 0) {
        // LOG_E("open %s fail", devname);
        return -1;
    }


    // 写入所有修改的参数
    param_payload_t payload;
    param_status_t  status;
    uint16_t        count = 0;
    uint32_t        check = 0;

    param_wbuf_s *s = nullptr;
    while ((s = (struct param_wbuf_s *)utarray_next(_utarray_param_values, s)) != nullptr) {
        if (filter && !filter(s->index)) {
            continue;
        }
        // 对payload赋值
        rt_memcpy(payload.name, param_get_name(s->index), sizeof(payload.name));
        payload.type = param_get_type(s->index);
        param_get(s->index, &payload.value);
        // 写入到设备
        dev->ops->write(dev, sizeof(param_header_t) + count * sizeof(payload), &payload, sizeof(payload));
        // TODO:将参数标记为已保存

        // 计算crc32
        check = crc32part(&payload, sizeof(payload), check);


        if (payload.type == PARAM_TYPE_INT32) {
            LOG_D("export param %s, %s, %d", payload.name, param_type_cstr(payload.type), payload.value.i32);
        } else if (payload.type == PARAM_TYPE_FLOAT) {
            LOG_D("export param %s, %s, %g", payload.name, param_type_cstr(payload.type), payload.value.f32);
        }

        count++;
    }

    for (uint16_t idx = 0; idx < param_get_count(); idx++) {
        status = param_get_status(idx);

        if (status.changed) {
            // 对payload赋值
            rt_memcpy(payload.name, param_get_name(idx), sizeof(payload.name));
            payload.type = param_get_type(idx);
            param_get(idx, &payload.value);
            // 写入到设备
            dev->ops->write(dev, sizeof(param_header_t) + count * sizeof(payload), &payload, sizeof(payload));
            // 将参数标记为已保存
            status.unsaved = false;
            param_set_status(idx, &status);
            // 计算crc32
            check = crc32part(&payload, sizeof(payload), check);
            //
            count++;

            if (payload.type == PARAM_TYPE_INT32) {
                LOG_D("export param %s, %s, %d", payload.name, param_type_cstr(payload.type), payload.value.i32);
            } else if (payload.type == PARAM_TYPE_FLOAT) {
                LOG_D("export param %s, %s, %g", payload.name, param_type_cstr(payload.type), payload.value.f32);
            }
        }
    }

    // 写入头
    param_header_t header;
    header.check = check;
    header.count = count;
    header.utc   = time(NULL);
    rt_memcpy(header.magic, "nextpilot", sizeof(header.magic));
    dev->ops->write(dev, 0, &header, sizeof(header));

    LOG_D("write header, utc=%u, count=%u, crc=%u", header.utc, count, check);

    // 关闭设备
    dev->ops->close(dev);

    if (devname) {
        LOG_I("export params to %s", devname);
    } else if (dev->name) {
        LOG_I("export params to %s", dev->name);
    } else {
        LOG_I("export params to %s", "default");
    }

    return 0;
}

int param_import_internal(const char *devname, param_filter_func filter) {
    param_storage_t *dev = __param_storage__[0];

    if (!dev) {
        return -1;
    }

    // 先打开设备
    if (dev->ops->open(dev, devname, 'r') != 0) {
        // LOG_E("open %s fail", devname);
        return -1;
    }

    int             ret = -1;
    param_header_t  header;
    param_payload_t payload;
    uint32_t        check = 0;

    // 读取头部信息
    if (dev->ops->read(dev, 0, &header, sizeof(header)) != sizeof(header)) {
        LOG_E("read header fail");
        goto __exit;
    }
    // 对比magic魔术字
    if (rt_strncmp(header.magic, "zhan", 4) != 0 || // magic不对
        header.utc < 1514764800LL ||                // 时间戳不对
        header.count == 0) {                        // 长度为0
        goto __exit;
    }
    LOG_D("read header, utc=%u, count=%u, crc=%u", header.utc, header.count, header.check);

    // 计算存储param的check
    for (uint16_t idx = 0; idx < header.count; idx++) {
        dev->ops->read(dev, sizeof(header) + idx * sizeof(payload), &payload, sizeof(payload));
        check = crc32part((const uint8_t *)&payload, sizeof(payload), check);
    }
    if (check != header.check) {
        LOG_W("check crc fail, recv=%u, calc=%u", header.check, check);
        goto __exit;
    }

    // 读取param并写入系统
    for (uint16_t idx = 0; idx < header.count; idx++) {
        // 读取一个param
        dev->ops->read(dev, sizeof(header) + idx * sizeof(payload), &payload, sizeof(payload));
        // 查找系统中是否已存在
        param_t pp = param_find_internal(payload.name, false);
        if (pp == PARAM_INVALID || param_get_type(pp) != payload.type) {
            continue;
        }
        // 更新到系统
        param_set_internal(pp, &payload.value, true, false);
        // 打印调试信息
        if (payload.type == PARAM_TYPE_INT32) {
            LOG_D("import param %s, %s, %d", payload.name, param_type_cstr(payload.type), payload.value.i32);
        } else if (payload.type == PARAM_TYPE_FLOAT) {
            LOG_D("import param %s, %s, %g", payload.name, param_type_cstr(payload.type), payload.value.f32);
        }
    }

    // 提醒参数已经更新
    param_notify_autosave();
    param_notify_changes();

    if (devname) {
        LOG_I("import params from %s", devname);
    } else if (dev->name) {
        LOG_I("import params from %s", dev->name);
    } else {
        LOG_I("import params from %s", "default");
    }

    ret = 0;

__exit:
    // 关闭设备
    dev->ops->close(dev);
    return ret;
}

int param_reset_and_import(const char *devname) {
    // 如果devname=NULL表示使用默认文件路径

    // 先重置所有参数，但是不发送param_update
    param_reset_all_no_notification();

    // 然后从设备中加载参数，并发送param_update
    return param_import_internal(devname, NULL);
}

int param_load_default() {
    return param_reset_and_import(NULL);
}

int param_save_default() {
    return param_export_internal(NULL, NULL);
}

static bool        _param_autosave_enable = true;
static rt_event_t  _param_autosave_event  = NULL;
static rt_thread_t _param_autosave_thread = NULL;
#define PARAM_EVENT_UPDATED (1 << 0)

void param_notify_autosave() {
    if (_param_autosave_enable && _param_autosave_event) {
        rt_event_send(_param_autosave_event, PARAM_EVENT_UPDATED);
        LOG_D("notify param autosave");
    }
}

void param_control_autosave(bool enable) {
    rt_enter_critical();
    _param_autosave_enable = enable;
    rt_exit_critical();
}

static void param_autosave_entry(void *param) {
    rt_tick_t last_autosave_timestamp = rt_tick_get();
    while (1) {
        // 等待autosave事件
        uint32_t evt = 0;
        rt_err_t ret = rt_event_recv(_param_autosave_event, PARAM_EVENT_UPDATED,
                                     RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                                     RT_WAITING_FOREVER, &evt);

        if (ret == 0 && _param_autosave_enable) {
            // 保存间隔时间要大于3s
            rt_tick_t delta = rt_tick_get() - last_autosave_timestamp;
            if (delta < 3000UL) {
                rt_thread_mdelay(3000 - delta);
            }

            // 保存参数到flash或文件
            if (param_save_default() == 0) {
                last_autosave_timestamp = rt_tick_get();
                LOG_D("param autosave okay");
            } else {
                LOG_E("param autosave fail");
            }
        }
    }
}

RT_WEAK void board_param_init() {
}

static int param_storage_init() {
    // 从默认设备加载param
    if (param_load_default() == 0) {
        LOG_D("load param from default");
    }

    // 加载板级参数
    // board_param_init();

    _param_autosave_event = rt_event_create("param_autosave", RT_IPC_FLAG_PRIO);
    if (!_param_autosave_event) {
        LOG_E("create autosave event fail");
        return 0;
    }

    _param_autosave_thread = rt_thread_create("param_autosave", param_autosave_entry, NULL, 2018, 20, 5);

    if (!_param_autosave_thread) {
        LOG_E("create autosave thread fail");
        return -1;
    }

    if (rt_thread_startup(_param_autosave_thread) != 0) {
        rt_thread_delete(_param_autosave_thread);
        LOG_E("startup autosave thread fail");
        return -1;
    }

    return 0;
}

INIT_EXPORT(param_storage_init, "5.2");
