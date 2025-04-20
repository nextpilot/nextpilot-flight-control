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

#include "uORBTest_UnitTest.hpp"

extern "C" {
__EXPORT int uorb_tests_main(int argc, char *argv[]);
}

static void usage() {
    PX4_INFO("Usage: uorb_tests [latency_test]");
}

int uorb_tests_main(int argc, char *argv[]) {
    /*
	 * Test the driver/device.
	 */
    if (argc == 1) {
        uORBTest::UnitTest &t  = uORBTest::UnitTest::instance();
        int                 rc = t.test();

        if (rc == OK) {
            PX4_INFO("PASS");
            return 0;

        } else {
            PX4_ERR("FAIL");
            return -1;
        }
    }

    /*
	 * Test the latency.
	 */
    if (argc > 1 && !strcmp(argv[1], "latency_test")) {
        uORBTest::UnitTest &t = uORBTest::UnitTest::instance();
        return t.latency_test(true);
    }

    usage();
    return -EINVAL;
}
