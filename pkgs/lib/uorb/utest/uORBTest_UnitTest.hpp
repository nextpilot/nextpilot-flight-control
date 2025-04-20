/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#ifndef _uORBTest_UnitTest_hpp_
#define _uORBTest_UnitTest_hpp_

#include "defines.h"
#include <uORB/uORB.h>
#include <uORB/topics/orb_test.h>
#include <uORB/topics/orb_test_medium.h>
#include <uORB/topics/orb_test_large.h>
#include <drivers/drv_hrt.h>
#include <errno.h>
#include <unistd.h>

namespace uORBTest {
class UnitTest;
}

class uORBTest::UnitTest {
public:
    // Singleton pattern
    static uORBTest::UnitTest &instance();
    ~UnitTest() = default;

    int test();
    int latency_test(bool print);
    int info();

    // Disallow copy
    UnitTest(const uORBTest::UnitTest & /*unused*/) = delete;

    // Assist in testing the wrap-around situation
    static void set_generation(uORB::DeviceNode &node, unsigned generation) {
        node._generation.store(generation);
    }

private:
    UnitTest() = default;

    static int pubsubtest_threadEntry(int argc, char *argv[]);
    int        pubsublatency_main();

    static int pub_test_multi2_entry(int argc, char *argv[]);
    int        pub_test_multi2_main();

    volatile bool _thread_should_exit;

    bool pubsubtest_passed{false};
    bool pubsubtest_print{false};
    int  pubsubtest_res = OK;

    orb_advert_t _pfd[4]{}; ///< used for test_multi and test_multi_reversed

    int test_single();

    /* These 3 depend on each other and must be called in this order */
    int test_multi();
    int test_multi_reversed();
    int test_unadvertise();

    int test_multi2();

    int test_wrap_around();

    int test_SubscriptionMulti();

    /* queuing tests */
    int          test_queue();
    static int   pub_test_queue_entry(int argc, char *argv[]);
    int          pub_test_queue_main();
    int          test_queue_poll_notify();
    volatile int _num_messages_sent = 0;

    int test_fail(const char *fmt, ...);
    int test_note(const char *fmt, ...);
};

#endif // _uORBTest_UnitTest_hpp_
