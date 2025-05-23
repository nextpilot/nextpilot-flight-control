/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/
/**
 * @file dataman.cpp
 * DATAMANAGER driver.
 *
 * @author Jean Cyr
 * @author Lorenz Meier
 * @author Julian Oes
 * @author Thomas Gubler
 * @author David Sidrane
 */

#define LOG_TAG "dataman"
#define LOG_LVL LOG_LVL_INFO


#include <defines.h>
#include <module/module_command.hpp>
#include <getopt/getopt.h>
#include <hrtimer.h>
#include <param/param.h>
#include <perf/perf_counter.h>
#include <stdlib.h>
#include <dfs_file.h>
#include <unistd.h>
#include "dataman.h"

__BEGIN_DECLS
__EXPORT int dataman_main(int argc, char *argv[]);
__END_DECLS

static constexpr int TASK_STACK_SIZE = 1320;

/* Private File based Operations */
static ssize_t _file_write(dm_item_t item, unsigned index, const void *buf, size_t count);
static ssize_t _file_read(dm_item_t item, unsigned index, void *buf, size_t count);
static int     _file_clear(dm_item_t item);
static int     _file_initialize(unsigned max_offset);
static void    _file_shutdown();

/* Private Ram based Operations */
static ssize_t _ram_write(dm_item_t item, unsigned index, const void *buf, size_t count);
static ssize_t _ram_read(dm_item_t item, unsigned index, void *buf, size_t count);
static int     _ram_clear(dm_item_t item);
static int     _ram_initialize(unsigned max_offset);
static void    _ram_shutdown();

typedef struct dm_operations_t {
    ssize_t (*write)(dm_item_t item, unsigned index, const void *buf, size_t count);
    ssize_t (*read)(dm_item_t item, unsigned index, void *buf, size_t count);
    int (*clear)(dm_item_t item);
    int (*initialize)(unsigned max_offset);
    void (*shutdown)();
    rt_err_t (*wait)(struct rt_semaphore *sem, rt_int32_t time);
} dm_operations_t;

static constexpr dm_operations_t dm_file_operations = {
    .write      = _file_write,
    .read       = _file_read,
    .clear      = _file_clear,
    .initialize = _file_initialize,
    .shutdown   = _file_shutdown,
    .wait       = rt_sem_take,
};

static constexpr dm_operations_t dm_ram_operations = {
    .write      = _ram_write,
    .read       = _ram_read,
    .clear      = _ram_clear,
    .initialize = _ram_initialize,
    .shutdown   = _ram_shutdown,
    .wait       = rt_sem_take,
};

static const dm_operations_t *g_dm_ops;

static struct {
    union {
        struct {
            int fd;
        } file;

        struct {
            uint8_t *data;
            uint8_t *data_end;
        } ram;
    };

    bool running;
    bool silence = false;
} dm_operations_data;

/** Types of function calls supported by the worker task */
typedef enum {
    dm_write_func = 0,
    dm_read_func,
    dm_clear_func,
    dm_number_of_funcs
} dm_function_t;

/** Work task work item */
typedef struct {
    sq_entry_t          link; /**< list linkage */
    struct rt_semaphore wait_sem;
    unsigned char       first;
    unsigned char       func;
    ssize_t             result;

    union {
        struct {
            dm_item_t   item;
            unsigned    index;
            const void *buf;
            size_t      count;
        } write_params;

        struct {
            dm_item_t item;
            unsigned  index;
            void     *buf;
            size_t    count;
        } read_params;

        struct {
            dm_item_t item;
        } clear_params;
    };
} work_q_item_t;

const size_t k_work_item_allocation_chunk_size = 8;

/* Usage statistics */
static unsigned g_func_counts[dm_number_of_funcs];

/* table of maximum number of instances for each item type */
static const unsigned g_per_item_max_index[DM_KEY_NUM_KEYS] = {
    DM_KEY_SAFE_POINTS_MAX,
    DM_KEY_FENCE_POINTS_MAX,
    DM_KEY_WAYPOINTS_OFFBOARD_0_MAX,
    DM_KEY_WAYPOINTS_OFFBOARD_1_MAX,
    DM_KEY_MISSION_STATE_MAX,
    DM_KEY_COMPAT_MAX};

#define DM_SECTOR_HDR_SIZE 4 /* data manager per item header overhead */

