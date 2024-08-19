/**
 * @file test02_create_lotsof_thread.cpp
 * @author your name (you@domain.com)
 * @brief
 * 大量线程创建测试。
 * 宏THREADS_NUM定义了创建的线程数量；
 *
 * 启动命令：
 * $ utest02 start
 * @version 0.1
 * @date 2024-08-19
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <rtthread.h>
#include <string.h>

#define THREADS_NUM 2

static bool should_exit_flag{false};

static char        thread_names[THREADS_NUM][10] = {};
static rt_thread_t thread_tids[THREADS_NUM]      = {};

extern "C" void create_thread_names() {
    for (int i = 0; i < THREADS_NUM; ++i) {
        rt_sprintf(thread_names[i], "thread-%d", i);
    }
}

extern "C" void thread_test02_main_entry(void *param) {
    rt_uint32_t cnt = 0;

    while (!should_exit_flag) {
        rt_thread_mdelay(50);
        rt_kprintf(">>>%s: %ld, %d\n", (char *)param, rt_tick_get(), cnt++);
    }
}

extern "C" int thread_test02_start() {
    // 先创建每个线程的名称
    create_thread_names();

    for (int i = 0; i < THREADS_NUM; ++i) {
        thread_tids[i] = rt_thread_create(thread_names[i], thread_test02_main_entry, thread_names[i], 1024, 10, 100);
        if (thread_tids[i] != RT_NULL) {
            rt_thread_startup(thread_tids[i]);
        }
    }

    return 0;
}

extern "C" int thread_test02_stop() {
    should_exit_flag = true;
    return 0;
}

// INIT_APP_EXPORT(start);

extern "C" int thread_test02(int argc, char *argv[]) {
    if (argc < 2) {
        return 0;
    }
    if (strcmp(argv[1], "help") == 0) {
    } else if (strcmp(argv[1], "start") == 0) {
        thread_test02_start();
    } else if (strcmp(argv[1], "stop") == 0) {
        thread_test02_stop();
    }

    return 0;
}

MSH_CMD_EXPORT_ALIAS(thread_test02, utest02, thread test02 useage);
