/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <px4_platform_common/log.h>

#include <uavcan/uavcan.hpp>
#include <uavcan/protocol/debug/LogMessage.hpp>

class UavcanLogMessage {
public:
    UavcanLogMessage(uavcan::INode &node) :
        _sub_logmessage(node) {
    }
    ~UavcanLogMessage() = default;

    int init() {
        int res = _sub_logmessage.start(LogMessageCbBinder(this, &UavcanLogMessage::logmessage_sub_cb));

        if (res < 0) {
            PX4_ERR("LogMessage sub failed %i", res);
            return res;
        }

        return 0;
    }

private:
    typedef uavcan::MethodBinder<UavcanLogMessage *,
                                 void (UavcanLogMessage::*)(const uavcan::ReceivedDataStructure<uavcan::protocol::debug::LogMessage> &)>
        LogMessageCbBinder;

    void logmessage_sub_cb(const uavcan::ReceivedDataStructure<uavcan::protocol::debug::LogMessage> &msg) {
        int px4_level = _PX4_LOG_LEVEL_INFO;

        switch (msg.level.value) {
        case uavcan::protocol::debug::LogLevel::DEBUG:
            px4_level = _PX4_LOG_LEVEL_DEBUG;
            break;

        case uavcan::protocol::debug::LogLevel::INFO:
            px4_level = _PX4_LOG_LEVEL_INFO;
            break;

        case uavcan::protocol::debug::LogLevel::WARNING:
            px4_level = _PX4_LOG_LEVEL_WARN;
            break;

        case uavcan::protocol::debug::LogLevel::ERROR:
            px4_level = _PX4_LOG_LEVEL_ERROR;
            break;
        }

        char module_name_buffer[80];
        snprintf(module_name_buffer, sizeof(module_name_buffer), "uavcan:%d:%s", msg.getSrcNodeID().get(), msg.source.c_str());
        px4_log_modulename(px4_level, module_name_buffer, msg.text.c_str());
    }

    uavcan::Subscriber<uavcan::protocol::debug::LogMessage, LogMessageCbBinder> _sub_logmessage;
};
