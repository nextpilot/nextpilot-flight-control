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

#include <string>
#include <string.h>
#include "uORB/uORBCommunicator.hpp"

namespace mUORB {

class Aggregator {
public:
    typedef int (*sendFuncPtr)(const char *, const uint8_t *, int);

    void RegisterSendHandler(sendFuncPtr func) {
        sendFunc = func;
    }

    void RegisterHandler(uORBCommunicator::IChannelRxHandler *handler) {
        _RxHandler = handler;
    }

    int16_t ProcessTransmitTopic(const char *topic, const uint8_t *data, uint32_t length_in_bytes);

    void ProcessReceivedTopic(const char *topic, const uint8_t *data, uint32_t length_in_bytes);

    int16_t SendData();

private:
    static const bool debugFlag;

    const std::string topicName = "aggregation";

    // Master flag to enable aggregation
    const bool aggregationEnabled = true;

    const uint32_t        syncFlag            = 0x5A01FF00;
    const uint32_t        syncFlagSize        = 4;
    const uint32_t        topicNameLengthSize = 4;
    const uint32_t        dataLengthSize      = 4;
    const uint32_t        headerSize          = syncFlagSize + topicNameLengthSize + dataLengthSize;
    static const uint32_t numBuffers          = 2;
    static const uint32_t bufferSize          = 2048;

    uint32_t bufferId;
    uint32_t bufferWriteIndex;
    uint8_t  buffer[numBuffers][bufferSize];

    uORBCommunicator::IChannelRxHandler *_RxHandler;

    sendFuncPtr sendFunc;

    bool isAggregate(const char *name) {
        return (strcmp(name, topicName.c_str()) == 0);
    }

    bool NewRecordOverflows(const char *messageName, int32_t length);

    void MoveToNextBuffer();

    void AddRecordToBuffer(const char *messageName, int32_t length, const uint8_t *data);
};

} // namespace mUORB
