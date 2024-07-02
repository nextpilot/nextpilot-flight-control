/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "param"
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

#include <atomic/atomic_bitset.hpp>
#include <utarray.h>
#include "param.h"

#include "param_autogen.hpp"

using namespace nextpilot::global_params;

RT_WEAK uint32_t crc32part(const void *buff, int size, uint32_t crc) {
    for (int i = 0; i < size; i++) {
        crc += ((uint8_t *)buff)[i];
    }
    return crc;
}

///////////////////////////////////////////////////////////////////////////////////////
// 基于sem实现读写锁
///////////////////////////////////////////////////////////////////////////////////////

static int                 _reader_lock_holders = 0;
static struct rt_semaphore _reader_lock_holders_lock;
static struct rt_semaphore _param_sem;

static void param_lock_reader() {
    rt_sem_take(&_reader_lock_holders_lock, RT_WAITING_FOREVER);

    ++_reader_lock_holders;

    if (_reader_lock_holders == 1) {
        rt_sem_take(&_param_sem, RT_WAITING_FOREVER);
    }

    rt_sem_release(&_reader_lock_holders_lock);
}

static void param_unlock_reader() {
    rt_sem_take(&_reader_lock_holders_lock, RT_WAITING_FOREVER);

    --_reader_lock_holders;

    if (_reader_lock_holders == 0) {
        // the last reader releases the lock
        rt_sem_release(&_param_sem);
    }

    rt_sem_release(&_reader_lock_holders_lock);
}

static void param_lock_writer() {
    rt_sem_take(&_param_sem, RT_WAITING_FOREVER);
}

static void param_unlock_writer() {
    rt_sem_release(&_param_sem);
}

/** assert that the parameter store is locked */
static void param_assert_locked() {
    /* XXX */
}

///////////////////////////////////////////////////////////////////////////////////////
// UT_array接口定义
///////////////////////////////////////////////////////////////////////////////////////

static constexpr uint16_t _param_info_count = sizeof(params_meta) / sizeof(param_info_s);

static AtomicBitset<_param_info_count> _params_active;
static AtomicBitset<_param_info_count> _params_changed;
static AtomicBitset<_param_info_count> _params_unsaved;

// 存储被修改的值
static UT_array *_utarray_param_values = nullptr;

typedef struct param_wbuf_s {
    param_value_t value;
    // param_status_t status;
    param_t index;
} utarray_param_value_t;

static const UT_icd _utarray_param_values_icd = {sizeof(utarray_param_value_t), nullptr, nullptr, nullptr};

static int utarray_param_compare(const void *a, const void *b) {
    struct param_wbuf_s *pa = (struct param_wbuf_s *)a;
    struct param_wbuf_s *pb = (struct param_wbuf_s *)b;

    if (pa->index < pb->index) {
        return -RT_ERROR;
    }

    if (pa->index > pb->index) {
        return 1;
    }

    return 0;
}

static struct param_wbuf_s *param_find_changed(param_t idx) {
    param_assert_locked();

    if (_params_changed[idx] && _utarray_param_values) {
        struct param_wbuf_s key {};

        key.index = idx;
        return (struct param_wbuf_s *)utarray_find(_utarray_param_values, &key, utarray_param_compare);
    }

    return nullptr;
}

////////////////////////////////////////////////////////////////////////////
// 参数更新通知
////////////////////////////////////////////////////////////////////////////

void param_notify_changes() {
#ifdef PKG_USING_UORB_
    static uint32_t           param_instance = 0;
    struct parameter_update_s pup;
    pup.instance       = param_instance++;
    pup.get_count      = 0;
    pup.set_count      = 0;
    pup.find_count     = 0;
    pup.export_count   = 0;
    pup.active         = 0;
    pup.custom_default = 0;
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
    return _param_info_count;
}

param_t param_get_count_used() {
    return _params_active.count();
}

bool param_in_range(param_t idx) {
    return idx < param_get_count();
}

////////////////////////////////////////////////////////////////////////////
// 获取meta信息（get info）
////////////////////////////////////////////////////////////////////////////

param_info_t param_get_info(param_t idx) {
    if (param_in_range(idx)) {
        return params_meta[idx];
    }
    param_info_t info{};

    return info;
}

const char *param_get_name(param_t idx) {
    if (param_in_range(idx)) {
        return params_meta[idx].name;
    }
    return nullptr;
}

param_type_t param_get_type(param_t idx) {
    if (param_in_range(idx)) {
        return params_meta[idx].type;
    }
    return PARAM_TYPE_UNKNOWN;
}

const char *param_get_type_cstr(param_t idx) {
    if (param_in_range(idx)) {
        return param_type_cstr(param_get_type(idx));
    }
    return "unknown";
}

uint8_t param_get_type_size(param_t idx) {
    if (param_in_range(idx)) {
        return param_type_size(param_get_type(idx));
    }
    return 0;
}

