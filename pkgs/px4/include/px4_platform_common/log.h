#ifndef __LOG_H__
#define __LOG_H__

#include <rtdbg.h>

#define _PX4_LOG_LEVEL_DEBUG 0
#define _PX4_LOG_LEVEL_INFO  1
#define _PX4_LOG_LEVEL_WARN  2
#define _PX4_LOG_LEVEL_ERROR 3
#define _PX4_LOG_LEVEL_PANIC 4

// Used to silence unused variable warning
static inline void do_nothing(int level, ...) {
    (void)level;
}

#define PX4_INFO     LOG_I
#define PX4_PANIC    LOG_I
#define PX4_ERR      LOG_E
#define PX4_WARN     LOG_W
#define PX4_DEBUG    LOG_D
#define PX4_INFO_RAW LOG_RAW


#define PX4_LOG_NAMED_COND(name, cond, fmt, ...) \
    do {                                         \
        if (cond) {                              \
            LOG_I(fmt, ##__VA_ARGS__);           \
        }                                        \
    } while (0);

#endif // __LOG_H__
