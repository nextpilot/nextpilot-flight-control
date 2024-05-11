/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef SMART_BATTERY_INFO_HPP
#define SMART_BATTERY_INFO_HPP

#include <uORB/topics/battery_status.h>

class MavlinkStreamSmartBatteryInfo : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamSmartBatteryInfo(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "SMART_BATTERY_INFO";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_SMART_BATTERY_INFO;
    }

    const char *get_name() const override {
        return get_name_static();
    }

    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        static constexpr unsigned size_per_battery = MAVLINK_MSG_ID_SMART_BATTERY_INFO_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
        return size_per_battery * _battery_status_subs.advertised_count();
    }

private:
    explicit MavlinkStreamSmartBatteryInfo(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::SubscriptionMultiArray<battery_status_s, battery_status_s::MAX_INSTANCES> _battery_status_subs{ORB_ID::battery_status};

    bool send() override {
        bool updated = false;

        for (auto &battery_sub : _battery_status_subs) {
            battery_status_s battery_status;

            if (battery_sub.update(&battery_status)) {
                if (battery_status.serial_number == 0) {
                    // This is not smart battery
                    continue;
                }

                mavlink_smart_battery_info_t msg{};

                msg.id                          = battery_status.id - 1;
                msg.capacity_full_specification = battery_status.capacity;
                msg.capacity_full               = (int32_t)((float)(battery_status.state_of_health * battery_status.capacity) / 100.f);
                msg.cycle_count                 = battery_status.cycle_count;

                if (battery_status.manufacture_date) {
                    uint16_t day   = battery_status.manufacture_date % 32;
                    uint16_t month = (battery_status.manufacture_date >> 5) % 16;
                    uint16_t year  = (80 + (battery_status.manufacture_date >> 9)) % 100;

                    // Formatted as 'dd/mm/yy-123456' (maxed 15 + 1 chars)
                    rt_snprintf(msg.serial_number, sizeof(msg.serial_number), "%d/%d/%d-%d", day, month, year, battery_status.serial_number);

                } else {
                    rt_snprintf(msg.serial_number, sizeof(msg.serial_number), "%d", battery_status.serial_number);
                }

                // msg.device_name = ??
                msg.weight                    = -1;
                msg.discharge_minimum_voltage = -1;
                msg.charging_minimum_voltage  = -1;
                msg.resting_minimum_voltage   = -1;

                mavlink_msg_smart_battery_info_send_struct(_mavlink->get_channel(), &msg);
                updated = true;
            }
        }

        return updated;
    }
};

#endif // SMART_BATTERY_INFO_HPP