/* Table of the len of each item type */
static constexpr size_t g_per_item_size[DM_KEY_NUM_KEYS] = {
    sizeof(struct mission_safe_point_s) + DM_SECTOR_HDR_SIZE,
    sizeof(struct mission_fence_point_s) + DM_SECTOR_HDR_SIZE,
    sizeof(struct mission_item_s) + DM_SECTOR_HDR_SIZE,
    sizeof(struct mission_item_s) + DM_SECTOR_HDR_SIZE,
    sizeof(struct mission_s) + DM_SECTOR_HDR_SIZE,
    sizeof(struct dataman_compat_s) + DM_SECTOR_HDR_SIZE};

/* Table of offset for index 0 of each item type */
static unsigned int g_key_offsets[DM_KEY_NUM_KEYS];

/* Item type lock mutexes */
static struct rt_semaphore *g_item_locks[DM_KEY_NUM_KEYS];
static struct rt_semaphore  g_sys_state_mutex_mission;
static struct rt_semaphore  g_sys_state_mutex_fence;

static perf_counter_t _dm_read_perf{nullptr};
static perf_counter_t _dm_write_perf{nullptr};

/* The data manager store file handle and file name */
static const char *default_device_path        = PX4_STORAGEDIR "/dataman.bin";
static char       *k_data_manager_device_path = nullptr;

static enum {
    BACKEND_NONE = 0,
    BACKEND_FILE,
    BACKEND_RAM,
    BACKEND_LAST
} backend = BACKEND_NONE;

/* The data manager work queues */

typedef struct {
    sq_queue_t          q;        /* Nuttx queue */
    struct rt_semaphore mutex;    /* Mutual exclusion on work queue adds and deletes */
    unsigned            size;     /* Current size of queue */
    unsigned            max_size; /* Maximum queue size reached */
} work_q_t;

static work_q_t g_free_q;                      /* queue of free work items. So that we don't always need to call malloc and free*/
static work_q_t g_work_q;                      /* pending work items. To be consumed by worker thread */

static struct rt_semaphore g_work_queued_sema; /* To notify worker thread a work item has been queued */
static struct rt_semaphore g_init_sema;

static bool g_task_should_exit; /**< if true, dataman task should exit */

static void init_q(work_q_t *q) {
    sq_init(&(q->q));                                         /* Initialize the NuttX queue structure */
    rt_sem_init(&(q->mutex), "q_mutex", 1, RT_IPC_FLAG_PRIO); /* Queue is initially unlocked */
    q->size = q->max_size = 0;                                /* Queue is initially empty */
}

static inline void
destroy_q(work_q_t *q) {
    rt_sem_detach(&(q->mutex)); /* Destroy the queue lock */
}

static inline void
lock_queue(work_q_t *q) {
    rt_sem_take(&(q->mutex), RT_WAITING_FOREVER); /* Acquire the queue lock */
}

static inline void
unlock_queue(work_q_t *q) {
    rt_sem_release(&(q->mutex)); /* Release the queue lock */
}

static work_q_item_t *
create_work_item() {
    work_q_item_t *item;

    /* Try to reuse item from free item queue */
    lock_queue(&g_free_q);

    if ((item = (work_q_item_t *)sq_remfirst(&(g_free_q.q)))) {
        g_free_q.size--;
    }

    unlock_queue(&g_free_q);

    /* If we there weren't any free items then obtain memory for a new ones */
    if (item == nullptr) {
        item = (work_q_item_t *)malloc(k_work_item_allocation_chunk_size * sizeof(work_q_item_t));

        if (item) {
            item->first = 1;
            lock_queue(&g_free_q);

            for (size_t i = 1; i < k_work_item_allocation_chunk_size; i++) {
                (item + i)->first = 0;
                sq_addfirst(&(item + i)->link, &(g_free_q.q));
            }

            /* Update the queue size and potentially the maximum queue size */
            g_free_q.size += k_work_item_allocation_chunk_size - 1;

            if (g_free_q.size > g_free_q.max_size) {
                g_free_q.max_size = g_free_q.size;
            }

            unlock_queue(&g_free_q);
        }
    }

    /* If we got one then lock the item*/
    if (item) {
        rt_sem_init(&item->wait_sem, "item_wait", 0, RT_IPC_FLAG_PRIO); /* Caller will wait on this... initially locked */

        /* item->wait_sem use case is a signal */

        // px4_sem_setprotocol(&item->wait_sem, SEM_PRIO_NONE);
    }

    /* return the item pointer, or nullptr if all failed */
    return item;
}

/* Work queue management functions */

