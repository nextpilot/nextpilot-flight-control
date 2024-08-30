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

#include <module/module_command.hpp>
#include <module/module_params.hpp>

#include <src/modules/uxrce_dds_client/dds_topics.h>

#include <timesync/Timesync.hpp>

class UxrceddsClient : public ModuleCommand<UxrceddsClient>, public ModuleParams {
public:
    enum class Transport {
        Serial,
        Udp
    };

    UxrceddsClient(Transport transport, const char *device, int baudrate, const char *host, const char *port,
                   bool localhost_only, bool custom_participant, const char *client_namespace);

    ~UxrceddsClient();

    /** @see ModuleCommand */
    static int task_spawn(int argc, char *argv[]);

    /** @see ModuleCommand */
    static UxrceddsClient *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleCommand::run() */
    void run() override;

    /** @see ModuleCommand::print_status() */
    int print_status() override;

private:
    int setBaudrate(int fd, unsigned baud);

    const bool  _localhost_only;
    const bool  _custom_participant;
    const char *_client_namespace;

    // max port characters (5+'\0')
    static const uint8_t PORT_MAX_LENGTH = 6;
    // max agent ip characters (15+'\0')
    static const uint8_t AGENT_IP_MAX_LENGTH = 16;

#if defined(RT_USING_LWIP)
    char _port[PORT_MAX_LENGTH];
    char _agent_ip[AGENT_IP_MAX_LENGTH];
#endif

    SendTopicsSubs *_subs{nullptr};
    RcvTopicsPubs  *_pubs{nullptr};

    uxrSerialTransport *_transport_serial{nullptr};
    uxrUDPTransport    *_transport_udp{nullptr};
    uxrCommunication   *_comm{nullptr};
    int                 _fd{-1};

    int  _last_payload_tx_rate{}; ///< in B/s
    int  _last_payload_rx_rate{}; ///< in B/s
    bool _connected{false};

    Timesync _timesync{timesync_status_s::SOURCE_PROTOCOL_DDS};

    DEFINE_PARAMETERS(
        (ParamInt<px4::params::UXRCE_DDS_DOM_ID>)_param_xrce_dds_dom_id,
        (ParamInt<px4::params::UXRCE_DDS_KEY>)_param_xrce_key)
};
