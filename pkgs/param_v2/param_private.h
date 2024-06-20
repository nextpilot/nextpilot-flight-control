/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __PARAM_PRIVATE_H__
#define __PARAM_PRIVATE_H__


#include <param_type.h>
#include <stdint.h>
#include <rtdevice.h>

// param 访问接口

typedef struct param_interface_ops_s {
    int (*init)();
    param_t (*find)(const char *name);
    uint16_t (*get_count)();
    int (*get_info)(param_t idx, param_info_t *info);
    int (*get_value)(param_t idx, param_value_t *val);
    const void *(*get_value_ptr)(param_t idx);
    int (*set_value)(param_t idx, const param_value_t *val);
    int (*get_status)(param_t idx, param_status_t *status);
    int (*set_status)(param_t idx, const param_status_t *status);
    int (*reset)(param_t idx);
} param_interface_ops_t;

typedef struct param_interface_s {
    const char            *name;
    uint8_t                type;
    param_interface_ops_t *ops;
} param_interface_t;

int param_interface_register(param_interface_t *api);

// param 存储接口

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

typedef struct {
    uint32_t check;
    char     magic[9]; // "nextpilot"
    uint64_t utc;      // 保存时间戳
    uint16_t count;    // 参数个数
} param_header_t;

typedef struct {
    char          name[16];
    param_type_t  type;
    param_value_t value;
} param_payload_t;

int param_storage_register(param_storage_t *dev);


#endif // __PARAM_PRIVATE_H__
