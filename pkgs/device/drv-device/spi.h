/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/
#pragma once

#ifdef __PX4_NUTTX
#   include "nuttx/SPI.hpp"
#elif defined(__PX4_QURT)
#   include "qurt/SPI.hpp"
#else
#   include "posix/SPI.hpp"
#endif