static inline void
destroy_work_item(work_q_item_t *item) {
    rt_sem_detach(&item->wait_sem); /* Destroy the item lock */
    /* Return the item to the free item queue for later reuse */
    lock_queue(&g_free_q);
    sq_addfirst(&item->link, &(g_free_q.q));

    /* Update the queue size and potentially the maximum queue size */
    if (++g_free_q.size > g_free_q.max_size) {
        g_free_q.max_size = g_free_q.size;
    }

    unlock_queue(&g_free_q);
}

static inline work_q_item_t *
dequeue_work_item() {
    work_q_item_t *work;

    /* retrieve the 1st item on the work queue */
    lock_queue(&g_work_q);

    if ((work = (work_q_item_t *)sq_remfirst(&g_work_q.q))) {
        g_work_q.size--;
    }

    unlock_queue(&g_work_q);
    return work;
}

static int
enqueue_work_item_and_wait_for_result(work_q_item_t *item) {
    /* put the work item at the end of the work queue */
    lock_queue(&g_work_q);
    sq_addlast(&item->link, &(g_work_q.q));

    /* Adjust the queue size and potentially the maximum queue size */
    if (++g_work_q.size > g_work_q.max_size) {
        g_work_q.max_size = g_work_q.size;
    }

    unlock_queue(&g_work_q);

    /* tell the work thread that work is available */
    rt_sem_release(&g_work_queued_sema);

    /* wait for the result */
    rt_sem_take(&item->wait_sem, RT_WAITING_FOREVER);

    int result = item->result;

    destroy_work_item(item);

    return result;
}

static bool is_running() {
    return dm_operations_data.running;
}

/* Calculate the offset in file of specific item */
static int
calculate_offset(dm_item_t item, unsigned index) {
    /* Make sure the item type is valid */
    if (item >= DM_KEY_NUM_KEYS) {
        return -1;
    }

    /* Make sure the index for this item type is valid */
    if (index >= g_per_item_max_index[item]) {
        return -1;
    }

    /* Calculate and return the item index based on type and index */
    return g_key_offsets[item] + (index * g_per_item_size[item]);
}

/* Each data item is stored as follows
 *
 * byte 0: Length of user data item
 * byte 1: Unused (previously persistence of this data item)
 * byte 2: Unused (for future use)
 * byte 3: Unused (for future use)
 * byte DM_SECTOR_HDR_SIZE... : data item value
 *
 * The total size must not exceed g_per_item_max_index[item]
 */

/* write to the data manager RAM buffer  */
static ssize_t _ram_write(dm_item_t item, unsigned index, const void *buf, size_t count) {
    /* Get the offset for this item */
    int offset = calculate_offset(item, index);

    /* If item type or index out of range, return error */
    if (offset < 0) {
        return -1;
    }

    /* Make sure caller has not given us more data than we can handle */
    if (count > (g_per_item_size[item] - DM_SECTOR_HDR_SIZE)) {
        return -E2BIG;
    }

    uint8_t *buffer = &dm_operations_data.ram.data[offset];

    if (buffer > dm_operations_data.ram.data_end) {
        return -1;
    }

    /* Write out the data, prefixed with length */
    buffer[0] = count;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = 0;

    if (count > 0) {
        memcpy(buffer + DM_SECTOR_HDR_SIZE, buf, count);
    }

    /* All is well... return the number of user data written */
    return count;
}

/* write to the data manager file */
static ssize_t
_file_write(dm_item_t item, unsigned index, const void *buf, size_t count) {
    unsigned char buffer[g_per_item_size[item]];

    /* Get the offset for this item */
    const int offset = calculate_offset(item, index);

    /* If item type or index out of range, return error */
    if (offset < 0) {
        return -1;
    }

    /* Make sure caller has not given us more data than we can handle */
    if (count > (g_per_item_size[item] - DM_SECTOR_HDR_SIZE)) {
        return -E2BIG;
    }

    /* Write out the data, prefixed with length */
    buffer[0] = count;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = 0;

    if (count > 0) {
        memcpy(buffer + DM_SECTOR_HDR_SIZE, buf, count);
    }

    count += DM_SECTOR_HDR_SIZE;

    bool write_success = false;

    for (int i = 0; i < 2; i++) {
        int ret_seek = lseek(dm_operations_data.file.fd, offset, SEEK_SET);

        if (ret_seek < 0) {
            PX4_ERR("file write lseek failed %d", errno);
            continue;
        }

        if (ret_seek != offset) {
            PX4_ERR("file write lseek failed, incorrect offset %d vs %d", ret_seek, offset);
            continue;
        }

        int ret_write = write(dm_operations_data.file.fd, buffer, count);

        if (ret_write < 0) {
            PX4_ERR("file write failed %d", errno);
            continue;
        }

        if (ret_write != (ssize_t)count) {
            PX4_ERR("file write failed, wrote %d bytes, expected %zu", ret_write, count);
            continue;

        } else {
            write_success = true;
            break;
        }
    }

    if (!write_success) {
        return -1;
    }

    /* Make sure data is written to physical media */
    fsync(dm_operations_data.file.fd);

    /* All is well... return the number of user data written */
    return count - DM_SECTOR_HDR_SIZE;
}

