/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#include <console_buffer.h>
#include <defines.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>

#ifdef BOARD_ENABLE_CONSOLE_BUFFER
#   ifndef BOARD_CONSOLE_BUFFER_SIZE
#      define BOARD_CONSOLE_BUFFER_SIZE (1024 * 4) // default buffer size
#   endif

// TODO: User side implementation of px4_console_buffer

int px4_console_buffer_init() {
    return 0;
}

int px4_console_buffer_size() {
    return 0;
}

int px4_console_buffer_read(char *buffer, int buffer_length, int *offset) {
    return 0;
}

#endif /* BOARD_ENABLE_CONSOLE_BUFFER */
