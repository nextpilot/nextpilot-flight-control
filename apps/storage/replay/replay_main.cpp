/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "Replay.hpp"

using namespace px4;

extern "C" __EXPORT int
replay_main(int argc, char *argv[]) {
    // check for logfile env variable
    const char *logfile = getenv(replay::ENV_FILENAME);

    if (logfile && !Replay::isSetup()) {
        PX4_INFO("using replay log file: %s", logfile);
        Replay::setupReplayFile(logfile);
    }

    return Replay::main(argc, argv);
}
