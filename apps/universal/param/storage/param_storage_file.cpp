/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <string.h>
#include <rtthread.h>
#include <rtdbg.h>
#include <fcntl.h>  // open
#include <unistd.h> // read/write/lseek
#include "param_storage.h"

static char *param_default_file = nullptr;
static char *param_backup_file  = nullptr;

const char *param_get_default_file() {
    return param_default_file;
}

int param_set_backup_file(const char *filename) {
    if (param_default_file && strcmp(filename, param_default_file) == 0) {
        LOG_E("backup file can't be the same as the default file %s", filename);
        return -1;
    }

    if (param_backup_file != nullptr) {
        // we assume this is not in use by some other thread
        rt_free(param_backup_file);
        param_backup_file = nullptr;
    }

    if (filename) {
        param_backup_file = rt_strdup(filename);

    } else {
        param_backup_file = nullptr; // backup disabled
    }

    return 0;
}

const char *param_get_backup_file() {
    return param_backup_file;
}

static int _init() {
    return 0;
}

static int _open(param_storage_t *dev, const char *name) {
    if (!dev || !name) {
        return -1;
    }

    dev->fid = open(name, O_RDONLY);
    if (dev->fid < 0) {
        return -1;
    }

    return 0;
}

static int _read(param_storage_t *dev, int32_t offset, void *buff, uint32_t size) {
    if (!dev || !buff) {
        return -1;
    }

    return read(dev->fid, buff, size);
}

static int _write(param_storage_t *dev, int32_t offset, void *buff, uint32_t size) {
    if (!dev || !buff) {
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

param_storage_ops_t _file_ops = {
    .init  = NULL,
    .open  = _open,
    .read  = _read,
    .write = _write,
    .close = _close,
};
