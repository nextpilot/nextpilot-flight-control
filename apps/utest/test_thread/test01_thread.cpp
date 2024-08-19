#include <rtthread.h>
#include <string.h>

static bool should_exit_flag{false};

extern "C" void thread_test01_main_entry(void *param) {
    rt_uint32_t cnt = 0;
    while (!should_exit_flag) {
        rt_thread_mdelay(5);
        rt_kprintf("======%ld, %d\n", rt_tick_get(), cnt++);
    }
}

extern "C" int thread_test01_start() {
    rt_thread_t tid = rt_thread_create("alex", thread_test01_main_entry, RT_NULL, 1024, 10, 100);
    if (tid != RT_NULL) {
        rt_thread_startup(tid);
    }

    return 0;
}

extern "C" int thread_test01_stop() {
    should_exit_flag = true;
    return 0;
}

// INIT_APP_EXPORT(start);

extern "C" int thread_test01_test(int argc, char *argv[]) {
    if (argc < 2) {
        return 0;
    }
    if (strcmp(argv[1], "help") == 0) {
    } else if (strcmp(argv[1], "start") == 0) {
        thread_test01_start();
    } else if (strcmp(argv[1], "stop") == 0) {
        thread_test01_stop();
    }

    return 0;
}

MSH_CMD_EXPORT_ALIAS(thread_test01_test, utest_th01, thread test01 useage);
