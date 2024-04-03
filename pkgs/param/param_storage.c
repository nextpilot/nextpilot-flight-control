/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "param.storage"

#include <stdbool.h>
#include <rtdbg.h>
#include <fcntl.h>
#include "param_storage.h"
#include "param_interface.h"

#define MAX_DEV_COUNT 2
static param_storage_t *__param_storage__[MAX_DEV_COUNT] = {NULL, NULL};

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
    for (int i = 0; i < MAX_DEV_COUNT; i++) {
        if (__param_storage__[i] == NULL) {
            __param_storage__[i] = dev;
            success              = true;
            break;
        }
    }

    return success ? 0 : -1;
}

int param_export_internal(const char *devname, param_filter_func filter) {
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
    rt_memcpy(header.magic, "zhan", sizeof(header.magic));
    dev->ops->write(dev, 0, &header, sizeof(header));

    LOG_D("write header, utc=%u, count=%u, crc=%u", header.utc, count, check);

    // 关闭设备
    dev->ops->close(dev);

    LOG_I("export all params ok");

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

    // 读取头部信息
    param_header_t header;
    if (dev->ops->read(dev, 0, &header, sizeof(header)) != sizeof(header)) {
        LOG_E("read header fail");
        goto __exit;
    }

    if (rt_strncmp(header.magic, "zhan", 4) != 0 || // magic不对
        header.utc < 1514764800LL ||                // 时间戳不对
        header.count == 0) {                        // 长度为0
        goto __exit;
    }
    LOG_D("read header, utc=%u, count=%u, crc=%u", header.utc, header.count, header.check);

    // 计算存储param的check
    param_payload_t payload;
    uint32_t        check = 0;
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
        param_set_internal(idx, &payload.value, true, false);
        // 打印调试信息
        if (payload.type == PARAM_TYPE_INT32) {
            LOG_D("import param %s, %s, %d", payload.name, param_type_cstr(payload.type), payload.value.i32);
        } else if (payload.type == PARAM_TYPE_FLOAT) {
            LOG_D("import param %s, %s, %g", payload.name, param_type_cstr(payload.type), payload.value.f32);
        }
    }

    // 关闭设备
    dev->ops->close(dev);

    LOG_I("import all params ok");

    // 提醒参数已经更新
    param_notify_autosave();
    param_notify_changes();

    return 0;

__exit:

    dev->ops->close(dev);
    return -1;
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
            if (delta < 10000UL) {
                rt_thread_mdelay(10000 - delta);
            }

            // 保存参数到flash或文件
            if (param_save_default() == 0) {
                last_autosave_timestamp = rt_tick_get();
                LOG_D("param autosave ok");
            } else {
                LOG_E("param autosave fail");
            }
        }
    }
}

static int param_autosave_init() {
    // 从默认设备加载param
    if (param_load_default() == 0) {
        LOG_I("load default ok");
    }

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

INIT_APP_EXPORT(param_autosave_init);
