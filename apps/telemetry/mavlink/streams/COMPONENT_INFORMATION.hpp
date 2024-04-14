/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef COMPONENT_INFORMATION_HPP
#define COMPONENT_INFORMATION_HPP

#include "../mavlink_stream.h"

#include <component_information/checksums.h>

#include <px4_platform_common/defines.h>

#include <sys/stat.h>

class MavlinkStreamComponentInformation : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamComponentInformation(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "COMPONENT_INFORMATION";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_COMPONENT_INFORMATION;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return 0; // never streamed
    }

    bool request_message(float param2, float param3, float param4,
                         float param5, float param6, float param7) override {
        mavlink_component_information_t component_info{};
        PX4_DEBUG("COMPONENT_INFORMATION request");

        strncpy(component_info.general_metadata_uri, "mftp://etc/extras/component_general.json.xz",
                sizeof(component_info.general_metadata_uri) - 1);
        component_info.general_metadata_file_crc = component_information::component_general_crc;

        component_info.time_boot_ms = hrt_absolute_time() / 1000;
        mavlink_msg_component_information_send_struct(_mavlink->get_channel(), &component_info);

        return true;
    }

private:
    explicit MavlinkStreamComponentInformation(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    bool send() override {
        return false;
    }
};

#endif // COMPONENT_INFORMATION_HPP
