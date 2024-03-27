/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

void param_notify_changes() {
#ifdef SYS_USING_UORB
    static uint32_t           param_instance = 0;
    struct parameter_update_s pup;
    pup.instance  = param_instance++;
    pup.timestamp = hrt_absolute_time();
    orb_publish(ORB_ID(parameter_update), NULL, &pup);
#endif // SYS_USING_UORB
}
