#include <rtthread.h>
#include <string.h>

extern "C" void main_entry(void *param) {
    rt_uint32_t cnt = 0;
    while (1) {
        rt_thread_mdelay(5);
        rt_kprintf("======%ld, %d\n", rt_tick_get(), cnt++);
    }
}

extern "C" int start() {
    rt_thread_t tid = rt_thread_create("alex", main_entry, RT_NULL, 1024, 10, 100);
    if (tid != RT_NULL) {
        rt_thread_startup(tid);
    }
    return 0;
}

// INIT_APP_EXPORT(start);

extern "C" int alex_test(int argc, char *argv[]) {
    if (argc < 2) {
        return 0;
    }
    if (strcmp(argv[1], "help") == 0) {
        rt_kprintf("===============\n");
    } else if (strcmp(argv[1], "start") == 0) {
        start();
    }

    return 0;
}

MSH_CMD_EXPORT_ALIAS(alex_test, alex, alex test useage);
