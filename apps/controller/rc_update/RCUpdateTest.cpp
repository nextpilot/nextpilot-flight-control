/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define MODULE_NAME "rc_update"

#include <gtest/gtest.h>
#include "rc_update.h"

using namespace rc_update;

TEST(RCUpdateTest, ModeSlotUnassigned) {
    RCUpdate rc_update;
    // GIVEN: Default configuration with no assigned mode switch
    EXPECT_EQ(rc_update._param_rc_map_fltmode.get(), 0);

    // WHEN: we update the switches two times to pass the simple outlier protection
    rc_update.UpdateManualSwitches(0);
    rc_update.UpdateManualSwitches(0);

    // THEN: we receive no mode slot
    uORB::SubscriptionData<manual_control_switches_s> manual_control_switches_sub{ORB_ID(manual_control_switches)};
    manual_control_switches_sub.update();

    EXPECT_EQ(manual_control_switches_sub.get().mode_slot, 0); // manual_control_switches_s::MODE_SLOT_NONE
}

void checkModeSlotSwitch(float channel_value, uint8_t expected_slot) {
    RCUpdate rc_update;

    // GIVEN: First channel is configured as mode switch
    rc_update._param_rc_map_fltmode.set(1);
    EXPECT_EQ(rc_update._param_rc_map_fltmode.get(), 1);
    // GIVEN: First channel has some value
    rc_update._rc.channels[0] = channel_value;

    // WHEN: we update the switches two times to pass the simple outlier protection
    rc_update.UpdateManualSwitches(0);
    rc_update.UpdateManualSwitches(0);

    // THEN: we receive the expected mode slot
    uORB::SubscriptionData<manual_control_switches_s> manual_control_switches_sub{ORB_ID(manual_control_switches)};
    manual_control_switches_sub.update();

    EXPECT_EQ(manual_control_switches_sub.get().mode_slot, expected_slot);
}

TEST(RCUpdateTest, ModeSlotSwitchAllValues) {
    checkModeSlotSwitch(-1.f, 1); // manual_control_switches_s::MODE_SLOT_1
    checkModeSlotSwitch(-.5f, 2); // manual_control_switches_s::MODE_SLOT_2
    checkModeSlotSwitch(-.1f, 3); // manual_control_switches_s::MODE_SLOT_3
    checkModeSlotSwitch(0.f, 4);  // manual_control_switches_s::MODE_SLOT_4
    checkModeSlotSwitch(.5f, 5);  // manual_control_switches_s::MODE_SLOT_5
    checkModeSlotSwitch(1.f, 6);  // manual_control_switches_s::MODE_SLOT_6
}

void checkModeSlotButton(uint8_t button_configuration, uint8_t channel, float channel_value, uint8_t expected_slot) {
    RCUpdate rc_update;

    // GIVEN: Buttons are configured
    rc_update._param_rc_map_fltm_btn.set(button_configuration);
    EXPECT_EQ(rc_update._param_rc_map_fltm_btn.get(), button_configuration);
    // GIVEN: buttons are mapped
    rc_update.update_rc_functions();
    // GIVEN: First channel has some value
    rc_update._rc.channels[channel - 1] = channel_value;

    // WHEN: we update the switches 4 times:
    // - initiate the button press
    // - keep the same button pressed
    // - hold the button for 50ms
    // - pass the simple outlier protection
    rc_update.UpdateManualSwitches(0);
    rc_update.UpdateManualSwitches(0);
    rc_update.UpdateManualSwitches(51_ms);
    rc_update.UpdateManualSwitches(51_ms);

    // THEN: we receive the expected mode slot
    uORB::SubscriptionData<manual_control_switches_s> manual_control_switches_sub{ORB_ID(manual_control_switches)};
    manual_control_switches_sub.update();

    EXPECT_EQ(manual_control_switches_sub.get().mode_slot, expected_slot);
}

TEST(RCUpdateTest, ModeSlotButtonAllValues) {
    checkModeSlotButton(1, 1, -1.f, 0); // button not pressed -> manual_control_switches_s::MODE_SLOT_NONE
    checkModeSlotButton(1, 1, 0.f, 0);  // button not pressed over threshold -> manual_control_switches_s::MODE_SLOT_NONE
    checkModeSlotButton(1, 1, 1.f, 1);  // button 1 pressed -> manual_control_switches_s::MODE_SLOT_1
    checkModeSlotButton(1, 2, 1.f, 0);  // button 2 pressed but not configured -> manual_control_switches_s::MODE_SLOT_NONE
    checkModeSlotButton(3, 2, 1.f, 2);  // button 2 pressed -> manual_control_switches_s::MODE_SLOT_2
    checkModeSlotButton(3, 3, 1.f, 0);  // button 3 pressed but not configured -> manual_control_switches_s::MODE_SLOT_NONE
    checkModeSlotButton(7, 3, 1.f, 3);  // button 3 pressed -> manual_control_switches_s::MODE_SLOT_3
    checkModeSlotButton(7, 4, 1.f, 0);  // button 4 pressed but not configured -> manual_control_switches_s::MODE_SLOT_NONE
    checkModeSlotButton(15, 4, 1.f, 4); // button 4 pressed -> manual_control_switches_s::MODE_SLOT_4
    checkModeSlotButton(15, 5, 1.f, 0); // button 5 pressed but not configured -> manual_control_switches_s::MODE_SLOT_NONE
    checkModeSlotButton(31, 5, 1.f, 5); // button 5 pressed -> manual_control_switches_s::MODE_SLOT_5
    checkModeSlotButton(31, 6, 1.f, 0); // button 6 pressed but not configured -> manual_control_switches_s::MODE_SLOT_NONE
    checkModeSlotButton(63, 6, 1.f, 6); // button 6 pressed -> manual_control_switches_s::MODE_SLOT_6
}
