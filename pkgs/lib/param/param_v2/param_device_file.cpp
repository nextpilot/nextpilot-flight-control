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
#define LOG_LVL LOG_LVL_INFO

#include <string.h>
#include <rtthread.h>
#include <rtdbg.h>
#include <fcntl.h>  // open
#include <unistd.h> // read/write/lseek
#include "param_device_file.h"

#define SEEK_SET 0

static char *_param_default_file = nullptr;
static char *_param_backup_file  = nullptr;

const char *param_get_default_file() {
    if (_param_default_file) {
        return _param_default_file;
    } else {
        return "/param.bin";
    }
}

int param_set_default_file(const char *filename) {
    if ((_param_backup_file && rt_strcmp(filename, _param_backup_file) == 0)) {
        LOG_E("default file can't be the same as the backup file %s", filename);
        return -RT_ERROR;
    }

    if (_param_default_file != nullptr) {
        // we assume this is not in use by some other thread
        rt_free(_param_default_file);
        _param_default_file = nullptr;
    }

    if (filename) {
        _param_default_file = rt_strdup(filename);
    }

    return RT_EOK;
}

int param_set_backup_file(const char *filename) {
    if (_param_default_file && rt_strcmp(filename, _param_default_file) == 0) {
        LOG_E("backup file can't be the same as the default file %s", filename);
        return -RT_ERROR;
    }

    if (_param_backup_file != nullptr) {
        // we assume this is not in use by some other thread
        rt_free(_param_backup_file);
        _param_backup_file = nullptr;
    }

    if (filename) {
        _param_backup_file = rt_strdup(filename);

    } else {
        _param_backup_file = nullptr; // backup disabled
    }

    return RT_EOK;
}

const char *param_get_backup_file() {
    return _param_backup_file;
}

static int _init() {
    return 0;
}

static int _open(param_device_t *dev, const char *name, uint8_t flag) {
    if (!dev) {
        return -1;
    }

    if (!name) {
        name = param_get_default_file();
    }

    if (flag == 'r') {
        if (access(name, 0) != F_OK) {
            LOG_W("access %s fail", name);
            return -1;
        }
        dev->fid = open(name, O_RDONLY);
    } else {
        dev->fid = open(name, O_WRONLY | O_CREAT | O_TRUNC);
    }

    if (dev->fid < 0) {
        LOG_W("open %s fail", name);
        return -1;
    }

    dev->name = name;

    LOG_D("select file %s ", name);

    return 0;
}

static int _read(param_device_t *dev, int32_t offset, void *buff, uint32_t size) {
    if (!dev || !buff) {
        return -1;
    }

    if (lseek(dev->fid, offset, SEEK_SET) != offset) {
        return -1;
    }

    return read(dev->fid, buff, size);
}

static int _write(param_device_t *dev, int32_t offset, void *buff, uint32_t size) {
    if (!dev || !buff) {
        return -1;
    }

    if (lseek(dev->fid, offset, SEEK_SET) != offset) {
        return -1;
    }

    return write(dev->fid, buff, size);
}

static int _close(param_device_t *dev) {
    if (!dev) {
        return -1;
    }

    return close(dev->fid);
}

static param_device_ops_t _ops = {
    .init  = _init,
    .open  = _open,
    .read  = _read,
    .write = _write,
    .close = _close,
};

param_device_t _param_file_device = {
    .name = "file",
    .type = 1,
    .ops  = &_ops,
};