/* Retrieve from the data manager RAM buffer*/
static ssize_t _ram_read(dm_item_t item, unsigned index, void *buf, size_t count) {
    /* Get the offset for this item */
    int offset = calculate_offset(item, index);

    /* If item type or index out of range, return error */
    if (offset < 0) {
        return -1;
    }

    /* Make sure the caller hasn't asked for more data than we can handle */
    if (count > (g_per_item_size[item] - DM_SECTOR_HDR_SIZE)) {
        return -E2BIG;
    }

    /* Read the prefix and data */

    uint8_t *buffer = &dm_operations_data.ram.data[offset];

    if (buffer > dm_operations_data.ram.data_end) {
        return -1;
    }

    /* See if we got data */
    if (buffer[0] > 0) {
        /* We got more than requested!!! */
        if (buffer[0] > count) {
            return -1;
        }

        /* Looks good, copy it to the caller's buffer */
        memcpy(buf, buffer + DM_SECTOR_HDR_SIZE, buffer[0]);
    }

    /* Return the number of bytes of caller data read */
    return buffer[0];
}

/* Retrieve from the data manager file */
static ssize_t
_file_read(dm_item_t item, unsigned index, void *buf, size_t count) {
    if (item >= DM_KEY_NUM_KEYS) {
        return -1;
    }

    unsigned char buffer[g_per_item_size[item]];

    /* Get the offset for this item */
    int offset = calculate_offset(item, index);

    /* If item type or index out of range, return error */
    if (offset < 0) {
        return -1;
    }

    /* Make sure the caller hasn't asked for more data than we can handle */
    if (count > (g_per_item_size[item] - DM_SECTOR_HDR_SIZE)) {
        return -E2BIG;
    }

    int  len          = -1;
    bool read_success = false;

    for (int i = 0; i < 2; i++) {
        int ret_seek = lseek(dm_operations_data.file.fd, offset, SEEK_SET);

        if ((ret_seek < 0) && !dm_operations_data.silence) {
            PX4_ERR("file read lseek failed %d", errno);
            continue;
        }

        if ((ret_seek != offset) && !dm_operations_data.silence) {
            PX4_ERR("file read lseek failed, incorrect offset %d vs %d", ret_seek, offset);
            continue;
        }

        /* Read the prefix and data */
        len = read(dm_operations_data.file.fd, buffer, count + DM_SECTOR_HDR_SIZE);

        /* Check for read error */
        if (len >= 0) {
            read_success = true;
            break;

        } else {
            if (!dm_operations_data.silence) {
                PX4_ERR("file read failed %d", errno);
            }
        }
    }

    if (!read_success) {
        return -1;
    }

    /* A zero length entry is a empty entry */
    if (len == 0) {
        buffer[0] = 0;
    }

    /* See if we got data */
    if (buffer[0] > 0) {
        /* We got more than requested!!! */
        if (buffer[0] > count) {
            return -1;
        }

        /* Looks good, copy it to the caller's buffer */
        memcpy(buf, buffer + DM_SECTOR_HDR_SIZE, buffer[0]);
    }

    /* Return the number of bytes of caller data read */
    return buffer[0];
}

static int _ram_clear(dm_item_t item) {
    int i;
    int result = 0;

    /* Get the offset of 1st item of this type */
    int offset = calculate_offset(item, 0);

    /* Check for item type out of range */
    if (offset < 0) {
        return -1;
    }

    /* Clear all items of this type */
    for (i = 0; (unsigned)i < g_per_item_max_index[item]; i++) {
        uint8_t *buf = &dm_operations_data.ram.data[offset];

        if (buf > dm_operations_data.ram.data_end) {
            result = -1;
            break;
        }

        buf[0]  = 0;
        offset += g_per_item_size[item];
    }

    return result;
}

