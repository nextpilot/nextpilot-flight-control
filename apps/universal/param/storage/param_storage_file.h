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

const char *param_get_default_file();
int         param_set_default_file(const char *filename);

const char *param_get_backup_file();
int         param_set_backup_file(const char *filename);
#endif // __PARAM_STORAGE_FILE_H__
