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
 * @file Timesync.hpp
 * time synchroniser definition.
 *
 * @author Mohammed Kabir <mhkabir98@gmail.com>
 */

#pragma once

#include <hrtimer.h>

#include <uORB/uORBPublication.hpp>
#include <uORB/topics/timesync_status.h>

#include <math.h>
#include <float.h>

using namespace time_literals;
using namespace nextpilot;

static constexpr time_t PX4_EPOCH_SECS = 1234567890ULL;

// Filter gains
//
// Alpha : Used to smooth the overall clock offset estimate. Smaller values will lead
// to a smoother estimate, but track time drift more slowly, introducing a bias
// in the estimate. Larger values will cause low-amplitude oscillations.
//
// Beta : Used to smooth the clock skew estimate. Smaller values will lead to a
// tighter estimation of the skew (derivative), but will negatively affect how fast the
// filter reacts to clock skewing (e.g cause by temperature changes to the oscillator).
// Larger values will cause large-amplitude oscillations.
static constexpr double ALPHA_GAIN_INITIAL = 0.05;
static constexpr double BETA_GAIN_INITIAL = 0.05;
static constexpr double ALPHA_GAIN_FINAL = 0.003;
static constexpr double BETA_GAIN_FINAL = 0.003;

// Filter gain scheduling
//
// The filter interpolates between the INITIAL and FINAL gains while the number of
// exhanged timesync packets is less than CONVERGENCE_WINDOW. A lower value will
// allow the timesync to converge faster, but with potentially less accurate initial
// offset and skew estimates.
static constexpr uint32_t CONVERGENCE_WINDOW = 500;

// Outlier rejection and filter reset
//
// Samples with round-trip time higher than MAX_RTT_SAMPLE are not used to update the filter.
// More than MAX_CONSECUTIVE_HIGH_RTT number of such events in a row will throw a warning
// but not reset the filter.
// Samples whose calculated clock offset is more than MAX_DEVIATION_SAMPLE off from the current
// estimate are not used to update the filter. More than MAX_CONSECUTIVE_HIGH_DEVIATION number
// of such events in a row will reset the filter. This usually happens only due to a time jump
// on the remote system.
// TODO : automatically determine these using ping statistics?
static constexpr uint64_t MAX_RTT_SAMPLE = 10_ms;
static constexpr uint64_t MAX_DEVIATION_SAMPLE = 100_ms;
static constexpr uint32_t MAX_CONSECUTIVE_HIGH_RTT = 5;
static constexpr uint32_t MAX_CONSECUTIVE_HIGH_DEVIATION = 5;

class Timesync
{
public:
	Timesync(uint8_t source = timesync_status_s::SOURCE_PROTOCOL_UNKNOWN) : _source(source) {};
	~Timesync() = default;

	void update(const uint64_t now_us, const int64_t remote_timestamp_ns, int64_t originate_timestamp_ns);

	/**
	 * Convert remote timestamp to local hrt time (usec)
	 * Use synchronised time if available, monotonic boot time otherwise
	 */
	uint64_t sync_stamp(uint64_t usec);

	int64_t offset() const { return (int64_t)_time_offset; }

private:

	/**
	 * Online exponential filter to smooth time offset
	 */
	void add_sample(int64_t offset_us);

	/**
	 * Return true if the timesync algorithm converged to a good estimate,
	 * return false otherwise
	 */
	bool sync_converged() const { return _sequence >= CONVERGENCE_WINDOW; }

	/**
	 * Reset the exponential filter and its states
	 */
	void reset_filter();

	uORB::PublicationMulti<timesync_status_s>  _timesync_status_pub{ORB_ID(timesync_status)};

	uint32_t _sequence{0};

	// Timesync statistics
	double _time_offset{0};
	double _time_skew{0};

	// Filter parameters
	double _filter_alpha{ALPHA_GAIN_INITIAL};
	double _filter_beta{BETA_GAIN_INITIAL};

	// Outlier rejection and filter reset
	uint32_t _high_deviation_count{0};
	uint32_t _high_rtt_count{0};

	uint8_t _source{};
};