static int
_file_clear(dm_item_t item) {
    int i, result = 0;

    /* Get the offset of 1st item of this type */
    int offset = calculate_offset(item, 0);

    /* Check for item type out of range */
    if (offset < 0) {
        return -1;
    }

    /* Clear all items of this type */
    for (i = 0; (unsigned)i < g_per_item_max_index[item]; i++) {
        char buf[1];

        if (lseek(dm_operations_data.file.fd, offset, SEEK_SET) != offset) {
            result = -1;
            break;
        }

        /* Avoid SD flash wear by only doing writes where necessary */
        if (read(dm_operations_data.file.fd, buf, 1) < 1) {
            break;
        }

        /* If item has length greater than 0 it needs to be overwritten */
        if (buf[0]) {
            if (lseek(dm_operations_data.file.fd, offset, SEEK_SET) != offset) {
                result = -1;
                break;
            }

            buf[0] = 0;

            if (write(dm_operations_data.file.fd, buf, 1) != 1) {
                result = -1;
                break;
            }
        }

        offset += g_per_item_size[item];
    }

    /* Make sure data is actually written to physical media */
    fsync(dm_operations_data.file.fd);
    return result;
}

static int
_file_initialize(unsigned max_offset) {
    /* See if the data manage file exists and is a multiple of the sector size */
    dm_operations_data.file.fd = open(k_data_manager_device_path, O_RDONLY | O_BINARY);

    if (dm_operations_data.file.fd >= 0) {
        // Read the mission state and check the hash
        struct dataman_compat_s compat_state;
        dm_operations_data.silence = true;
        int ret                    = g_dm_ops->read(DM_KEY_COMPAT, 0, &compat_state, sizeof(compat_state));
        dm_operations_data.silence = false;

        bool incompat = true;

        if (ret == sizeof(compat_state)) {
            if (compat_state.key == DM_COMPAT_KEY) {
                incompat = false;
            }
        }

        close(dm_operations_data.file.fd);

        if (incompat) {
            unlink(k_data_manager_device_path);
        }
    }

    /* Open or create the data manager file */
    dm_operations_data.file.fd = open(k_data_manager_device_path, O_RDWR | O_CREAT | O_BINARY);

    if (dm_operations_data.file.fd < 0) {
        PX4_WARN("Could not open data manager file %s", k_data_manager_device_path);
        rt_sem_release(&g_init_sema); /* Don't want to hang startup */
        return -1;
    }

    if ((unsigned)lseek(dm_operations_data.file.fd, max_offset, SEEK_SET) != max_offset) {
        close(dm_operations_data.file.fd);
        PX4_WARN("Could not seek data manager file %s", k_data_manager_device_path);
        rt_sem_release(&g_init_sema); /* Don't want to hang startup */
        return -1;
    }

    /* Write current compat info */
    struct dataman_compat_s compat_state;
    compat_state.key = DM_COMPAT_KEY;
    int ret          = g_dm_ops->write(DM_KEY_COMPAT, 0, &compat_state, sizeof(compat_state));

    if (ret != sizeof(compat_state)) {
        PX4_ERR("Failed writing compat: %d", ret);
    }

    fsync(dm_operations_data.file.fd);
    dm_operations_data.running = true;

    return 0;
}

static int
_ram_initialize(unsigned max_offset) {
    /* In memory */
    dm_operations_data.ram.data = (uint8_t *)malloc(max_offset);

    if (dm_operations_data.ram.data == nullptr) {
        PX4_WARN("Could not allocate %u bytes of memory", max_offset);
        rt_sem_release(&g_init_sema); /* Don't want to hang startup */
        return -1;
    }

    memset(dm_operations_data.ram.data, 0, max_offset);
    dm_operations_data.ram.data_end = &dm_operations_data.ram.data[max_offset - 1];
    dm_operations_data.running      = true;

    return 0;
}

static void
_file_shutdown() {
    close(dm_operations_data.file.fd);
    dm_operations_data.running = false;
}

static void
_ram_shutdown() {
    free(dm_operations_data.ram.data);
    dm_operations_data.running = false;
}

/** Write to the data manager file */
__EXPORT ssize_t
dm_write(dm_item_t item, unsigned index, const void *buf, size_t count) {
    work_q_item_t *work;

    /* Make sure data manager has been started and is not shutting down */
    if (!is_running() || g_task_should_exit) {
        return -1;
    }

    perf_begin(_dm_write_perf);

    /* get a work item and queue up a write request */
    if ((work = create_work_item()) == nullptr) {
        PX4_ERR("dm_write create_work_item failed");
        perf_end(_dm_write_perf);
        return -1;
    }

    work->func               = dm_write_func;
    work->write_params.item  = item;
    work->write_params.index = index;
    work->write_params.buf   = buf;
    work->write_params.count = count;

    /* Enqueue the item on the work queue and wait for the worker thread to complete processing it */
    ssize_t ret = (ssize_t)enqueue_work_item_and_wait_for_result(work);
    perf_end(_dm_write_perf);
    return ret;
}

