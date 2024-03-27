/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "param.file"

#include <stdbool.h>
#include <rtdbg.h>
#include "param_storage.h"
#include "param_interface.h"

#define MAX_DEV_COUNT 2
static param_storage_t *__param_storage__[MAX_DEV_COUNT] = {NULL, NULL};

RT_WEAK uint32_t crc32part(const void *buff, int size, uint32_t crc) {
    return crc;
}

int param_export_internal(const char *devname, param_filter_func filter) {
    param_storage_t *dev = __param_storage__[0];

    // 先打开设备
    if (dev->ops->open(dev, devname) != 0) {
        LOG_E("open %s fail", devname);
        return -1;
    }

    return 0;
}

int param_import_internal(const char *devname, param_filter_func filter) {
    param_storage_t *dev = __param_storage__[0];

    // 先打开设备
    if (dev->ops->open(dev, devname) != 0) {
        LOG_E("open %s fail", devname);
        return -1;
    }

    // 读取头部信息
    param_header_t header;
    if (dev->ops->read(dev, 0, &header, sizeof(header)) != sizeof(header)) {
        LOG_E("read header fail");
        goto __exit;
    }

    // 判断header是否正确
    if (rt_strncmp(header.magic, "zhan", 4) != 0 || // magic不对
        header.utc < 123456789LL ||                 // 时间戳不对
        header.count == 0) {                        // 长度为0
        goto __exit;
    }

    // 计算存储param的check
    param_payload_t payload;
    uint32_t        check = crc32part((const uint8_t *)&header.magic, sizeof(header) - 4, 0);
    for (uint16_t idx = 0; idx < header.count; idx++) {
        dev->ops->read(dev, 0, &payload, sizeof(payload));
        check = crc32part((const uint8_t *)&payload, sizeof(payload), check);
    }
    if (check != header.check) {
        LOG_W("check crc fail");
        goto __exit;
    }

    // 重新读取param
    for (uint16_t idx = 0; idx < header.count; idx++) {
        dev->ops->read(dev, 0, &payload, sizeof(payload));
        //
        param_t pp = param_find_internal(payload.name, false);
        if (pp == PARAM_INVALID) {
            continue;
        }

        param_set_internal(idx, &payload.value, true, false);

        LOG_D("import %s : %d", payload.name, 0);
    }

    dev->ops->close(dev);
    param_notify_changes();

    return 0;

__exit:

    dev->ops->close(dev);
    return -1;
}

int param_reset_and_import(const char *devname) {
    // 如果devname=NULL表示使用默认文件路径

    // 先重置所有参数，但是不发送param_update
    param_reset_all_notification();

    // 然后从设备中加载参数，并发送param_update
    return param_import_internal(devname, NULL);
}

int param_load_default() {
    param_reset_and_import(NULL);
}

int param_save_default() {
    return param_export_internal(NULL, NULL);
}

static bool        _param_autosave_enable = true;
static rt_event_t  _param_autosave_event  = NULL;
static rt_thread_t _param_autosave_thread = NULL;
#define PARAM_EVENT_AUTOSAVE 0

void param_notify_autosave() {
    if (_param_autosave_enable && _param_autosave_event) {
        rt_event_send(_param_autosave_event, PARAM_EVENT_AUTOSAVE);
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
        int ret = rt_event_recv(_param_autosave_event, PARAM_EVENT_AUTOSAVE, 0, RT_WAITING_FOREVER, NULL);

        // 每次保存间隔时间要大于3s
        if (_param_autosave_enable &&
            (rt_tick_get() - last_autosave_timestamp >= 3000UL)) {
            // 保存参数到flash或文件
            if (param_save_default() == 0) {
                last_autosave_timestamp = rt_tick_get();
            } else {
                LOG_E("param autosave fail");
            }
        }
    }
}

static int param_autosave_init() {
    // 从默认设备加载param
    param_load_default();

    _param_autosave_event = rt_event_create("param_autosave", RT_IPC_FLAG_PRIO);
    if (!_param_autosave_event) {
        return 0;
    }

    _param_autosave_thread = rt_thread_create("param_autosave", param_autosave_entry, NULL, 1024, 20, 5);

    if (!_param_autosave_thread) {
        return -1;
    }

    if (rt_thread_startup(_param_autosave_thread) != 0) {
        rt_thread_delete(_param_autosave_thread);
        return -1;
    }

    return 0;
}

INIT_APP_EXPORT(param_autosave_init);
