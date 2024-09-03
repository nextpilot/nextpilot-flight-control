/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "pa_gs02b"
#define LOG_LVL LOG_LVL_INFO

#include "pa_gs02b.hpp"

using namespace time_literals;
using namespace matrix;
using namespace math;

#ifndef PA_GS02B_UART_DEVICE
#   define PA_GS02B_UART_DEVICE "uart4"
#endif // PA_GS02B_UART_DEVICE

/* 串口设备 */
static rt_device_t _pa_gs02b_device;


ps_gs02b_msg_t _pa_gs02b_msg{};
uint8_t        _pa_gs02b_buf[512];

//
uint16_t    _pa_gs02b_recv_cnt = 0;
hrt_abstime _last_statics_us   = 0;

// 发布主题
static sensor_gps_s                   _sensor_gps{}; // driver
static sensor_baro_s                  _sensor_baro{};
static sensor_gyro_s                  _sensor_gyro{};
static sensor_accel_s                 _sensor_accel{};
static sensor_mag_s                   _sensor_mag{};
static vehicle_air_data_s             _vehicle_air_data{}; // sensor
static vehicle_angular_velocity_s     _vehicle_angular_velocity{};
static vehicle_angular_acceleration_s _vehicle_angular_acceleration{};
static vehicle_acceleration_s         _vehicle_acceleration{};
static vehicle_magnetometer_s         _vehicle_mag{};
static vehicle_attitude_s             _vehicle_attitude{}; // apps
static vehicle_global_position_s      _vehicle_gpos{};
static vehicle_local_position_s       _vehicle_lpos{};

// 发布主题句柄
static orb_publication_data<sensor_gps_s>                   _sensor_gps_pub{ORB_ID(sensor_gps)}; // driver
static orb_publication_data<sensor_baro_s>                  _sensor_baro_pub{ORB_ID(sensor_baro_test)};
static orb_publication_data<sensor_gyro_s>                  _sensor_gyro_pub{ORB_ID(sensor_gyro_test)};
static orb_publication_data<sensor_accel_s>                 _sensor_accel_pub{ORB_ID(sensor_accel_test)};
static orb_publication_data<sensor_mag_s>                   _sensor_mag_pub{ORB_ID(sensor_mag_test)};
static orb_publication_data<vehicle_air_data_s>             _vehicle_air_data_pub{ORB_ID(vehicle_air_data)}; // sensor
static orb_publication_data<vehicle_angular_velocity_s>     _vehicle_angular_velocity_pub{ORB_ID(vehicle_angular_velocity)};
static orb_publication_data<vehicle_angular_acceleration_s> _vehicle_angular_acceleration_pub{ORB_ID(vehicle_angular_acceleration)};
static orb_publication_data<vehicle_acceleration_s>         _vehicle_acceleration_pub{ORB_ID(vehicle_acceleration)};
static orb_publication_data<vehicle_magnetometer_s>         _vehicle_mag_pub{ORB_ID(vehicle_magnetometer)};
static orb_publication_data<vehicle_attitude_s>             _vehicle_attitude_pub{ORB_ID(vehicle_attitude)}; // apps
static orb_publication_data<vehicle_global_position_s>      _vehicle_global_pos_pub{ORB_ID(vehicle_global_position)};
static orb_publication_data<vehicle_local_position_s>       _vehicle_local_pos_pub{ORB_ID(vehicle_local_position)};

uint16_t pa_gs02b_get_check(unsigned char *ptr, unsigned char len) {
    uint16_t Code;
    uint8_t  i, j;

    Code = 0xFFFF;
    for (i = 0; i < len; i++) {
        Code ^= ptr[i];
        for (j = 0; j < 8; j++) {
            if (Code & 0x0001) {
                Code >>= 1;
                Code  ^= 0xa001;
            } else {
                Code >>= 1;
            }
        }
    }
    return (Code);
}

int pa_gs02b_parse_char(uint8_t ch) {
    static int     parse_state = 0;
    static uint8_t rx_bytes    = 0;
    int            ret         = 0;

    switch (parse_state) {
    case 0:
        if (ch == 0xDA) {
            parse_state               = 1;
            rx_bytes                  = 0;
            _pa_gs02b_buf[rx_bytes++] = ch;
        }
        break;

    case 1:
        if (ch == 0x61) {
            _pa_gs02b_buf[rx_bytes++] = ch;
            parse_state               = 2;
        } else {
            _pa_gs02b_msg.err_cnt++;
            parse_state = 0;
        }
        break;

    case 2:
        _pa_gs02b_buf[rx_bytes++] = ch;
        if (rx_bytes == 76) {
            parse_state = 3;
        }
        break;

    case 3:
        _pa_gs02b_buf[rx_bytes++] = ch;
        parse_state               = 4;
        break;

    case 4: {
        _pa_gs02b_buf[rx_bytes++] = ch;
        uint16_t crc_recv         = (_pa_gs02b_buf[76] << 8 | _pa_gs02b_buf[77]);
        uint16_t crc_calc         = pa_gs02b_get_check(_pa_gs02b_buf, 76);
        if (crc_calc == crc_recv) {
            ret = rx_bytes;
        } else {
            _pa_gs02b_msg.err_cnt++;
            LOG_D("crc fail");
        }
        parse_state = 0;
    } break;

    default:
        parse_state = 0;
        break;
    }

    return ret;
}

