#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

File: wind_estimator_replay.py
Author: Mathieu Bresciani <mathieu@auterion.com>
License: BSD 3-Clause
Description:
"""

import matplotlib.pylab as plt
from pyulog import ULog
import numpy as np

from generated.fuse_airspeed import fuse_airspeed


def getData(log, topic_name, variable_name, instance=0):
    variable_data = np.array([])
    for elem in log.data_list:
        if elem.name == topic_name:
            if instance == elem.multi_id:
                variable_data = elem.data[variable_name]
                break

    return variable_data


def us2s(time_ms):
    return time_ms * 1e-6


def run(logfile, use_gnss):
    log = ULog(logfile)

    if use_gnss:
        v_local = np.array(
            [
                getData(log, "vehicle_gps_position", "vel_n_m_s"),
                getData(log, "vehicle_gps_position", "vel_e_m_s"),
                getData(log, "vehicle_gps_position", "vel_d_m_s"),
            ]
        )
        t_v_local = us2s(getData(log, "vehicle_gps_position", "timestamp"))

    else:
        v_local = np.array(
            [
                getData(log, "vehicle_local_position", "vx"),
                getData(log, "vehicle_local_position", "vy"),
                getData(log, "vehicle_local_position", "vz"),
            ]
        )
        t_v_local = us2s(getData(log, "vehicle_local_position", "timestamp"))

    true_airspeed = getData(log, "airspeed", "true_airspeed_m_s")
    t_true_airspeed = us2s(getData(log, "airspeed", "timestamp"))

    dist_bottom = getData(log, "vehicle_local_position", "dist_bottom")
    t_dist_bottom = us2s(getData(log, "vehicle_local_position", "timestamp"))

    state = np.array([0.0, 0.0, 1.0])
    P = np.diag([1.0, 1.0, 1e-4])
    wind_nsd = 1e-2
    scale_nsd = 1e-4
    Q = np.diag([wind_nsd**2, wind_nsd**2, scale_nsd**2])
    R = 1.4**2
    epsilon = 1e-8
    t_now = t_v_local[0]

    n = len(t_v_local)
    wind_est_n = np.zeros(n)
    wind_est_e = np.zeros(n)
    scale_est = np.zeros(n)
    i_airspeed = 0
    i_dist_bottom = 0

    for i in range(n):
        dt = t_v_local[i] - t_now
        t_now = t_v_local[i]  # run on local position updates

        while i_dist_bottom < len(t_dist_bottom) and t_dist_bottom[i_dist_bottom] <= t_now:
            i_dist_bottom += 1
        i_dist_bottom -= 1

        if dist_bottom[i_dist_bottom] > 20.0:  # Don't start too low

            P += Q * dt

            if t_true_airspeed[i_airspeed] < t_now:
                while i_airspeed < len(t_true_airspeed) and t_true_airspeed[i_airspeed] < t_now:
                    i_airspeed += 1
                i_airspeed -= 1

                (H, K, innov_var, innov) = fuse_airspeed(
                    np.asarray(v_local[:, i]), state, P.flatten(), true_airspeed[i_airspeed], R, epsilon
                )
                state += np.array(K) * innov
                P -= K * H * P
                i_airspeed += 1

        wind_est_n[i] = state[0]
        wind_est_e[i] = state[1]
        scale_est[i] = state[2]

    plt.figure(1)
    ax1 = plt.subplot(2, 1, 1)
    ax1.plot(t_v_local, wind_est_n)
    ax1.plot(t_v_local, wind_est_e)
    ax1.set_ylabel("wind speed (m/s)")
    ax1.legend(["north", "east"])

    ax2 = plt.subplot(2, 1, 2)
    ax2.plot(t_v_local, scale_est)
    ax2.set_xlabel("time (s)")
    ax2.set_ylabel("airspeed scale (-)")
    plt.show()


if __name__ == "__main__":
    import os
    import argparse

    # Get the path of this script (without file name)
    script_path = os.path.split(os.path.realpath(__file__))[0]

    # Parse arguments
    parser = argparse.ArgumentParser(description="Wind estimator with airspeed scale factor")

    # Provide parameter file path and name
    parser.add_argument("logfile", help="Full ulog file path, name and extension", type=str)
    parser.add_argument("--gnss", help="Use GNSS velocity instead of local velocity estimate", action="store_true")
    args = parser.parse_args()

    logfile = os.path.abspath(args.logfile)  # Convert to absolute path

    run(logfile, args.gnss)
