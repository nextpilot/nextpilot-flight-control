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
 * Test code for the 2nd order Butterworth low-pass filter
 * Run this test only using make tests TESTFILTER=LowPassFilter2pVector3f
 */

#include <gtest/gtest.h>
#include <matrix/matrix/math.hpp>
#include <px4_platform_common/defines.h>

#include <lib/mathlib/math/filter/LowPassFilter2p.hpp>

using matrix::Vector3f;

class LowPassFilter2pVector3fTest : public ::testing::Test
{
public:
	void runSimulatedFilter(const Vector3f &signal_freq_hz, const Vector3f &phase_delay_deg, const Vector3f &gain_db);

	math::LowPassFilter2p<Vector3f> _lpf{800.f, 30.f};

	const float _epsilon_near = 0.08f;
};

void LowPassFilter2pVector3fTest::runSimulatedFilter(const Vector3f &signal_freq_hz, const Vector3f &phase_delay_deg,
		const Vector3f &gain_db)
{
	const Vector3f phase_delay = phase_delay_deg * M_PI_F / 180.f;
	const Vector3f omega = 2.f * M_PI_F * signal_freq_hz;
	Vector3f gain;

	for (int i = 0; i < 3; i++) {
		gain(i) = powf(10.f, gain_db(i) / 20.f);
	}

	const float dt = 1.f / _lpf.get_sample_freq();
	const int n_steps = roundf(1.f / dt); // run for 1 second

	float t = 0.f;

	for (int i = 0; i < n_steps; i++) {
		Vector3f input{0.f, sinf(omega(1) * t), -sinf(omega(2) * t)};
		Vector3f output_expected{0.f,
					 gain(1) *sinf(omega(1) * t - phase_delay(1)),
					 -gain(2) *sinf(omega(2) * t - phase_delay(2))};
		Vector3f out = _lpf.apply(input);
		t = i * dt;

		// Let some time for the filter to settle
		if (t > 0.3f) {
			EXPECT_EQ(out(0), 0.f);
			EXPECT_NEAR(out(1), output_expected(1), _epsilon_near);
			EXPECT_NEAR(out(2), output_expected(2), _epsilon_near);
		}
	}
}

TEST_F(LowPassFilter2pVector3fTest, setGet)
{
	const float sample_freq = 1000.f;
	const float cutoff_freq = 80.f;

	_lpf.set_cutoff_frequency(sample_freq, cutoff_freq);
	EXPECT_EQ(_lpf.get_sample_freq(), sample_freq);
	EXPECT_EQ(_lpf.get_cutoff_freq(), cutoff_freq);
}

TEST_F(LowPassFilter2pVector3fTest, simulate80HzCutoff)
{
	const float sample_freqs[4] = {400.f, 1000.f, 8000.f, 16000.f};
	const float cutoff_freq = 80.f;

	const Vector3f signal_freq_hz{0.f, 10.f, 100.f};
	const Vector3f phase_delay_deg = Vector3f{0.f, 10.4f, 108.5f}; // Given by simulation
	const Vector3f gain_db{0.f, 0.f, -5.66f}; // given by simulation

	for (float sample_freq : sample_freqs) {
		_lpf.set_cutoff_frequency(sample_freq, cutoff_freq);
		runSimulatedFilter(signal_freq_hz, phase_delay_deg, gain_db);
	}
}
