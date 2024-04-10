/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef GYRO_FFT_HPP
#define GYRO_FFT_HPP

#include <mathlib/math/filter/MedianFilter.hpp>
#include <matrix/matrix/math.hpp>
#include <perf/perf_counter.h>
#include <defines.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <px4_platform_common/posix.h>
#include <workq/WorkItemScheduled.hpp>
#include <uORB/uORBPublication.hpp>
#include <uORB/uORBPublication.hpp>
#include <uORB/uORBSubscription.hpp>
#include <uORB/uORBSubscription.hpp>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_gyro.h>
#include <uORB/topics/sensor_gyro_fft.h>
#include <uORB/topics/sensor_gyro_fifo.h>
#include <uORB/topics/sensor_selection.h>
#include <uORB/topics/vehicle_imu_status.h>

#include "arm_math.h"
#include "arm_const_structs.h"

using namespace time_literals;

class GyroFFT : public ModuleCommand<GyroFFT>, public ModuleParams, public WorkItemScheduled {
public:
    GyroFFT();
    ~GyroFFT() override;

    /** @see ModuleCommand */
    static int *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleCommand::print_status() */
    int print_status() override;

    int init() override;

private:
    static constexpr int MAX_SENSOR_COUNT = 4;

    static constexpr int MAX_NUM_PEAKS = sizeof(sensor_gyro_fft_s::peak_frequencies_x) / sizeof(
                                                                                             sensor_gyro_fft_s::peak_frequencies_x[0]);

    void         Run() override;
    inline void  FindPeaks(const hrt_abstime &timestamp_sample, int axis, q15_t *fft_outupt_buffer);
    inline float EstimatePeakFrequencyBin(q15_t fft[], int peak_index);
    inline void  Publish();
    bool         SensorSelectionUpdate(bool force = false);
    void         Update(const hrt_abstime &timestamp_sample, int16_t *input[], uint8_t N);
    inline void  UpdateOutput(const hrt_abstime &timestamp_sample, int axis, float peak_frequencies[MAX_NUM_PEAKS],
                              float peak_snr[MAX_NUM_PEAKS], int num_peaks_found);
    void         VehicleIMUStatusUpdate(bool force = false);

    template <size_t N>
    bool AllocateBuffers() {
        _gyro_data_buffer_x = new q15_t[N];
        _gyro_data_buffer_y = new q15_t[N];
        _gyro_data_buffer_z = new q15_t[N];
        _hanning_window     = new q15_t[N];
        _fft_input_buffer   = new q15_t[N];
        _fft_outupt_buffer  = new q15_t[N * 2];

        _peak_magnitudes_all = new float[N];

        return (_gyro_data_buffer_x && _gyro_data_buffer_y && _gyro_data_buffer_z && _hanning_window && _fft_input_buffer && _fft_outupt_buffer);
    }

    uORB::Publication<sensor_gyro_fft_s> _sensor_gyro_fft_pub{ORB_ID(sensor_gyro_fft)};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::Subscription _sensor_selection_sub{ORB_ID(sensor_selection)};
    uORB::Subscription _vehicle_imu_status_sub{ORB_ID(vehicle_imu_status)};

    uORB::SubscriptionCallbackWorkItem _sensor_gyro_sub{this, ORB_ID(sensor_gyro)};
    uORB::SubscriptionCallbackWorkItem _sensor_gyro_fifo_sub{this, ORB_ID(sensor_gyro_fifo)};

    perf_counter_t _cycle_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")};
    perf_counter_t _cycle_interval_perf{perf_alloc(PC_INTERVAL, MODULE_NAME ": cycle interval")};
    perf_counter_t _fft_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": FFT")};
    perf_counter_t _gyro_generation_gap_perf{nullptr};
    perf_counter_t _gyro_fifo_generation_gap_perf{nullptr};

    uint32_t _selected_sensor_device_id{0};

    bool _gyro_fifo{false};

    arm_rfft_instance_q15 _rfft_q15;

    q15_t *_gyro_data_buffer_x{nullptr};
    q15_t *_gyro_data_buffer_y{nullptr};
    q15_t *_gyro_data_buffer_z{nullptr};
    q15_t *_hanning_window{nullptr};
    q15_t *_fft_input_buffer{nullptr};
    q15_t *_fft_outupt_buffer{nullptr};

    float *_peak_magnitudes_all{nullptr};

    float _gyro_sample_rate_hz{8000}; // 8 kHz default

    float _fifo_last_scale{0};

    int _fft_buffer_index[3]{};

    unsigned _gyro_last_generation{0};

    math::MedianFilter<float, 7> _median_filter[3][MAX_NUM_PEAKS]{};

    sensor_gyro_fft_s _sensor_gyro_fft{};

    hrt_abstime _last_update[3][MAX_NUM_PEAKS]{};

    int32_t _imu_gyro_fft_len{256};

    bool _fft_updated{false};
    bool _publish{false};

    DEFINE_PARAMETERS(
        (ParamInt<params_id::IMU_GYRO_FFT_LEN>)_param_imu_gyro_fft_len,
        (ParamFloat<params_id::IMU_GYRO_FFT_MIN>)_param_imu_gyro_fft_min,
        (ParamFloat<params_id::IMU_GYRO_FFT_MAX>)_param_imu_gyro_fft_max,
        (ParamFloat<params_id::IMU_GYRO_FFT_SNR>)_param_imu_gyro_fft_snr)
};

#endif // !GYRO_FFT_HPP