int param_get_default_value(param_t idx, param_value_t *val) {
    if (!param_in_range(idx) || !val) {
        return -RT_ERROR;
    }

    *val = params_meta[idx].value;
    return 0;
}

param_flag_t param_get_flag(param_t idx) {
    if (param_in_range(idx)) {
        return params_meta[idx].flag;
    }

    param_flag_t flag = {.value = 0};
    return flag;
}

bool param_is_volatile(param_t idx) {
    if (param_in_range(idx)) {
        return params_meta[idx].flag.system_required;
    }
    return false;
}

bool param_reboot_required(param_t idx) {
    if (param_in_range(idx)) {
        return params_meta[idx].flag.reboot_required;
    }
    return false;
}

bool param_disarm_required(param_t idx) {
    if (param_in_range(idx)) {
        return params_meta[idx].flag.disarm_required;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////
// 查询/设置状态（get/set status）
////////////////////////////////////////////////////////////////////////////

param_status_t param_get_status(param_t idx) {
    param_status_t status{.value = 0};
    if (param_in_range(idx)) {
        status.actived = _params_active[idx];
        status.changed = _params_changed[idx];
        status.unsaved = _params_unsaved[idx];
    }
    return status;
}

bool param_value_used(param_t idx) {
    if (param_in_range(idx)) {
        return _params_active[idx];
    }

    return false;
}

bool param_value_unsaved(param_t idx) {
    if (param_in_range(idx)) {
        return _params_unsaved[idx];
    }
    return false;
}

bool param_value_changed(param_t idx) {
    if (param_in_range(idx)) {
        return _params_changed[idx];
    }
    return false;
}

void param_mark_changed(param_t idx) {
    if (param_in_range(idx)) {
        _params_changed.set(idx, true);
    }
}

void param_mark_used(param_t idx) {
    if (param_in_range(idx)) {
        _params_active.set(idx, true);
    }
}

////////////////////////////////////////////////////////////////////////////
// 查找/查询/设置参数（get/set param）
////////////////////////////////////////////////////////////////////////////

int param_find_internal(const char *name, bool mark_used) {
    // perf_count(param_find_perf);

    param_t middle;
    param_t front = 0;
    param_t last  = _param_info_count;

    /* perform a binary search of the known parameters */

    while (front <= last) {
        middle  = front + (last - front) / 2;
        int ret = rt_strcmp(name, param_get_name(middle));

        if (ret == 0) {
            if (mark_used) {
                param_mark_used(middle);
            }

            return middle;

        } else if (middle == front) {
            /* An end point has been hit, but there has been no match */
            break;

        } else if (ret < 0) {
            last = middle;

        } else {
            front = middle;
        }
    }

    /* not found */
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
    } else {
        return &params_meta[idx].value;
    }
}

int param_get_internal(param_t idx, param_value_t *val, bool mark_used) {
    if (!param_in_range(idx)) {
        return -RT_ERROR;
    }

    if (!val) {
        return -RT_ERROR;
    }

    int ret = -RT_ERROR;

    if (!_params_changed[idx]) {
        *val = params_meta[idx].value;
        ret  = 0;
    } else {
        param_lock_reader();
        struct param_wbuf_s *s = param_find_changed(idx);
        if (s) {
            *val = s->value;
            ret  = 0;
        }
        param_unlock_reader();
    }

    if (ret == 0 && mark_used) {
        _params_active.set(idx, true);
    }

    return ret;
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
        return -RT_ERROR;
    }

    if (!val) {
        return -RT_ERROR;
    }

    int                  ret = -RT_ERROR;
    struct param_wbuf_s *s   = nullptr;

    // 进入临界区保护
    param_lock_writer();

    // 初始化param_values动态数组
    if (!_utarray_param_values) {
        utarray_new(_utarray_param_values, &_utarray_param_values_icd);
        // mark all parameters unchanged (default)
        _params_changed.reset();
        _params_unsaved.reset();
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
        utarray_sort(_utarray_param_values, utarray_param_compare);

        _params_changed.set(idx, true);

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
    _params_changed.set(idx, true);
    _params_unsaved.set(idx, !mark_saved);


    LOG_D("set param %s", param_get_name(idx));

    ret = 0;

__exit:
    param_unlock_writer();

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
        return -RT_ERROR;
    }

    param_wbuf_s *s = nullptr;

    param_lock_writer();

    /* look for a saved value */
    s = param_find_changed(idx);
    /* if we found one, erase it */
    if (s) {
        int pos = utarray_eltidx(_utarray_param_values, s);
        utarray_erase(_utarray_param_values, pos, 1);
    }

    _params_changed.set(idx, false);
    _params_unsaved.set(idx, true);

    param_unlock_writer();

    if (s != nullptr && notify) {
        param_notify_changes();
        param_notify_autosave();
    }

    return RT_EOK;
}

