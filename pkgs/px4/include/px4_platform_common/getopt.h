/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2025 NextPilot Development Team
 ******************************************************************/


/**
 * @file getopt.h
 * Thread safe version of getopt
 */

#pragma once

__BEGIN_DECLS

int px4_getopt(int argc, char *argv[], const char *options, int *myoptind, const char **myoptarg);

__END_DECLS
