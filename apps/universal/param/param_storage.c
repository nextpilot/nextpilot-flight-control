/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/
#include <stdbool.h>
#include <rtdbg.h>
#include "param_storage.h"
#include "param_interface.h"

static param_storage_t *__param_storage__[3] = {NULL, NULL, NULL};

RT_WEAK uint32_t crc32part(const void *buff, int size, uint32_t crc) {
    return crc;
}

int param_storage_register(param_storage_t *dev) {
    if (!dev) {
        return -1;
    }

    bool success = false;

    for (int i = 0; i < sizeof(__param_storage__) / sizeof(param_storage_t); i++) {
        if (__param_storage__[i] == NULL) {
            __param_storage__[i] = dev;
            success              = true;
            break;
        }
    }

    return success ? 0 : -1;
}

// int param_export_internal(param_filter_func filter) {
// }

int param_import_internal(const char *devname) {
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
