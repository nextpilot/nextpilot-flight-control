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
#include "param_storage.h"

#define SEEK_SET 0

static char *param_default_file = NULL;
static char *param_backup_file  = NULL;

const char *param_get_default_file() {
    if (param_default_file) {
        return param_default_file;
    } else {
        return "/param.bin";
    }
}

int param_set_default_file(const char *filename) {
    if ((param_backup_file && rt_strcmp(filename, param_backup_file) == 0)) {
        LOG_E("default file can't be the same as the backup file %s", filename);
        return -1;
    }

    if (param_default_file != NULL) {
        // we assume this is not in use by some other thread
        rt_free(param_default_file);
        param_default_file = NULL;
    }

    if (filename) {
        param_default_file = rt_strdup(filename);
    }

    return 0;
}

int param_set_backup_file(const char *filename) {
    if (param_default_file && strcmp(filename, param_default_file) == 0) {
        LOG_E("backup file can't be the same as the default file %s", filename);
        return -1;
    }

    if (param_backup_file != NULL) {
        // we assume this is not in use by some other thread
        rt_free(param_backup_file);
        param_backup_file = NULL;
    }

    if (filename) {
        param_backup_file = rt_strdup(filename);

    } else {
        param_backup_file = NULL; // backup disabled
    }

    return 0;
}

const char *param_get_backup_file() {
    return param_backup_file;
}

static int _init() {
    return 0;
}

static int _open(param_storage_t *dev, const char *name, uint8_t flag) {
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

static int _read(param_storage_t *dev, int32_t offset, void *buff, uint32_t size) {
    if (!dev || !buff) {
        return -1;
    }

    if (lseek(dev->fid, offset, SEEK_SET) != offset) {
        return -1;
    }

    return read(dev->fid, buff, size);
}

static int _write(param_storage_t *dev, int32_t offset, void *buff, uint32_t size) {
    if (!dev || !buff) {
        return -1;
    }

    if (lseek(dev->fid, offset, SEEK_SET) != offset) {
        return -1;
    }

    return write(dev->fid, buff, size);
}

static int _close(param_storage_t *dev) {
    if (!dev) {
        return -1;
    }

    return close(dev->fid);
}

static param_storage_ops_t _ops = {
    .init  = _init,
    .open  = _open,
    .read  = _read,
    .write = _write,
    .close = _close,
};

static param_storage_t _dev = {
    .name = "",
    .type = 1,
    .ops  = &_ops,
};

static int param_storage_file_init() {
    return param_storage_register(&_dev);
}

INIT_COMPONENT_EXPORT(param_storage_file_init);
