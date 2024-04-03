# ulog_uorb_backend

将LOG_I/W/E/D打印日志转为uORB的log_message_s消息，便于logger模块订阅并写入到飞控日志文件中。

# ulog_logger_backend

将LOG_I/W/E/D打印日志，直接写入到microSD中，以便用于数据分析。

# mavlink_vasprintf

提供了mavlink_log_info等函数，将告警、错误等信息通过ulog组件输出，同时转为uORB的mavlink_log_s消息，然后mavlink模块订阅并发送地面站