/** Retrieve from the data manager file */
__EXPORT ssize_t
dm_read(dm_item_t item, unsigned index, void *buf, size_t count) {
    work_q_item_t *work;

    /* Make sure data manager has been started and is not shutting down */
    if (!is_running() || g_task_should_exit) {
        return -1;
    }

    perf_begin(_dm_read_perf);

    /* get a work item and queue up a read request */
    if ((work = create_work_item()) == nullptr) {
        PX4_ERR("dm_read create_work_item failed");
        perf_end(_dm_read_perf);
        return -1;
    }

    work->func              = dm_read_func;
    work->read_params.item  = item;
    work->read_params.index = index;
    work->read_params.buf   = buf;
    work->read_params.count = count;

    /* Enqueue the item on the work queue and wait for the worker thread to complete processing it */
    ssize_t ret = (ssize_t)enqueue_work_item_and_wait_for_result(work);
    perf_end(_dm_read_perf);
    return ret;
}

/** Clear a data Item */
__EXPORT int
dm_clear(dm_item_t item) {
    work_q_item_t *work;

    /* Make sure data manager has been started and is not shutting down */
    if (!is_running() || g_task_should_exit) {
        return -1;
    }

    /* get a work item and queue up a clear request */
    if ((work = create_work_item()) == nullptr) {
        PX4_ERR("dm_clear create_work_item failed");
        return -1;
    }

    work->func              = dm_clear_func;
    work->clear_params.item = item;

    /* Enqueue the item on the work queue and wait for the worker thread to complete processing it */
    return enqueue_work_item_and_wait_for_result(work);
}

__EXPORT int
dm_lock(dm_item_t item) {
    /* Make sure data manager has been started and is not shutting down */
    if (!is_running() || g_task_should_exit) {
        errno = EINVAL;
        return -1;
    }

    if (item >= DM_KEY_NUM_KEYS) {
        errno = EINVAL;
        return -1;
    }

    if (g_item_locks[item]) {
        return rt_sem_take(g_item_locks[item], RT_WAITING_FOREVER);
    }

    errno = EINVAL;
    return -1;
}

__EXPORT int
dm_trylock(dm_item_t item) {
    /* Make sure data manager has been started and is not shutting down */
    if (!is_running() || g_task_should_exit) {
        errno = EINVAL;
        return -1;
    }

    if (item >= DM_KEY_NUM_KEYS) {
        errno = EINVAL;
        return -1;
    }

    if (g_item_locks[item]) {
        return rt_sem_trytake(g_item_locks[item]);
    }

    errno = EINVAL;
    return -1;
}

/** Unlock a data Item */
__EXPORT void
dm_unlock(dm_item_t item) {
    /* Make sure data manager has been started and is not shutting down */
    if (!is_running() || g_task_should_exit) {
        return;
    }

    if (item >= DM_KEY_NUM_KEYS) {
        return;
    }

    if (g_item_locks[item]) {
        rt_sem_release(g_item_locks[item]);
    }
}

