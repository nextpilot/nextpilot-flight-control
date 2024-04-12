/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <gtest/gtest.h>
#include <math.h>
#include <mathlib/mathlib.h>
#include <memory>
#include <geo/geo.h>

class GeoTest : public ::testing::Test
{
public:
	void SetUp() override
	{
		proj.initReference(math::radians(473566094 / 1e7), math::radians(85190237 / 1e7), 0);
	}

protected:
	MapProjection proj;

};


TEST_F(GeoTest, reprojectProject)
{
	// GIVEN: x and y coordinates in the local cartesian frame
	float x = 0.5;
	float y = 1;
	double lat;
	double lon;
	proj.reproject(x, y, lat, lon);
	float x_new;
	float y_new;
	proj.project(lat, lon, x_new, y_new);
	double lat_new;
	double lon_new;
	proj.reproject(x_new, y_new, lat_new, lon_new);
	EXPECT_FLOAT_EQ(x, x_new);
	EXPECT_FLOAT_EQ(y, y_new);
	EXPECT_FLOAT_EQ(lat, lat_new);
	EXPECT_FLOAT_EQ(lon, lon_new);
}

TEST_F(GeoTest, projectReproject)
{
	// GIVEN: x and y coordinates in the local cartesian frame
	double lat = 47.356616973876953;
	double lon = 8.5190505981445313;
	float x;
	float y;
	proj.project(lat, lon, x, y);
	double lat_new;
	double lon_new;
	proj.reproject(x, y, lat_new, lon_new);
	float x_new;
	float y_new;
	proj.project(lat_new, lon_new, x_new, y_new);
	// WHEN: apply the mapping and its inverse the output should stay the same
	EXPECT_FLOAT_EQ(x, x_new);
	EXPECT_FLOAT_EQ(y, y_new);
	EXPECT_FLOAT_EQ(lat, lat_new);
	EXPECT_FLOAT_EQ(lon, lon_new);
}

TEST_F(GeoTest, waypoint_from_heading_and_zero_distance)
{
	// GIVEN: a starting waypoint, a heading and a distance of 0
	double lat_start = -33;
	double lon_start = 18;
	float bearing = 0;
	float dist = 0;

	double lat_target = 0;
	double lon_target = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_distance(lat_start, lon_start, bearing, dist, &lat_target, &lon_target);

	// THEN: it should be the same
	EXPECT_DOUBLE_EQ(lat_start, lat_target);
	EXPECT_DOUBLE_EQ(lon_start, lon_target);
}


TEST_F(GeoTest, waypoint_from_heading_and_negative_distance)
{
	// GIVEN: a starting waypoint, a heading and negative distance
	double lat_start = -33;
	double lon_start = 18;
	float bearing = 0;
	float lat_offset = -0.01f;
	float dist = CONSTANTS_RADIUS_OF_EARTH_F * M_PI_F / 180.f * lat_offset;

	double lat_target = 0;
	double lon_target = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_distance(lat_start, lon_start, bearing, dist, &lat_target, &lon_target);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(static_cast<float>(lat_start) + lat_offset, lat_target);
	EXPECT_DOUBLE_EQ(lon_start, lon_target);
}

TEST_F(GeoTest, waypoint_from_heading_and_positive_distance)
{
	// GIVEN: a starting waypoint, a heading and positive distance
	double lat_start = -33;
	double lon_start = 18;
	float bearing = 0;
	float lat_offset = 0.01f;
	float dist = CONSTANTS_RADIUS_OF_EARTH_F * M_PI_F / 180.f * lat_offset;

	double lat_target = 0;
	double lon_target = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_distance(lat_start, lon_start, bearing, dist, &lat_target, &lon_target);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(static_cast<float>(lat_start) + lat_offset, lat_target);
	EXPECT_DOUBLE_EQ(lon_start, lon_target);
}

TEST_F(GeoTest, waypoint_from_line_and_zero_distance)
{
	// GIVEN: a starting waypoint, a heading and a distance of 0
	double lat_start = -33;
	double lon_start = 18;
	double lat_end = -32;
	double lon_end = 18;
	float dist = 0;

	double lat_target = 0;
	double lon_target = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_line_and_dist(lat_start, lon_start, lat_end, lon_end, dist, &lat_target, &lon_target);

	// THEN: it should be the same
	EXPECT_DOUBLE_EQ(lat_start, lat_target);
	EXPECT_DOUBLE_EQ(lon_start, lon_target);
}

TEST_F(GeoTest, waypoint_from_line_and_positive_distance)
{
	// GIVEN: a starting waypoint, a heading and a positive distance
	double lat_start = -33;
	double lon_start = 18;
	double lat_offset = 0.01;
	double lat_end = lat_start + lat_offset;
	double lon_end = 18;
	float dist = CONSTANTS_RADIUS_OF_EARTH_F * M_PI_F / 180.f * static_cast<float>(lat_offset);

	double lat_target = 0;
	double lon_target = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_line_and_dist(lat_start, lon_start, lat_end, lon_end, dist, &lat_target, &lon_target);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(lat_start + lat_offset, lat_target);
	EXPECT_DOUBLE_EQ(lon_start, lon_target);
}


TEST_F(GeoTest, waypoint_from_line_and_negative_distance)
{
	// GIVEN: a starting waypoint, a heading and a negative distance
	double lat_start = -33;
	double lon_start = 18;
	double lat_offset = 0.01;
	double lat_end = lat_start + lat_offset;
	double lon_end = 18;
	float dist = -CONSTANTS_RADIUS_OF_EARTH_F * M_PI_F / 180.f * static_cast<float>(lat_offset);

	double lat_target = 0;
	double lon_target = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_line_and_dist(lat_start, lon_start, lat_end, lon_end, dist, &lat_target, &lon_target);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(lat_start - lat_offset, lat_target);
	EXPECT_DOUBLE_EQ(lon_start, lon_target);
}
