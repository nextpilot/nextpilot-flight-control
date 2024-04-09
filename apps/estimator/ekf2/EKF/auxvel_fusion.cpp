/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "ekf.h"

void Ekf::controlAuxVelFusion() {
    if (_auxvel_buffer) {
        auxVelSample auxvel_sample_delayed;

        if (_auxvel_buffer->pop_first_older_than(_time_delayed_us, &auxvel_sample_delayed)) {
            resetEstimatorAidStatus(_aid_src_aux_vel);

            updateVelocityAidSrcStatus(auxvel_sample_delayed.time_us, auxvel_sample_delayed.vel, auxvel_sample_delayed.velVar, fmaxf(_params.auxvel_gate, 1.f), _aid_src_aux_vel);

            if (isHorizontalAidingActive()) {
                _aid_src_aux_vel.fusion_enabled = true;
                fuseVelocity(_aid_src_aux_vel);
            }
        }
    }
}

void Ekf::stopAuxVelFusion() {
    ECL_INFO("stopping aux vel fusion");
    //_control_status.flags.aux_vel = false;
    resetEstimatorAidStatus(_aid_src_aux_vel);
}