static void
task_main(void *param) {
    /* Dataman can use disk or RAM */
    switch (backend) {
    case BACKEND_FILE:
        g_dm_ops = &dm_file_operations;
        break;

    case BACKEND_RAM:
        g_dm_ops = &dm_ram_operations;
        break;

    default:
        PX4_WARN("No valid backend set.");
        return;
    }

    work_q_item_t *work;

    /* Initialize global variables */
    g_key_offsets[0] = 0;

    for (int i = 0; i < ((int)DM_KEY_NUM_KEYS - 1); i++) {
        g_key_offsets[i + 1] = g_key_offsets[i] + (g_per_item_max_index[i] * g_per_item_size[i]);
    }

    unsigned max_offset = g_key_offsets[DM_KEY_NUM_KEYS - 1] + (g_per_item_max_index[DM_KEY_NUM_KEYS - 1] * g_per_item_size[DM_KEY_NUM_KEYS - 1]);

    for (unsigned i = 0; i < dm_number_of_funcs; i++) {
        g_func_counts[i] = 0;
    }

    /* Initialize the item type locks, for now only DM_KEY_MISSION_STATE & DM_KEY_FENCE_POINTS supports locking */
    rt_sem_init(&g_sys_state_mutex_mission, "g_sys_mission", 1, RT_IPC_FLAG_PRIO); /* Initially unlocked */
    rt_sem_init(&g_sys_state_mutex_fence, "g_sys_fence", 1, RT_IPC_FLAG_PRIO);     /* Initially unlocked */

    for (unsigned i = 0; i < DM_KEY_NUM_KEYS; i++) {
        g_item_locks[i] = nullptr;
    }

    g_item_locks[DM_KEY_MISSION_STATE] = &g_sys_state_mutex_mission;
    g_item_locks[DM_KEY_FENCE_POINTS]  = &g_sys_state_mutex_fence;

    g_task_should_exit = false;

    init_q(&g_work_q);
    init_q(&g_free_q);

    rt_sem_init(&g_work_queued_sema, "g_workq_sema", 0, RT_IPC_FLAG_PRIO);

    /* g_work_queued_sema use case is a signal */

    // px4_sem_setprotocol(&g_work_queued_sema, SEM_PRIO_NONE);

    _dm_read_perf  = perf_alloc(PC_ELAPSED, MODULE_NAME ": read");
    _dm_write_perf = perf_alloc(PC_ELAPSED, MODULE_NAME ": write");

    int ret = g_dm_ops->initialize(max_offset);

    if (ret) {
        g_task_should_exit = true;
        goto end;
    }

    switch (backend) {
    case BACKEND_FILE:
        PX4_INFO("data manager file '%s' size is %u bytes", k_data_manager_device_path, max_offset);

        break;

    case BACKEND_RAM:
        PX4_INFO("data manager RAM size is %u bytes", max_offset);
        break;

    default:
        break;
    }

    /* Tell startup that the worker thread has completed its initialization */
    rt_sem_release(&g_init_sema);

    /* Start the endless loop, waiting for then processing work requests */
    while (true) {
        /* do we need to exit ??? */
        if (!g_task_should_exit) {
            /* wait for work */
            g_dm_ops->wait(&g_work_queued_sema, RT_WAITING_FOREVER);
        }

        /* Empty the work queue */
        while ((work = dequeue_work_item())) {
            /* handle each work item with the appropriate handler */
            switch (work->func) {
            case dm_write_func:
                g_func_counts[dm_write_func]++;
                work->result =
                    g_dm_ops->write(work->write_params.item, work->write_params.index, work->write_params.buf, work->write_params.count);
                break;

            case dm_read_func:
                g_func_counts[dm_read_func]++;
                work->result =
                    g_dm_ops->read(work->read_params.item, work->read_params.index, work->read_params.buf, work->read_params.count);
                break;

            case dm_clear_func:
                g_func_counts[dm_clear_func]++;
                work->result = g_dm_ops->clear(work->clear_params.item);
                break;

            default: /* should never happen */
                work->result = -1;
                break;
            }

            /* Inform the caller that work is done */
            rt_sem_release(&work->wait_sem);
        }

        /* time to go???? */
        if (g_task_should_exit) {
            break;
        }
    }

    g_dm_ops->shutdown();

    /* The work queue is now empty, empty the free queue */
    for (;;) {
        if ((work = (work_q_item_t *)sq_remfirst(&(g_free_q.q))) == nullptr) {
            break;
        }

        if (work->first) {
            free(work);
        }
    }

end:
    backend = BACKEND_NONE;
    destroy_q(&g_work_q);
    destroy_q(&g_free_q);
    rt_sem_detach(&g_work_queued_sema);
    rt_sem_detach(&g_sys_state_mutex_mission);
    rt_sem_detach(&g_sys_state_mutex_fence);

    perf_free(_dm_read_perf);
    _dm_read_perf = nullptr;

    perf_free(_dm_write_perf);
    _dm_write_perf = nullptr;

    return;
}

static int
start() {
    rt_sem_init(&g_init_sema, "g_init_sema", 0, RT_IPC_FLAG_PRIO);

    /* g_init_sema use case is a signal */

    // px4_sem_setprotocol(&g_init_sema, SEM_PRIO_NONE);

    rt_thread_t task = rt_thread_create("dataman", task_main, nullptr, 4096, 20, 10);

    /* start the worker thread with low priority for disk IO */
    if (!task) {
        rt_sem_detach(&g_init_sema);
        PX4_ERR("task start failed");
        return -1;
    }

    if (rt_thread_startup(task) != 0) {
        return -1;
    }

    /* wait for the thread to actually initialize */
    rt_sem_take(&g_init_sema, RT_WAITING_FOREVER);
    rt_sem_detach(&g_init_sema);

    return 0;
}