int32_t s24tos32(uint8_t data1, uint8_t data2, uint8_t data3) {
    int32_t combinedData = (data1 << 16) | (data2 << 8) | data3;
    if ((combinedData & 0x800000) == 0x800000) { // 如果最高位为1，则是负数
        combinedData |= 0xff000000;
    }

    return combinedData;
}

int32_t u32tos32(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4) {
    int32_t combinedData = (data1 << 24) | (data2 << 16) | (data3 << 8) | data4;
    if ((combinedData & 0x80000000) == 0x80000000) { // 如果最高位为1，则是负数
        combinedData |= 0xff00000000;
    }

    return combinedData;
}

void pa_gs02b_cal_local_pos() {
    static bool home_init = false;

    bool pos_type_valid = (_pa_gs02b_msg.pos_type == 16) || // SINGLE
                          (_pa_gs02b_msg.pos_type == 17) || // PSDRDIFF
                          (_pa_gs02b_msg.pos_type == 32) || // L1_FLOAT
                          (_pa_gs02b_msg.pos_type == 34) || // NARROW_FLOAT
                          (_pa_gs02b_msg.pos_type == 50);   // NARROW_INT

    bool vel_type_valid = (_pa_gs02b_msg.vel_type == 8) ||  // DOPPLER_VELOCITY
                          (_pa_gs02b_msg.vel_type == 17) || // PSRDIFF
                          (_pa_gs02b_msg.vel_type == 32) || // L1_FLOAT
                          (_pa_gs02b_msg.vel_type == 34) || // NARROW_FLOAT
                          (_pa_gs02b_msg.vel_type == 50);   // NARROW_INT

    bool hdg_type_valid = (_pa_gs02b_msg.hdg_type == 48) || // L1_INT
                          (_pa_gs02b_msg.hdg_type == 50);   // NARROW_INT

    static map_projection_reference_s reference_position{};

    if (!home_init && pos_type_valid && vel_type_valid && hdg_type_valid) {
        static hrt_abstime first_in = hrt_absolute_time();
        if (hrt_absolute_time() - first_in > 20_s) {
            home_init                      = true;
            hrt_abstime origin_timestamp   = hrt_absolute_time();
            _vehicle_lpos.timestamp        = origin_timestamp;
            _vehicle_lpos.timestamp_sample = origin_timestamp;
            _vehicle_lpos.ref_timestamp    = origin_timestamp;
            _vehicle_lpos.ref_lat          = _pa_gs02b_msg.lat;
            _vehicle_lpos.ref_lon          = _pa_gs02b_msg.lon;
            _vehicle_lpos.ref_alt          = _pa_gs02b_msg.alt;
            _vehicle_lpos.time_utc_usec    = _sensor_gps.time_utc_usec;
            map_projection_init_timestamped(&reference_position, _vehicle_lpos.ref_lat, _vehicle_lpos.ref_lon, hrt_absolute_time());
            map_projection_initialized(&reference_position);
            _pa_gs02b_msg.start_fusion_flag = 1;
            LOG_I("init lat: %.12f, lon: %.12f", _vehicle_lpos.ref_lat, _vehicle_lpos.ref_lon);
            LOG_I("pos, vel, hdg type: %d, %d, %d", _pa_gs02b_msg.pos_type, _pa_gs02b_msg.vel_type, _pa_gs02b_msg.hdg_type);
        }
    }

    if (home_init) {
        Vector2f lpos_horizonal{0.0f, 0.0f};
        map_projection_project(&reference_position, _pa_gs02b_msg.lat, _pa_gs02b_msg.lon, &lpos_horizonal(0), &lpos_horizonal(1));
        Matrix3f R_to_earth_now{};
        Vector3f body_acc{};
        Quatf    q_att{};
        q_att(0)       = _vehicle_attitude.q[0];
        q_att(1)       = _vehicle_attitude.q[1];
        q_att(2)       = _vehicle_attitude.q[2];
        q_att(3)       = _vehicle_attitude.q[3];
        R_to_earth_now = Dcmf(q_att);
        body_acc(0)    = _sensor_accel.x;
        body_acc(1)    = _sensor_accel.y;
        body_acc(2)    = _sensor_accel.z;
        Vector3f ned_acc{R_to_earth_now * body_acc};
        ned_acc(2) += CONSTANTS_ONE_G;

        _vehicle_lpos.timestamp        = hrt_absolute_time();
        _vehicle_lpos.timestamp_sample = _vehicle_lpos.timestamp;
        _vehicle_lpos.x                = lpos_horizonal(0);
        _vehicle_lpos.y                = lpos_horizonal(1);
        _vehicle_lpos.z                = -(_pa_gs02b_msg.alt - _vehicle_lpos.ref_alt);
        _vehicle_lpos.vx               = _pa_gs02b_msg.vel[0];
        _vehicle_lpos.vy               = _pa_gs02b_msg.vel[1];
        _vehicle_lpos.vz               = _pa_gs02b_msg.vel[2];
        _vehicle_lpos.heading          = _pa_gs02b_msg.att[2];
        _vehicle_lpos.ax               = ned_acc(0);
        _vehicle_lpos.ay               = ned_acc(1);
        _vehicle_lpos.az               = ned_acc(2);
        _vehicle_lpos.source           = 2;
        _pa_gs02b_msg.pos[0]           = _vehicle_lpos.x;
        _pa_gs02b_msg.pos[1]           = _vehicle_lpos.y;
        _pa_gs02b_msg.pos[2]           = _vehicle_lpos.z;
        _vehicle_local_pos_pub.publish(_vehicle_lpos);
    }
}

