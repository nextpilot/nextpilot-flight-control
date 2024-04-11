/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

// 将rt-thread的ulog组件LOG_XXX宏输出的日志转为uorb消息
// (1)log_message消息，被logger模块存储到飞行日志文件中
// (2)mavlink_log消息，通过mavlink模块传输到地面站告警

#define LOG_TAG "ulog_uorb_be"
#define LOG_LVL LOG_LVL_INFO

#include "hrtimer.h"
#include "topics/log_message.h"

#ifdef ULOG_USING_FILTER
static rt_bool_t ulog_uorb_backend_filter(
    struct ulog_backend *backend, rt_uint32_t level,
    const char *tag, rt_bool_t is_raw,
    const char *log, rt_size_t len) {
    // 进行过滤，只记录非dbg级别的日志
    if (level < LOG_LVL_DBG)
        return RT_TRUE;
    else
        return RT_FALSE;
}
#endif // ULOG_USING_FILTER

static orb_advert_t         log_msg_pub = nullptr;
static struct log_message_s log_msg     = {0};
static void                 ulog_uorb_backend_output(struct ulog_backend *backend,
                                                     rt_uint32_t          level,
                                                     const char          *tag,
                                                     rt_bool_t            is_raw,
                                                     const char          *log,
                                                     rt_size_t            len) {
    // 判断是否长度超过最大，如果字符串过长，进行截断处理
    rt_size_t n = (len >= sizeof(log_msg.text)) ? (sizeof(log_msg.text) - 1) : len;

    rt_memcpy(log_msg.text, log, n);
    log_msg.text[n]   = '\0'; ///< 对字符串进行截断
    log_msg.severity  = level;
    log_msg.timestamp = hrt_absolute_time();

    if (!log_msg_pub) {
        log_msg_pub = orb_advertise_queue(ORB_ID(log_message), &log_msg, log_message_s::ORB_QUEUE_LENGTH);
    }

    if (log_msg_pub) {
        orb_publish(ORB_ID(log_message), log_msg_pub, &log_msg);
    }
}

int ulog_uorb_backend_init(void) {
    log_msg_pub = orb_advertise_queue(ORB_ID(log_message), &log_msg, log_message_s::ORB_QUEUE_LENGTH);
    rt_memset(log_msg.text, 0, sizeof(log_msg.text));

    static struct ulog_backend uorb_be = {0};
    uorb_be.output                     = ulog_uorb_backend_output;
#ifdef ULOG_USING_FILTER
    ulog_backend_set_filter(&uorb_be, ulog_uorb_backend_filter);
#endif // ULOG_USING_FILTER
    // 由于ulog的字符串后续需要通过uorb进行处理，故不要添加”颜色“，否则字符串头与尾会增加额外表示颜色的字节
    ulog_backend_register(&uorb_be, "uorb_be", RT_FALSE);

    LOG_I("ulog uorb backend init ok");
    return 0;
}
INIT_APP_EXPORT(ulog_uorb_backend_init);