int param_reset(param_t idx) {
    return param_reset_internal(idx, true);
}

int param_reset_no_notification(param_t idx) {
    return param_reset_internal(idx, false);
}

void param_reset_all_internal(bool notify) {
    param_lock_writer();

    if (_utarray_param_values) {
        utarray_free(_utarray_param_values);
    }
    /*mark all unchanged*/
    _params_changed.reset();
    /* mark as reset / deleted */
    _utarray_param_values = nullptr;

    param_unlock_writer();


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
        if (only_changed && !param_find_changed(idx)) {
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

    param_lock_reader();
    for (param_t idx = 0; idx < param_get_count(); idx++) {
        if (!param_value_used(idx) || param_is_volatile(idx)) {
            continue;
        }

        const char *name  = param_get_name(idx);
        const void *value = param_get_value_ptr(idx);

        hash = crc32part((const uint8_t *)name, rt_strlen(name), hash);
        hash = crc32part((const uint8_t *)value, param_get_type_size(idx), hash);
    }
    param_unlock_reader();

    return hash;
}

/////////////////////////////////////////////////////////////
// 参数导入/导出
/////////////////////////////////////////////////////////////

#include "param_private.h"
#define PARAM_MAX_DEV_COUNT 2
static param_storage_t *__param_storage__[PARAM_MAX_DEV_COUNT] = {NULL, NULL};

int param_storage_register(param_storage_t *dev) {
    if (!dev) {
        return -RT_ERROR;
    }

    bool success = false;
    for (int i = 0; i < PARAM_MAX_DEV_COUNT; i++) {
        if (__param_storage__[i] == NULL) {
            __param_storage__[i] = dev;
            success              = true;
            break;
        }
    }

    return success ? 0 : -RT_ERROR;
}

int param_export_internal(const char *devname, param_filter_func filter) {
    if (!_utarray_param_values) {
        return -RT_ERROR;
    }

    param_storage_t *dev = __param_storage__[0];

    if (!dev) {
        return -RT_ERROR;
    }

    // 先打开设备
    if (dev->ops->open(dev, devname, 'w') != 0) {
        // LOG_E("open %s fail", devname);
        return -RT_ERROR;
    }

    param_lock_reader();

    // 写入所有修改的参数
    param_payload_t payload;
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
        _params_unsaved.set(s->index, false);
        // 计算crc32
        check = crc32part(&payload, sizeof(payload), check);


        if (payload.type == PARAM_TYPE_INT32) {
            LOG_D("export param %s, %s, %d", payload.name, param_type_cstr(payload.type), payload.value.i32);
        } else if (payload.type == PARAM_TYPE_FLOAT) {
            LOG_D("export param %s, %s, %g", payload.name, param_type_cstr(payload.type), payload.value.f32);
        }

        count++;
    }
    param_unlock_reader();

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
        return -RT_ERROR;
    }

    // 先打开设备
    if (dev->ops->open(dev, devname, 'r') != 0) {
        // LOG_E("open %s fail", devname);
        return -RT_ERROR;
    }

    int             ret = -RT_ERROR;
    param_header_t  header;
    param_payload_t payload;
    uint32_t        check = 0;

    // 读取头部信息
    if (dev->ops->read(dev, 0, &header, sizeof(header)) != sizeof(header)) {
        LOG_E("read header fail");
        goto __exit;
    }
    // 对比magic魔术字
    if (rt_strncmp(header.magic, "nextpilot", sizeof(header.magic)) != 0 || // magic不对
        header.utc < 1514764800LL ||                                        // 时间戳不对
        header.count == 0) {                                                // 长度为0
        LOG_D("check header fail");
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
        param_t pp = param_find_no_notification(payload.name);
        if (pp == PARAM_INVALID || param_get_type(pp) != payload.type) {
            LOG_W("ignoring unrecognised param %s", payload.name);
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
    // param_notify_autosave();
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

static int param_init() {
    // 初始化读写锁信号量
    rt_sem_init(&_param_sem, "param_write_lock", 1, RT_IPC_FLAG_FIFO);
    rt_sem_init(&_reader_lock_holders_lock, "param_read_lock", 1, RT_IPC_FLAG_FIFO);

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

    _param_autosave_thread = rt_thread_create("param_autosave", param_autosave_entry, NULL, 2048, 20, 5);

    if (!_param_autosave_thread) {
        LOG_E("create autosave thread fail");
        return -RT_ERROR;
    }

    if (rt_thread_startup(_param_autosave_thread) != 0) {
        rt_thread_delete(_param_autosave_thread);
        LOG_E("startup autosave thread fail");
        return -RT_ERROR;
    }

    return 0;
}

INIT_EXPORT(param_init, "5.2");
