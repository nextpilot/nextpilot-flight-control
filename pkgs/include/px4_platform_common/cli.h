

/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2025 NextPilot Development Team
 ******************************************************************/

#ifndef __CLI_H__
#define __CLI_H__

/**
 * @file cli.h
 * Helper methods for command-line parameters
 */

#pragma once


/**
 * Parse a CLI argument to an integer. There are 2 valid formats:
 * - 'p:<param_name>'
 *   in this case the parameter is loaded from an integer parameter
 * - <int>
 *   an integer value, so just a string to integer conversion is done
 * @param option CLI argument
 * @param value returned value
 * @return 0 on success, -errno otherwise
 */
int px4_get_parameter_value(const char *option, int &value);

#endif // __CLI_H__