int pa_gs02b_handle_gps_msg(sensor_gps_s *gps_ptr, jzck_fusion_t *jzck_ptr) {
    static uint8_t state_machine = 0;
    switch (state_machine) {
    case 0: {
        // 周期数：1
        // Bit1-当前时刻输出航向 head_exist
        // Bit2-当前时刻输出UTC exist
        // Bit3-速度位置有效标志 pv_valid
        // Bit4-航向有效标志 head_vaid
        // Bit5-UTC 有效标志 UTC valid
        // Bit6~bit8 : 保留
        uint8_t out_flag = jzck_ptr->gps_info[0];
        // DEVICE_ID临时用来存储导航有效标志位
        gps_ptr->device_id = out_flag;
        if (out_flag | 0x04) {
            gps_ptr->vel_ned_valid = true;
        } else {
            gps_ptr->vel_ned_valid = false;
        }
        state_machine = 1;
        break;
    }

    case 1: {
        // 周期数：2
        // GPS时间戳
        uint32_t gps_timestamp_ms = ((jzck_ptr->gps_info[3] << 24) | (jzck_ptr->gps_info[2] << 16) | (jzck_ptr->gps_info[1] << 8) | jzck_ptr->gps_info[0]);
        gps_ptr->time_utc_usec    = gps_timestamp_ms * 1000;
        state_machine             = 2;
        break;
    }

    case 2: {
        // 周期数：3~8
        // GPS经纬高
        static uint8_t lla_raw[24];
        static uint8_t lla_index = 0;
        if (lla_index < 6) {
            lla_raw[lla_index * 4]     = jzck_ptr->gps_info[0];
            lla_raw[lla_index * 4 + 1] = jzck_ptr->gps_info[1];
            lla_raw[lla_index * 4 + 2] = jzck_ptr->gps_info[2];
            lla_raw[lla_index * 4 + 3] = jzck_ptr->gps_info[3];
            lla_index++;
        } else {
            double gps_lat_deg = 0;
            double gps_lon_deg = 0;
            double gps_alt_m   = 0;
            rt_memcpy(&gps_lat_deg, &lla_raw[0], sizeof(double));
            rt_memcpy(&gps_lon_deg, &lla_raw[8], sizeof(double));
            rt_memcpy(&gps_alt_m, &lla_raw[16], sizeof(double));
            gps_ptr->lat           = gps_lat_deg * 1e7;
            gps_ptr->lon           = gps_lon_deg * 1e7;
            gps_ptr->alt           = gps_alt_m * 1e3;
            gps_ptr->alt_ellipsoid = gps_ptr->alt;
            lla_index              = 0;
            state_machine          = 3;
        }
        break;
    }

    case 3: {
        // 周期数：9~11
        // GPS速度，ENU
        static uint8_t vel_raw[12];
        static uint8_t vel_index = 0;
        if (vel_index < 3) {
            vel_raw[vel_index * 4]     = jzck_ptr->gps_info[0];
            vel_raw[vel_index * 4 + 1] = jzck_ptr->gps_info[1];
            vel_raw[vel_index * 4 + 2] = jzck_ptr->gps_info[2];
            vel_raw[vel_index * 4 + 3] = jzck_ptr->gps_info[3];
            vel_index++;
        } else {
            float gps_vel_e_m_s = 0;
            float gps_vel_n_m_s = 0;
            float gps_vel_u_m_s = 0;
            rt_memcpy(&gps_vel_e_m_s, &vel_raw[0], sizeof(float));
            rt_memcpy(&gps_vel_n_m_s, &vel_raw[4], sizeof(float));
            rt_memcpy(&gps_vel_u_m_s, &vel_raw[8], sizeof(float));
            gps_ptr->vel_n_m_s = gps_vel_n_m_s;
            gps_ptr->vel_e_m_s = gps_vel_e_m_s;
            gps_ptr->vel_d_m_s = -gps_vel_u_m_s;
            gps_ptr->vel_m_s   = sqrtf(gps_vel_n_m_s * gps_vel_n_m_s + gps_vel_e_m_s * gps_vel_e_m_s);
            vel_index          = 0;
            state_machine      = 4;
        }
        break;
    }

    case 4: {
        // 周期数：12
        // GPS heading
        const uint16_t gps_heading = jzck_ptr->gps_info[0] | jzck_ptr->gps_info[1];
        state_machine              = 5;
        break;
    }

    case 5: {
        // 周期数：13~15
        // 保留字
        static uint8_t tmp_index = 0;
        if (tmp_index < 3) {
            tmp_index++;
        } else {
            tmp_index     = 0;
            state_machine = 6;
        }
        break;
    }

    case 6: {
        // 周期数：16
        // UTC STATUS
        const uint8_t utc_status = jzck_ptr->gps_info[0];
        const uint8_t utc_year   = jzck_ptr->gps_info[1] | jzck_ptr->gps_info[2];
        const uint8_t utc_month  = jzck_ptr->gps_info[3];
        state_machine            = 7;
        break;
    }

    case 7: {
        // 周期数：17
        // UTC TIME
        const uint8_t utc_day    = jzck_ptr->gps_info[0];
        const uint8_t utc_hour   = jzck_ptr->gps_info[1];
        const uint8_t utc_minute = jzck_ptr->gps_info[2];
        const uint8_t utc_second = jzck_ptr->gps_info[3];
        state_machine            = 8;
        break;
    }

    case 8: {
        // 周期数：18~40
        // 保留字
        static uint8_t rev_index = 0;
        if (rev_index < 22) {
            rev_index++;
        } else {
            rev_index     = 0;
            state_machine = 0;
            return 0;
        }
        break;
    }

    default:
        state_machine = 0;
        break;
    }

    return -1;
}

