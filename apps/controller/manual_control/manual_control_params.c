/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * Enable arm/disarm stick gesture
 *
 * This determines if moving the left stick to the lower right
 * arms and to the lower left disarms the vehicle.
 *
 * @boolean
 * @group Manual Control
 */
PARAM_DEFINE_INT32(MAN_ARM_GESTURE, 1);
