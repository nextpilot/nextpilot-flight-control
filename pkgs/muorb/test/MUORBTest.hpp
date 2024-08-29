/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

// These are the test types for each of the operations that can happen with muorb
typedef enum {
    ADVERTISE_TEST_TYPE,
    SUBSCRIBE_TEST_TYPE,
    TOPIC_TEST_TYPE,
    UNSUBSCRIBE_TEST_TYPE
} MUORBTestType;

// Common test name between DSP and CPU
extern char muorb_test_topic_name[];

// Check if topic is muorb test marker
#define IS_MUORB_TEST(x)    (strcmp(x, muorb_test_topic_name) == 0)

#define MUORB_TEST_DATA_LEN 8
