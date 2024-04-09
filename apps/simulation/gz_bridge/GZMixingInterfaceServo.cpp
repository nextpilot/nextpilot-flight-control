/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "GZMixingInterfaceServo.hpp"

bool GZMixingInterfaceServo::init(const std::string &model_name) {
    // /model/rascal_110_0/servo_2
    for (int i = 0; i < 8; i++) {
        std::string joint_name  = "servo_" + std::to_string(i);
        std::string servo_topic = "/model/" + model_name + "/" + joint_name;
        // std::cout << "Servo topic: " << servo_topic << std::endl;
        _servos_pub.push_back(_node.Advertise<gz::msgs::Double>(servo_topic));

        if (!_servos_pub.back().Valid()) {
            PX4_ERR("failed to advertise %s", servo_topic.c_str());
            return false;
        }
    }

    ScheduleNow();

    return true;
}

bool GZMixingInterfaceServo::updateOutputs(bool stop_motors, uint16_t outputs[MAX_ACTUATORS], unsigned num_outputs,
                                           unsigned num_control_groups_updated) {
    bool updated = false;
    // cmd.command_value = (float)outputs[i] / 500.f - 1.f; // [-1, 1]

    int i = 0;

    for (auto &servo_pub : _servos_pub) {
        if (_mixing_output.isFunctionSet(i)) {
            gz::msgs::Double servo_output;
            /// TODO: Normalize output data
            double output = (outputs[i] - 500) / 500.0;
            // std::cout << "outputs[" << i << "]: " << outputs[i] << std::endl;
            // std::cout << "  output: " << output << std::endl;
            servo_output.set_data(output);

            if (servo_pub.Valid()) {
                servo_pub.Publish(servo_output);
                updated = true;
            }
        }

        i++;
    }

    return updated;
}

void GZMixingInterfaceServo::Run() {
    pthread_mutex_lock(&_node_mutex);
    _mixing_output.update();
    _mixing_output.updateSubscriptions(false);
    pthread_mutex_unlock(&_node_mutex);
}
