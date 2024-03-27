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
    int (*open)(param_storage_t *dev, const char *name);
    int (*read)(param_storage_t *dev, int32_t offset, void *buff, uint32_t size);
    int (*write)(param_storage_t *dev, int32_t offset, void *buff, uint32_t size);
    int (*close)(param_storage_t *dev);
} param_storage_ops_t;

struct param_storage_s {
    const char *name;
    uint8_t     type;
    union {
        int         fid;
        rt_device_t dev;
    };
    param_storage_ops_t *ops;
};

#endif // __PARAM_STORAGE_H__
