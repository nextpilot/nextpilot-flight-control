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
 * @file console_buffer.h
 * This implements a simple console buffer to store the full bootup output.
 * It can be printed via the 'dmesg' utility.
 * The output of stdout is redirected to CONSOLE_BUFFER_DEVICE, which is stored
 * to a circular buffer and output to stderr, so that everything is still printed
 * to the original console.
 */

#pragma once

#ifndef __CONSOLE_BUFFER_H__
#define __CONSOLE_BUFFER_H__

#define CONSOLE_BUFFER_DEVICE "/dev/console_buf"

#ifdef BOARD_ENABLE_CONSOLE_BUFFER

__BEGIN_DECLS

/**
 * Initialize the console buffer: register the CONSOLE_BUFFER_DEVICE
 * @return 0 on success, <0 error otherwise
 */
int console_buffer_init();

/**
 * Print content of the console buffer to stdout
 * @param follow if true keep waiting and print new content whenever the buffer
 *               is updated
 */
void console_buffer_print(bool follow);

/**
 * Get the current used buffer size
 */
int console_buffer_size();

/**
 * Read (chunks) of the console buffer.
 * Note that no lock is held between reading multiple chunks, so the buffer could get
 * updated meanwhile. Use console_buffer_size() to read no more than expected.
 * @param buffer output buffer
 * @param buffer_length output buffer length
 * @param offset input and output argument for the offset. Initially set this to -1.
 * @return number of bytes written to the buffer (or <0 on error)
 */
int console_buffer_read(char *buffer, int buffer_length, int *offset);

__END_DECLS

#else

static inline int console_buffer_init() {
    return 0;
}

static inline int console_buffer_size() {
    return 0;
}

static inline int console_buffer_read(char *buffer, int buffer_length, int *offset) {
    return 0;
}

#endif /* BOARD_ENABLE_CONSOLE_BUFFER */

#endif // __CONSOLE_BUFFER_H__
