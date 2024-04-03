/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "ulog_file_be"
#define LOG_LVL LOG_LVL_INFO

#include <dfs_file.h>
#include <ulog_be.h>

#ifndef ULOG_FILE_PATH_LEN
#define ULOG_FILE_PATH_LEN 128
#endif // ULOG_FILE_PATH_LEN

#define ULOG_FILE_NAME_LEN 32

/////////////////////////////////////////////
// 将LOG_X写到文件
/////////////////////////////////////////////
#if defined(ULOG_USING_FILTER) && defined(ULOG_BACKEND_USING_FILE)
static rt_bool_t ulog_text_backend_filter(
    struct ulog_backend *backend, rt_uint32_t level,
    const char *tag, rt_bool_t is_raw,
    const char *log, rt_size_t len) {
    // 进行过滤，只记录非dbg级别的日志
    if (level < LOG_LVL_DBG) {
        return RT_TRUE;
    } else {
        return RT_FALSE;
    }
}

static rt_bool_t ulog_rawhex_backend_filter(
    struct ulog_backend *backend, rt_uint32_t level,
    const char *tag, rt_bool_t is_raw,
    const char *log, rt_size_t len) {
    // 第一步过滤，只让DBG日志通过
    if (level <= LOG_LVL_INFO) {
        return RT_FALSE;
    }
    // 第二步过滤，根据日志内容进行过滤
    if (!rt_strstr(tag, "HEX sensor")) { // 只记录带有"sensor"标签输出
        return RT_TRUE;
    } else {
        return RT_FALSE;
    }
}
#endif // ULOG_USING_FILTER

static int ulog_logger_backend_init() {
    char ulog_file_path[ULOG_FILE_PATH_LEN] = LOGGER_FULL_LOG_ROOT_PATH;
    char ulog_file_name[ULOG_FILE_NAME_LEN] = "console";

#ifdef RT_USING_RTC
    time_t utc_time_sec;
    time(&utc_time_sec);

    // 相当于北京时间2009-02-14 07:31:30
    const time_t GPS_EPOCH_SECS = ((time_t)1234567890ULL);
    if (utc_time_sec > GPS_EPOCH_SECS) {
        struct tm info;
        localtime_r(&utc_time_sec, &info); // 转为tm结构体
        strftime(ulog_file_path + strlen(ulog_file_path), ULOG_FILE_PATH_LEN - strlen(ulog_file_path), "/%Y-%m-%d", &info);
        strftime(ulog_file_name, ULOG_FILE_NAME_LEN, "%Y%m%d_%H%M%S", &info);
    }
#endif // RT_USING_RTC

    ////////////////////////////////////////////////////////
    // 记录LOG_I/D/W/E
    ////////////////////////////////////////////////////////
    static struct ulog_file_be text_be = {0};

    rt_err_t ret = ulog_file_backend_init(&text_be,
                                          ulog_file_name,   // 日志文件名
                                          ulog_file_path,   // 日志存储路径
                                          10,               // 日志文件最多个数
                                          50 * 1024 * 1024, // 单个文件的大小，50M
                                          256);             // 每1024字节存储一次

    // 设置过滤函数
#ifdef ULOG_USING_FILTER
    ulog_backend_set_filter(&text_be.parent, ulog_text_backend_filter);
#endif // ULOG_USING_FILTER

    // 使能后端
    ulog_file_backend_enable(&text_be);

    ////////////////////////////////////////////////////////
    // 记录rawhex
    ////////////////////////////////////////////////////////
    // rawhex后端用于保存raw原始数据
    //     static struct ulog_file_be rawhex_be = {0};
    //     ulog_file_backend_init(&rawhex_be,
    //                            file_name,        // 日志文件名
    //                            path_name,        // 日志存储路径
    //                            10,               // 日志文件最多个数
    //                            50 * 1024 * 1024, // 单个文件的大小，50M
    //                            256);             // 每256字节存储一次，不能太大

    //     // 设置过滤函数
    // #ifdef ULOG_USING_FILTER
    //     ulog_backend_set_filter(&(rawhex_be.parent), ulog_rawhex_backend_filter);
    // #endif // ULOG_USING_FILTER

    //     // 使能后端
    //     ulog_file_backend_enable(&rawhex_be);

    LOG_I("ulog file backend init ok, %s/%s.log", ulog_file_path, ulog_file_name);

    return 0;
}
INIT_EXPORT(ulog_logger_backend_init, "5.0.0");
