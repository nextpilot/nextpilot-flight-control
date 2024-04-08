/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __PARAM_STORAGE_FILE_H__
#define __PARAM_STORAGE_FILE_H__

#include "param_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the default parameter file name.
 *
 * @return		The path to the current default parameter file; either as
 *			a result of a call to param_set_default_file, or the
 *			built-in default.
 */
const char *param_get_default_file();

/**
 * Set the default parameter file name.
 * This has no effect if the FLASH-based storage is enabled.
 *
 * @param filename	Path to the default parameter file.  The file is not required to
 *			exist.
 * @return		Zero on success.
 */
int param_set_default_file(const char *filename);

/**
 * Get the backup parameter file name.
 *
 * @return		The path to the backup parameter file
 */
const char *param_get_backup_file();

/**
 * Set the backup parameter file name.
 *
 * @param filename	Path to the backup parameter file. The file is not required to
 *			exist.
 * @return		Zero on success.
 */
int param_set_backup_file(const char *filename);

#ifdef __cplusplus
}
#endif

#endif // __PARAM_STORAGE_FILE_H__