static inline int pa_gs02b_get_fix_type(int pos_type) {
    int fixed_type = 0;
    // 0 - 1: no fix, 2 : 2D fix, 3 : 3D fix, 4 : RTCM code differential,
    // 5 : Real - Time Kinematic, float, 6 : Real - Time Kinematic, fixed,
    // 8 : Extrapolated.Some applications will not use the value of this field unless it is at least two, so always correctly fill in the fix.
    switch (pos_type) {
    case 16:
        fixed_type = 3;
        break;
    case 17:
    case 18:
    case 20:
    case 68:
    case 69:
        fixed_type = 4; // GPS_OK_FIX_3D_DGPS
        break;
    case 32:
    case 33:
    case 34:
        fixed_type = 5; // GPS_OK_FIX_3D_RTK_FLOAT
        break;
    case 48:
    case 49:
    case 50:
        fixed_type = 6; // GPS_OK_FIX_3D_RTK_FIXED
        break;
    case 0:
    case 1:
    case 2:
    default:
        fixed_type = 0;
        break;
    }

    return fixed_type;
}

void pa_gs02b_run() {
    static uint8_t buff[512];
    rt_size_t      len = rt_device_read(_pa_gs02b_device, 0, buff, sizeof(buff));
    if (len > 0) {
        for (int i = 0; i < len; i++) {
            if (pa_gs02b_parse_char(buff[i]) > 0) {
                // 1 read raw data
                float gyro_x_deg = s24tos32(_pa_gs02b_buf[2], _pa_gs02b_buf[3], _pa_gs02b_buf[4]) * 1e-3f;
                float gyro_y_deg = s24tos32(_pa_gs02b_buf[5], _pa_gs02b_buf[6], _pa_gs02b_buf[7]) * 1e-3f;
                float gyro_z_deg = s24tos32(_pa_gs02b_buf[8], _pa_gs02b_buf[9], _pa_gs02b_buf[10]) * 1e-3f;

                float acc_x_g = s24tos32(_pa_gs02b_buf[11], _pa_gs02b_buf[12], _pa_gs02b_buf[13]) * 1e-5f;
                float acc_y_g = s24tos32(_pa_gs02b_buf[14], _pa_gs02b_buf[15], _pa_gs02b_buf[16]) * 1e-5f;
                float acc_z_g = s24tos32(_pa_gs02b_buf[17], _pa_gs02b_buf[18], _pa_gs02b_buf[19]) * 1e-5f;

                float mag_x_ut = (int16_t)(_pa_gs02b_buf[20] << 8 | _pa_gs02b_buf[21]) * 1e-2f;
                float mag_y_ut = (int16_t)(_pa_gs02b_buf[22] << 8 | _pa_gs02b_buf[23]) * 1e-2f;
                float mag_z_ut = (int16_t)(_pa_gs02b_buf[24] << 8 | _pa_gs02b_buf[25]) * 1e-2f;

                float baro_mbar = (float)((_pa_gs02b_buf[26] << 16) | (_pa_gs02b_buf[27] << 8) | _pa_gs02b_buf[28]) * 1e-2f;

                float pitch = (int16_t)((_pa_gs02b_buf[29] << 8) | _pa_gs02b_buf[30]) * 0.01f;
                float roll  = (int16_t)((_pa_gs02b_buf[31] << 8) | _pa_gs02b_buf[32]) * 0.01f;
                float yaw   = (uint16_t)((_pa_gs02b_buf[33] << 8) | _pa_gs02b_buf[34]) * 0.01f; // 0~360 degree

                float vel_e = s24tos32(_pa_gs02b_buf[35], _pa_gs02b_buf[36], _pa_gs02b_buf[37]) * 1e-4f;
                float vel_n = s24tos32(_pa_gs02b_buf[38], _pa_gs02b_buf[39], _pa_gs02b_buf[40]) * 1e-4f;
                float vel_u = s24tos32(_pa_gs02b_buf[41], _pa_gs02b_buf[42], _pa_gs02b_buf[43]) * 1e-4f;

                double global_pos_lon = u32tos32(_pa_gs02b_buf[44], _pa_gs02b_buf[45], _pa_gs02b_buf[46], _pa_gs02b_buf[47]) * 1e-7;
                double global_pos_lat = u32tos32(_pa_gs02b_buf[48], _pa_gs02b_buf[49], _pa_gs02b_buf[50], _pa_gs02b_buf[51]) * 1e-7;
                float  global_pos_alt = s24tos32(_pa_gs02b_buf[52], _pa_gs02b_buf[53], _pa_gs02b_buf[54]) * 1e-2f;

                uint8_t  ins_flag         = _pa_gs02b_buf[55];
                uint8_t  pos_type         = _pa_gs02b_buf[56];
                uint8_t  vel_type         = _pa_gs02b_buf[57];
                uint8_t  hdg_type         = _pa_gs02b_buf[58];
                uint8_t  scene_mode       = _pa_gs02b_buf[59];
                uint8_t  rev_bytes[2]     = {_pa_gs02b_buf[60], _pa_gs02b_buf[61]};
                uint8_t  gps_cycle_cnt    = _pa_gs02b_buf[62];
                uint16_t imu_timestamp_ms = ((_pa_gs02b_buf[63] << 24) | (_pa_gs02b_buf[64] << 16) | (_pa_gs02b_buf[65] << 8) | (_pa_gs02b_buf[66])) * 0.1;
                uint8_t  gps_info[4]      = {_pa_gs02b_buf[67], _pa_gs02b_buf[68], _pa_gs02b_buf[69], _pa_gs02b_buf[70]};
                float    temperautre      = ((_pa_gs02b_buf[71] << 8) | _pa_gs02b_buf[72]) * 1e-2f;
                uint16_t unknown_cnt      = ((_pa_gs02b_buf[73] << 8) | _pa_gs02b_buf[74]);
                uint8_t  rev_bytes2       = _pa_gs02b_buf[75];
                uint8_t  crc_bytes[2]     = {_pa_gs02b_buf[76], _pa_gs02b_buf[77]};

                // 2 statistical data
                _pa_gs02b_recv_cnt++;

                // 3 transform coordinates and units
                _pa_gs02b_msg.gyro[0]          = math::radians(gyro_y_deg);
                _pa_gs02b_msg.gyro[1]          = math::radians(gyro_x_deg);
                _pa_gs02b_msg.gyro[2]          = math::radians(-gyro_z_deg);
                _pa_gs02b_msg.acc[0]           = acc_y_g * CONSTANTS_ONE_G;
                _pa_gs02b_msg.acc[1]           = acc_x_g * CONSTANTS_ONE_G;
                _pa_gs02b_msg.acc[2]           = -acc_z_g * CONSTANTS_ONE_G;
                _pa_gs02b_msg.mag[0]           = mag_x_ut * 0.01f;
                _pa_gs02b_msg.mag[1]           = mag_y_ut * 0.01f;
                _pa_gs02b_msg.mag[2]           = mag_z_ut * 0.01f;
                _pa_gs02b_msg.baro             = baro_mbar;
                _pa_gs02b_msg.att[0]           = math::radians(roll);
                _pa_gs02b_msg.att[1]           = math::radians(pitch);
                _pa_gs02b_msg.att[2]           = wrap_pi(math::radians(yaw));
                _pa_gs02b_msg.vel[0]           = vel_n;
                _pa_gs02b_msg.vel[1]           = vel_e;
                _pa_gs02b_msg.vel[2]           = -vel_u;
                _pa_gs02b_msg.lat              = global_pos_lat;
                _pa_gs02b_msg.lon              = global_pos_lon;
                _pa_gs02b_msg.alt              = global_pos_alt;
                _pa_gs02b_msg.ins_flag         = ins_flag;
                _pa_gs02b_msg.pos_type         = pos_type;
                _pa_gs02b_msg.vel_type         = vel_type;
                _pa_gs02b_msg.hdg_type         = hdg_type;
                _pa_gs02b_msg.scene_mode       = scene_mode;
                _pa_gs02b_msg.rev_bytes2       = rev_bytes2;
                _pa_gs02b_msg.gps_cycle_cnt    = gps_cycle_cnt;
                _pa_gs02b_msg.imu_timestamp_ms = imu_timestamp_ms;
                _pa_gs02b_msg.gps_info[0]      = gps_info[0];
                _pa_gs02b_msg.gps_info[1]      = gps_info[1];
                _pa_gs02b_msg.gps_info[2]      = gps_info[2];
                _pa_gs02b_msg.gps_info[3]      = gps_info[3];
                _pa_gs02b_msg.temperautre      = temperautre;
                _pa_gs02b_msg.unknown_cnt      = unknown_cnt;
                _pa_gs02b_msg.rev_bytes2       = rev_bytes2;
                _pa_gs02b_msg.crc_bytes[0]     = crc_bytes[0];
                _pa_gs02b_msg.crc_bytes[1]     = crc_bytes[1];
                hrt_abstime pub_time_us        = hrt_absolute_time();
                if (pub_time_us - _last_statics_us > 5_s) {
                    _pa_gs02b_msg.freq = _pa_gs02b_recv_cnt / 5.0f;
                    _last_statics_us   = pub_time_us;
                    _pa_gs02b_recv_cnt = 0;
                }

                // 4 publish topics
                int recv_all = pa_gs02b_handle_gps_msg(&_sensor_gps, &_pa_gs02b_msg);
                if (recv_all == 0) {
                    _sensor_gps.fix_type        = pa_gs02b_get_fix_type(_pa_gs02b_msg.pos_type);
                    _sensor_gps.timestamp       = hrt_absolute_time();
                    _sensor_gps.s_variance_m_s  = 0.1f;
                    _sensor_gps.c_variance_rad  = 0.1f;
                    _sensor_gps.eph             = 0.1f;
                    _sensor_gps.epv             = 0.1f;
                    _sensor_gps.hdop            = 0.1f;
                    _sensor_gps.vdop            = 0.1f;
                    _sensor_gps.satellites_used = 20;
                    _sensor_gps_pub.publish(_sensor_gps);
                }

                /* vehicle_attitude */
                Quatf current_q                    = Quatf{Eulerf{_pa_gs02b_msg.att[0], _pa_gs02b_msg.att[1], _pa_gs02b_msg.att[2]}};
                _vehicle_attitude.timestamp        = pub_time_us;
                _vehicle_attitude.timestamp_sample = pub_time_us;
                _vehicle_attitude.time_utc_usec    = _sensor_gps.time_utc_usec;
                for (int i = 0; i < 4; i++) {
                    _vehicle_attitude.q[i]             = current_q(i);
                    _vehicle_attitude.delta_q_reset[i] = 0;
                }
                _vehicle_attitude.quat_reset_counter = 0;
                _vehicle_attitude.source             = 2;
                _vehicle_attitude_pub.publish(_vehicle_attitude);

                /* vehicle_global_position */
                _vehicle_gpos.timestamp        = pub_time_us;
                _vehicle_gpos.timestamp_sample = pub_time_us;
                _vehicle_gpos.lat              = _pa_gs02b_msg.lat;
                _vehicle_gpos.lon              = _pa_gs02b_msg.lon;
                _vehicle_gpos.time_utc_usec    = _sensor_gps.time_utc_usec;
                _vehicle_gpos.alt              = _pa_gs02b_msg.alt;
                _vehicle_gpos.source           = 2;
                _vehicle_global_pos_pub.publish(_vehicle_gpos);

                /* sensor_baro */
                _sensor_baro.timestamp        = pub_time_us;
                _sensor_baro.timestamp_sample = pub_time_us;
                _sensor_baro.device_id        = 2;
                _sensor_baro.error_count      = 0;
                _sensor_baro.pressure         = _pa_gs02b_msg.baro;
                _sensor_baro.temperature      = _pa_gs02b_msg.temperautre;
                _sensor_baro_pub.publish(_sensor_baro);

                /* vehicle_air_data */
                _vehicle_air_data.timestamp        = pub_time_us;
                _vehicle_air_data.timestamp_sample = pub_time_us;
                _vehicle_air_data.baro_device_id   = _sensor_baro.device_id;
                // _vehicle_air_data.baro_alt_meter    = _sensor_baro.filtered_alt_meter; // TODO: pas_gs02b协议未输出气压高度
                _vehicle_air_data.baro_temp_celcius = _sensor_baro.temperature;
                _vehicle_air_data.baro_pressure_pa  = _sensor_baro.pressure;
                _vehicle_air_data.rho               = 1.225f;
                _vehicle_air_data_pub.publish(_vehicle_air_data);

                /* sensor_gyro */
                _sensor_gyro.timestamp        = pub_time_us;
                _sensor_gyro.timestamp_sample = pub_time_us;
                _sensor_gyro.device_id        = 2;
                _sensor_gyro.x                = _pa_gs02b_msg.gyro[0];
                _sensor_gyro.y                = _pa_gs02b_msg.gyro[1];
                _sensor_gyro.z                = _pa_gs02b_msg.gyro[2];
                _sensor_gyro.temperature      = _pa_gs02b_msg.temperautre;
                _sensor_gyro.error_count      = 0;
                _sensor_gyro.samples          = 1;
                _sensor_gyro_pub.publish(_sensor_gyro);

                /* vehicle_angular_velocity */
                _vehicle_angular_velocity.timestamp        = pub_time_us;
                _vehicle_angular_velocity.timestamp_sample = pub_time_us;
                _vehicle_angular_velocity.xyz[0]           = _sensor_gyro.x;
                _vehicle_angular_velocity.xyz[1]           = _sensor_gyro.y;
                _vehicle_angular_velocity.xyz[2]           = _sensor_gyro.z;
                _vehicle_angular_velocity_pub.publish(_vehicle_angular_velocity);

                /* vehicle_angular_acceleration */
                static hrt_abstime                last_time_us = hrt_absolute_time();
                hrt_abstime                       dt_us        = hrt_absolute_time() - last_time_us;
                static vehicle_angular_velocity_s last_vehicle_ang_vel{};
                _vehicle_angular_acceleration.timestamp        = pub_time_us;
                _vehicle_angular_acceleration.timestamp_sample = pub_time_us;
                matrix::Vector3f ang_acc_raw((_vehicle_angular_velocity.xyz[0] - last_vehicle_ang_vel.xyz[0]) * 1_s / dt_us,
                                             (_vehicle_angular_velocity.xyz[1] - last_vehicle_ang_vel.xyz[1]) * 1_s / dt_us,
                                             (_vehicle_angular_velocity.xyz[2] - last_vehicle_ang_vel.xyz[2]) * 1_s / dt_us);
                // TODO: 未做滤波
                // matrix::Vector3f ang_acc_filter      = _angular_acceleration_filter.apply(ang_acc_raw);
                // _vehicle_angular_acceleration.xyz[0] = ang_acc_filter(0);
                // _vehicle_angular_acceleration.xyz[1] = ang_acc_filter(1);
                // _vehicle_angular_acceleration.xyz[2] = ang_acc_filter(2);
                _vehicle_angular_acceleration_pub.publish(_vehicle_angular_acceleration);
                last_vehicle_ang_vel = last_vehicle_ang_vel;
                last_time_us         = hrt_absolute_time();

                /* sensor_accel */
                _sensor_accel.timestamp        = pub_time_us;
                _sensor_accel.timestamp_sample = pub_time_us;
                _sensor_accel.device_id        = 2;
                _sensor_accel.x                = _pa_gs02b_msg.acc[0];
                _sensor_accel.y                = _pa_gs02b_msg.acc[1];
                _sensor_accel.z                = _pa_gs02b_msg.acc[2];
                _sensor_accel.temperature      = _pa_gs02b_msg.temperautre;
                _sensor_accel.error_count      = 0;
                _sensor_accel.clip_counter[0]  = 0;
                _sensor_accel.clip_counter[1]  = 0;
                _sensor_accel.clip_counter[2]  = 0;
                _sensor_accel.samples          = 1;
                _sensor_accel_pub.publish(_sensor_accel);

                /* vehicle_acceleration */
                _vehicle_acceleration.timestamp        = pub_time_us;
                _vehicle_acceleration.timestamp_sample = pub_time_us;
                _vehicle_acceleration.xyz[0]           = _sensor_accel.x;
                _vehicle_acceleration.xyz[1]           = _sensor_accel.y;
                _vehicle_acceleration.xyz[2]           = _sensor_accel.z;
                _vehicle_acceleration_pub.publish(_vehicle_acceleration);

                /* sensor_mag */
                _sensor_mag.timestamp        = pub_time_us;
                _sensor_mag.timestamp_sample = pub_time_us;
                _sensor_mag.device_id        = 2;
                _sensor_mag.x                = _pa_gs02b_msg.mag[0];
                _sensor_mag.y                = _pa_gs02b_msg.mag[1];
                _sensor_mag.z                = _pa_gs02b_msg.mag[2];
                _sensor_mag.temperature      = _pa_gs02b_msg.temperautre;
                _sensor_mag.error_count      = 0;
                _sensor_mag.is_external      = true;
                _sensor_mag_pub.publish(_sensor_mag);

                /* vehicle_mag */
                _vehicle_mag.timestamp          = pub_time_us;
                _vehicle_mag.timestamp_sample   = pub_time_us;
                _vehicle_mag.device_id          = _sensor_mag.device_id;
                _vehicle_mag.magnetometer_ga[0] = _sensor_mag.x;
                _vehicle_mag.magnetometer_ga[1] = _sensor_mag.y;
                _vehicle_mag.magnetometer_ga[2] = _sensor_mag.z;
                _vehicle_mag.calibration_count  = 0;
                _vehicle_mag_pub.publish(_vehicle_mag);

                /* Calculate local_position */
                pa_gs02b_cal_local_pos();
            }
        }
    }
}