static void
status() {
    /* display usage statistics */
    PX4_INFO("Writes   %u", g_func_counts[dm_write_func]);
    PX4_INFO("Reads    %u", g_func_counts[dm_read_func]);
    PX4_INFO("Clears   %u", g_func_counts[dm_clear_func]);
    PX4_INFO("Max Q lengths work %u, free %u", g_work_q.max_size, g_free_q.max_size);
    perf_print_counter(_dm_read_perf);
    perf_print_counter(_dm_write_perf);
}

static void
stop() {
    /* Tell the worker task to shut down */
    g_task_should_exit = true;
    rt_sem_release(&g_work_queued_sema);
}

static void
usage() {
    PRINT_MODULE_DESCRIPTION(
        R"DESCR_STR(
### Description
Module to provide persistent storage for the rest of the system in form of a simple database through a C API.
Multiple backends are supported:
- a file (eg. on the SD card)
- RAM (this is obviously not persistent)

It is used to store structured data of different types: mission waypoints, mission state and geofence polygons.
Each type has a specific type and a fixed maximum amount of storage items, so that fast random access is possible.

### Implementation
Reading and writing a single item is always atomic. If multiple items need to be read/modified atomically, there is
an additional lock per item type via `dm_lock`.

**DM_KEY_FENCE_POINTS** and **DM_KEY_SAFE_POINTS** items: the first data element is a `mission_stats_entry_s` struct,
which stores the number of items for these types. These items are always updated atomically in one transaction (from
the mavlink mission manager). During that time, navigator will try to acquire the geofence item lock, fail, and will not
check for geofence violations.

)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("dataman", "system");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_PARAM_STRING('f', nullptr, "<file>", "Storage file", true);
    PRINT_MODULE_USAGE_PARAM_FLAG('r', "Use RAM backend (NOT persistent)", true);
    PRINT_MODULE_USAGE_PARAM_COMMENT("The options -f and -r are mutually exclusive. If nothing is specified, a file 'dataman' is used");
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

static int backend_check() {
    if (backend != BACKEND_NONE) {
        PX4_WARN("-f and -r are mutually exclusive");
        usage();
        return -1;
    }

    return 0;
}

int dataman_main(int argc, char *argv[]) {
    if (argc < 2) {
        usage();
        return -1;
    }

    if (!strcmp(argv[1], "start")) {
        if (is_running()) {
            PX4_WARN("dataman already running");
            return -1;
        }

        int         ch;
        int         dmoptind = 1;
        const char *dmoptarg = nullptr;

        /* jump over start and look at options first */

        while ((ch = getopt(argc, argv, "f:r", &dmoptind, &dmoptarg)) != EOF) {
            switch (ch) {
            case 'f':
                if (backend_check()) {
                    return -1;
                }

                backend                    = BACKEND_FILE;
                k_data_manager_device_path = rt_strdup(dmoptarg);
                PX4_INFO("dataman file set to: %s", k_data_manager_device_path);
                break;

            case 'r':
                if (backend_check()) {
                    return -1;
                }

                backend = BACKEND_RAM;
                break;

            // no break
            default:
                usage();
                return -1;
            }
        }

        if (backend == BACKEND_NONE) {
            backend                    = BACKEND_FILE;
            k_data_manager_device_path = rt_strdup(default_device_path);
        }

        start();

        if (!is_running()) {
            PX4_ERR("dataman start failed");
            free(k_data_manager_device_path);
            k_data_manager_device_path = nullptr;
            return -1;
        }

        return 0;
    }

    /* Worker thread should be running for all other commands */
    if (!is_running()) {
        PX4_WARN("dataman worker thread not running");
        usage();
        return -1;
    }

    if (!strcmp(argv[1], "stop")) {
        stop();
        free(k_data_manager_device_path);
        k_data_manager_device_path = nullptr;

    } else if (!strcmp(argv[1], "status")) {
        status();

    } else {
        usage();
        return -1;
    }

    return 0;
}

MSH_CMD_EXPORT_ALIAS(dataman_main, dataman, dataman);

int dataman_start() {
    const char *argv[] = {"dataman", "start"};
    int         argc   = sizeof(argv) / sizeof(argv[0]);
    return dataman_main(argc, (char **)argv);
}

INIT_APP_EXPORT(dataman_start);
