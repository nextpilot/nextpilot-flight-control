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
 * @author David Sidrane <david_s5@nscdg.com>
 */

#pragma once
#if defined(UAVCAN_SOCKETCAN_NUTTX)
#include <uavcan_nuttx/uavcan_nuttx.hpp>
#elif defined(UAVCAN_KINETIS_NUTTX)
#include <uavcan_kinetis/uavcan_kinetis.hpp>
#elif defined(UAVCAN_STM32_NUTTX)
#include <uavcan_stm32/uavcan_stm32.hpp>
#elif defined(UAVCAN_STM32H7_NUTTX)
#include <uavcan_stm32h7/uavcan_stm32h7.hpp>
#else
#error "Unsupported driver"
#endif
