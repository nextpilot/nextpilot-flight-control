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
 * Dataman storage backend
 *
 * @group System
 * @value -1 Disabled
 * @value 0 default (SD card)
 * @value 1 RAM (not persistent)
 * @boolean
 * @reboot_required true
 */
PARAM_DEFINE_INT32(SYS_DM_BACKEND, 0);
