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
 * @file default_tunes.h
 */

#include "tunes.h"

#define PX4_DEFINE_TUNE(ordinal,name,tune,interruptable) tune,
// Initialize default tunes
const char *const Tunes::_default_tunes[] = {
#include "tune_definition.desc"
};
#undef PX4_DEFINE_TUNE

#define PX4_DEFINE_TUNE(ordinal,name,tune,interruptable) interruptable,
// Initialize default tunes
const bool Tunes::_default_tunes_interruptable[] = {
#include "tune_definition.desc"
};
#undef PX4_DEFINE_TUNE

// set default_tunes array size
const unsigned int Tunes::_default_tunes_size =  sizeof(_default_tunes) / sizeof(_default_tunes[0]);
