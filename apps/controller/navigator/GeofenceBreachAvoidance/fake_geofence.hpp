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
 * @file fake_geofence.h
 * Provides a fake geofence interface to use for testing
 *
 * @author Roman Bapst
 * @author Julian Kent
 */
#pragma once

#include "../geofence.h"
#include <lib/mathlib/mathlib.h>

class FakeGeofence : public Geofence {
public:
    FakeGeofence() :
        Geofence(nullptr){};

    virtual ~FakeGeofence(){};

    bool isInsidePolygonOrCircle(double lat, double lon, float altitude) override {
        switch (_probe_function_behavior) {
        case ProbeFunction::ALL_POINTS_OUTSIDE: {
            return _allPointsOutside(lat, lon, altitude);
        }

        case ProbeFunction::LEFT_INSIDE_RIGHT_OUTSIDE: {
            return _left_inside_right_outside(lat, lon, altitude);
        }

        case ProbeFunction::RIGHT_INSIDE_LEFT_OUTSIDE: {
            return _right_inside_left_outside(lat, lon, altitude);
        }

        case ProbeFunction::GF_BOUNDARY_20M_AHEAD: {
            return _gf_boundary_is_20m_north(lat, lon, altitude);
        }

        default:
            return _allPointsOutside(lat, lon, altitude);
        }
    }

    enum class ProbeFunction {
        ALL_POINTS_OUTSIDE = 0,
        LEFT_INSIDE_RIGHT_OUTSIDE,
        RIGHT_INSIDE_LEFT_OUTSIDE,
        GF_BOUNDARY_20M_AHEAD
    };

    void setProbeFunctionBehavior(ProbeFunction func) {
        _probe_function_behavior = func;
    }

private:
    ProbeFunction _probe_function_behavior = ProbeFunction::ALL_POINTS_OUTSIDE;

    bool _flag_on_left  = true;
    bool _flag_on_right = false;

    bool _allPointsOutside(double lat, double lon, float alt) {
        return false;
    }

    bool _left_inside_right_outside(double lat, double lon, float alt) {
        if (_flag_on_left) {
            _flag_on_left = false;
            return true;

        } else {
            return false;
        }
    }

    bool _right_inside_left_outside(double lat, double lon, float alt) {
        if (_flag_on_right) {
            _flag_on_right = false;
            return true;

        } else {
            _flag_on_right = true;
            return false;
        }
    }

    bool _gf_boundary_is_20m_north(double lat, double lon, float alt) {
        matrix::Vector2<double> home_global(42.1, 8.2);

        MapProjection    projection{home_global(0), home_global(1)};
        matrix::Vector2f waypoint_local = projection.project(lat, lon);

        if (waypoint_local(0) >= 20.0f) {
            return false;
        }

        return true;
    }
};
