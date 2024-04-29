/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef COMPONENT_METADATA_HPP
#define COMPONENT_METADATA_HPP

#include "../mavlink_stream.h"

#include <component_information/checksums.h>

#include <defines.h>

#include <sys/stat.h>

class MavlinkStreamComponentMetadata : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamComponentMetadata(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "COMPONENT_METADATA";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_COMPONENT_METADATA;
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
        mavlink_component_metadata_t component_metadata{};
        PX4_DEBUG("COMPONENT_METADATA request");

        strncpy(component_metadata.uri, "mftp://etc/extras/component_general.json.xz",
                sizeof(component_metadata.uri) - 1);
        component_metadata.file_crc = component_information::component_general_crc;

        component_metadata.time_boot_ms = hrt_absolute_time() / 1000;
        mavlink_msg_component_metadata_send_struct(_mavlink->get_channel(), &component_metadata);

        return true;
    }

private:
    explicit MavlinkStreamComponentMetadata(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    bool send() override {
        return false;
    }
};

#endif // COMPONENT_METADATA_HPP
