/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <string.h>
#include "uORBAppsProtobufChannel.hpp"
#include "uORB/uORBManager.hpp"

extern "C" {
__EXPORT int muorb_main(int argc, char *argv[]);
__EXPORT int muorb_init();
}

static bool enable_debug = false;

int muorb_main(int argc, char *argv[]) {
    return muorb_init();
}

int muorb_init() {
    uORB::AppsProtobufChannel *channel = uORB::AppsProtobufChannel::GetInstance();

    PX4_INFO("Got muorb init command");

    if (channel && channel->Initialize(enable_debug)) {
        uORB::Manager::get_instance()->set_uorb_communicator(channel);

        if (channel->Test()) {
            return OK;
        }
    }

    return -EINVAL;
}
