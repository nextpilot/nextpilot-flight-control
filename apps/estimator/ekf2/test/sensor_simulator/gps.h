/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * Feeds Ekf with Gps data
 * @author Kamil Ritz <ka.ritz@hotmail.com>
 */
#ifndef EKF_GPS_H
#define EKF_GPS_H

#include "sensor.h"

namespace sensor_simulator {
namespace sensor {

class Gps : public Sensor {
public:
    Gps(std::shared_ptr<Ekf> ekf);
    ~Gps();

    void setData(const gpsMessage &gps);
    void stepHeightByMeters(const float hgt_change);
    void stepHorizontalPositionByMeters(const Vector2f hpos_change);
    void setPositionRateNED(const Vector3f &rate);
    void setAltitude(const int32_t alt);
    void setLatitude(const int32_t lat);
    void setLongitude(const int32_t lon);
    void setVelocity(const Vector3f &vel);
    void setYaw(const float yaw);
    void setYawOffset(const float yaw);
    void setFixType(const int fix_type);
    void setNumberOfSatellites(const int num_satellites);
    void setPdop(const float pdop);

    gpsMessage getDefaultGpsData();

private:
    void send(uint64_t time) override;

    gpsMessage _gps_data{};
    Vector3f   _gps_pos_rate{};
};

}
} // namespace sensor_simulator::sensor
#endif // EKF_GPS_H