int pa_gs02b_init() {
    _pa_gs02b_device = rt_device_find(PA_GS02B_UART_DEVICE);
    if (!_pa_gs02b_device) {
        LOG_E("find %s fail", PA_GS02B_UART_DEVICE);
        return -1;
    }

    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate               = BAUD_RATE_460800;
    config.data_bits               = DATA_BITS_8;
    config.stop_bits               = STOP_BITS_1;
    config.parity                  = PARITY_NONE;
#if defined(RT_USING_SERIAL_V1)
    config.bufsz = 512;
#else
    config.tx_bufsz = 512;
    config.rx_bufsz = 512;
#endif // RT_USING_SERIAL_V1

    if (rt_device_control(_pa_gs02b_device, RT_DEVICE_CTRL_CONFIG, &config) != 0) {
        LOG_E("cfg %s fail", PA_GS02B_UART_DEVICE);
        return -1;
    }

    uint16_t openflag = 0;
#if defined(RT_USING_SERIAL_V1)
    if (_pa_gs02b_device->flag & RT_DEVICE_FLAG_DMA_RX) {
        openflag |= RT_DEVICE_FLAG_DMA_RX;
    } else if (_pa_gs02b_device->flag & RT_DEVICE_FLAG_INT_RX) {
        openflag |= RT_DEVICE_FLAG_INT_RX;
    }

    if (_pa_gs02b_device->flag & RT_DEVICE_FLAG_DMA_TX) {
        openflag |= RT_DEVICE_FLAG_DMA_TX;
    } else if (_pa_gs02b_device->flag & RT_DEVICE_FLAG_INT_TX) {
        openflag |= RT_DEVICE_FLAG_INT_TX;
    }
#else
    openflag = RT_DEVICE_FLAG_RX_NON_BLOCKING | RT_DEVICE_FLAG_TX_NON_BLOCKING;
#endif // RT_USING_SERIAL_V1

    if (rt_device_open(_pa_gs02b_device, openflag) != 0) {
        LOG_E("open %s fail, ret: %d", _pa_gs02b_device, open_ret);
        return -1;
    }

    return 0;
}

int pa_gs02b_main(int argc, char *argv[]) {
    LOG_RAW("PA_GS02B INFO:\n");
    LOG_RAW("\tfreq: %.2f Hz\n", _pa_gs02b_msg.freq);
    LOG_RAW("\terr count: %llu\n", _pa_gs02b_msg.err_cnt);
    return 0;
}

MSH_CMD_EXPORT_ALIAS(pa_gs02b_main, pags, pa_gs02b from jzck);
