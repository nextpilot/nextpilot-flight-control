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
#ifndef BOARD_CONSOLE_BUFFER_SIZE
#define BOARD_CONSOLE_BUFFER_SIZE (1024 * 4) // default buffer size
#endif

static ssize_t console_buffer_write(struct file *filep, const char *buffer, size_t buflen);

class ConsoleBuffer {
public:
    void write(const char *buffer, size_t len);

    void print(bool follow);

    int size();

    int read(char *buffer, int buffer_length, int *offset);

private:
    void lock() {
        rt_sem_take(&_lock, RT_WAITING_FOREVER);
    }
    void unlock() {
        rt_sem_release(&_lock);
    }

    char                _buffer[BOARD_CONSOLE_BUFFER_SIZE];
    int                 _head{0};
    int                 _tail{0};
    struct rt_semaphore _lock = SEM_INITIALIZER(1);
};

void ConsoleBuffer::print(bool follow) {
    // print to stdout, but with a buffer in between to avoid nested locking and potential dead-locks
    // (which could happen in combination with the MAVLink shell: dmesg writing to the pipe waiting
    // mavlink to read, while mavlink calls printf, waiting for the console lock)
    const int buffer_length = 512;
    char      buffer[buffer_length];
    int       offset = -1;

    do {
        // print the full buffer or what's newly added
        int total_size_read = 0;

        do {
            int read_size = read(buffer, buffer_length, &offset);

            if (read_size <= 0) {
                break;
            }

            ::write(1, buffer, read_size);

            if (read_size < buffer_length) {
                break;
            }

            total_size_read += read_size;
        } while (total_size_read < BOARD_CONSOLE_BUFFER_SIZE);

        if (follow) {
            usleep(10000);
        }
    } while (follow);
}

void ConsoleBuffer::write(const char *buffer, size_t len) {
    lock(); // same rule as for printf: this cannot be used from IRQ handlers

    for (size_t i = 0; i < len; ++i) {
        _buffer[_tail] = buffer[i];
        _tail          = (_tail + 1) % BOARD_CONSOLE_BUFFER_SIZE;

        if (_tail == _head) {
            _head = (_head + 1) % BOARD_CONSOLE_BUFFER_SIZE;
        }
    }

    unlock();
}

int ConsoleBuffer::size() {
    lock();
    int size;

    if (_head <= _tail) {
        size = _tail - _head;

    } else {
        size = BOARD_CONSOLE_BUFFER_SIZE - (_head - _tail);
    }

    unlock();
    return size;
}

int ConsoleBuffer::read(char *buffer, int buffer_length, int *offset) {
    lock();

    if (*offset == -1) {
        *offset = _head;
    }

    int size = 0;

    if (*offset < _tail) {
        size = _tail - *offset;

        if (size > buffer_length) {
            size = buffer_length;
        }

        memcpy(buffer, _buffer + *offset, size);

    } else if (_tail < *offset) {
        size = BOARD_CONSOLE_BUFFER_SIZE - *offset;

        if (size > buffer_length) {
            size = buffer_length;
        }

        memcpy(buffer, _buffer + *offset, size);
        buffer += size;
        buffer_length -= size;

        int size_secondary = _tail;

        if (size_secondary > buffer_length) {
            size_secondary = buffer_length;
        }

        if (size_secondary > 0) {
            memcpy(buffer, _buffer, size_secondary);
            size += size_secondary;
        }
    }

    unlock();
    *offset = (*offset + size) % BOARD_CONSOLE_BUFFER_SIZE;
    return size;
}

static ConsoleBuffer g_console_buffer;

void console_buffer_print(bool follow) {
    g_console_buffer.print(follow);
}

ssize_t console_buffer_write(struct file *filep, const char *buffer, size_t len) {
    g_console_buffer.write(buffer, len);

    // stderr still points to our original console and is available from everywhere, so we output to that.
    // We could use up_putc() as well, but it is considerably less efficient.
    // The drawback here is that a module writing to stderr bypasses the console buffer.
    write(2, buffer, len);
    fsync(2);

    return len;
}

static const struct file_operations g_console_buffer_fops = {
    NULL,                 /* open */
    NULL,                 /* close */
    NULL,                 /* read */
    console_buffer_write, /* write */
    NULL,                 /* seek */
    NULL                  /* ioctl */
#ifndef CONFIG_DISABLE_POLL
    ,
    NULL /* poll */
#endif
#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
    ,
    NULL /* unlink */
#endif
};

int console_buffer_init() {
    return register_driver(CONSOLE_BUFFER_DEVICE, &g_console_buffer_fops, 0666, NULL);
}

int console_buffer_size() {
    return g_console_buffer.size();
}

int console_buffer_read(char *buffer, int buffer_length, int *offset) {
    return g_console_buffer.read(buffer, buffer_length, offset);
}

#endif /* BOARD_ENABLE_CONSOLE_BUFFER */
