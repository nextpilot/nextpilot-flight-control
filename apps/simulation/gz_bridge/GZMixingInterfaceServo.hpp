/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <lib/mixer_module/mixer_module.hpp>

#include <gz/transport.hh>

// GZBridge mixing class for Servos.
// It is separate from GZBridge to have separate WorkItems and therefore allowing independent scheduling
// All work items are expected to run on the same work queue.
class GZMixingInterfaceServo : public OutputModuleInterface {
public:
    GZMixingInterfaceServo(gz::transport::Node &node, pthread_mutex_t &node_mutex) :
        OutputModuleInterface(MODULE_NAME "-actuators-servo", px4::wq_configurations::rate_ctrl),
        _node(node),
        _node_mutex(node_mutex) {
    }

    bool updateOutputs(bool stop_motors, uint16_t outputs[MAX_ACTUATORS],
                       unsigned num_outputs, unsigned num_control_groups_updated) override;

    MixingOutput &mixingOutput() {
        return _mixing_output;
    }

    bool init(const std::string &model_name);

    void stop() {
        _mixing_output.unregister();
        ScheduleClear();
    }

private:
    friend class GZBridge;

    void Run() override;

    gz::transport::Node &_node;
    pthread_mutex_t     &_node_mutex;

    MixingOutput _mixing_output{"SIM_GZ_SV", MAX_ACTUATORS, *this, MixingOutput::SchedulingPolicy::Auto, false, false};

    std::vector<gz::transport::Node::Publisher> _servos_pub;
};
