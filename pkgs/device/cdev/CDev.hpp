/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file CDev.hpp
 *
 * Definitions for the generic base classes in the device framework.
 */

#ifndef _CDEV_HPP
#define _CDEV_HPP

#include <rtthread.h>
#include <defines.h>

typedef struct rt_device file_t;

namespace cdev : public rt_device {

    /**
     * Abstract class for any character device
     */
    class __EXPORT CDev {
    public:
        /**
         * Constructor
         *
         * @param name		Driver name
         * @param devname	Device node name
         */
        explicit CDev(const char *devname);

        // no copy, assignment, move, move assignment
        CDev(const CDev &)            = delete;
        CDev &operator=(const CDev &) = delete;
        CDev(CDev &&)                 = delete;
        CDev &operator=(CDev &&)      = delete;

        virtual ~CDev();

        virtual int init();

        /**
         * Handle an open of the device.
         *
         * This function is called for every open of the device. The default
         * implementation maintains _open_count and always returns OK.
         *
         * @param filep		Pointer to the NuttX file structure.
         * @return		OK if the open is allowed, -errno otherwise.
         */
        virtual int open(rt_uint16_t oflag);

        /**
         * Handle a close of the device.
         *
         * This function is called for every close of the device. The default
         * implementation maintains _open_count and returns OK as long as it is not zero.
         *
         * @param filep		Pointer to the NuttX file structure.
         * @return		OK if the close was successful, -errno otherwise.
         */
        virtual int close();

        /**
         * Perform a read from the device.
         *
         * The default implementation returns -RT_ENOSYS.
         *
         * @param filep		Pointer to the NuttX file structure.
         * @param buffer	Pointer to the buffer into which data should be placed.
         * @param buflen	The number of bytes to be read.
         * @return		The number of bytes read or -errno otherwise.
         */
        virtual ssize_t read(char *buffer, size_t buflen) {
            return -RT_ENOSYS;
        }

        virtual ssize_t read(int offset, char *buffer, size_t buflen) {
            return -RT_ENOSYS;
        }

        /**
         * Perform a write to the device.
         *
         * The default implementation returns -RT_ENOSYS.
         *
         * @param filep		Pointer to the NuttX file structure.
         * @param buffer	Pointer to the buffer from which data should be read.
         * @param buflen	The number of bytes to be written.
         * @return		The number of bytes written or -errno otherwise.
         */
        virtual ssize_t write(const char *buffer, size_t buflen) {
            return -RT_ENOSYS;
        }

        virtual ssize_t write(int offset, const char *buffer, size_t buflen) {
            return -RT_ENOSYS;
        }

        /**
         * Perform a logical seek operation on the device.
         *
         * The default implementation returns -RT_ENOSYS.
         *
         * @param filep		Pointer to the NuttX file structure.
         * @param offset	The new file position relative to whence.
         * @param whence	SEEK_OFS, SEEK_CUR or SEEK_END.
         * @return		The previous offset, or -errno otherwise.
         */
        virtual off_t seek(off_t offset, int whence) {
            return -RT_ENOSYS;
        }

        /**
         * Perform an ioctl operation on the device.
         *
         * The default implementation returns -ENOTTY.
         * Subclasses should call the default implementation
         * for any command they do not handle themselves.
         *
         * @param filep		Pointer to the NuttX file structure.
         * @param cmd		The ioctl command value.
         * @param arg		The ioctl argument value.
         * @return		OK on success, or -errno otherwise.
         */
        virtual int ioctl(int cmd, unsigned long arg) {
            return -ENOTTY;
        }

        /**
         * Perform a poll setup/teardown operation.
         *
         * This is handled internally and should not normally be overridden.
         *
         * @param filep		Pointer to the internal file structure.
         * @param fds		Poll descriptor being waited on.
         * @param setup		True if this is establishing a request, false if
         *			it is being torn down.
         * @return		OK on success, or -errno otherwise.
         */
        // int poll(file_t *filep, px4_pollfd_struct_t *fds, bool setup);

        /**
         * Get the device name.
         *
         * @return the file system string of the device handle
         */
        const char *get_devname() const {
            return _devname;
        }

    protected:
        /**
         * Pointer to the default cdev file operations table; useful for
         * registering clone devices etc.
         */
        // static const px4_file_operations_t fops;

        /**
         * Check the current state of the device for poll events from the
         * perspective of the file.
         *
         * This function is called by the default poll() implementation when
         * a poll is set up to determine whether the poll should return immediately.
         *
         * The default implementation returns no events.
         *
         * @param filep		The file that's interested.
         * @return		The current set of poll events.
         */
        // virtual px4_pollevent_t poll_state(file_t *filep) {
        //     return 0;
        // }

        /**
         * Report new poll events.
         *
         * This function should be called anytime the state of the device changes
         * in a fashion that might be interesting to a poll waiter.
         *
         * @param events	The new event(s) being announced.
         */
        // void poll_notify(px4_pollevent_t events);

        /**
         * Internal implementation of poll_notify.
         *
         * @param fds		A poll waiter to notify.
         * @param events	The event(s) to send to the waiter.
         */
        // virtual void poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events);

        /**
         * Notification of the first open.
         *
         * This function is called when the device open count transitions from zero
         * to one.  The driver lock is held for the duration of the call.
         *
         * The default implementation returns OK.
         *
         * @param filep		Pointer to the NuttX file structure.
         * @return		OK if the open should proceed, -errno otherwise.
         */
        virtual int open_first(rt_uint16_t oflag) {
            return PX4_OK;
        }

        /**
         * Notification of the last close.
         *
         * This function is called when the device open count transitions from
         * one to zero.  The driver lock is held for the duration of the call.
         *
         * The default implementation returns OK.
         *
         * @param filep		Pointer to the NuttX file structure.
         * @return		OK if the open should return OK, -errno otherwise.
         */
        virtual int close_last() {
            return PX4_OK;
        }

        /**
         * Register a class device name, automatically adding device
         * class instance suffix if need be.
         *
         * @param class_devname   Device class name
         * @return class_instamce Class instance created, or -errno on failure
         */
        int register_class_devname(const char *class_devname);

        /**
         * Register a class device name, automatically adding device
         * class instance suffix if need be.
         *
         * @param class_devname   Device class name
         * @param class_instance  Device class instance from register_class_devname()
         * @return		  OK on success, -errno otherwise
         */
        int unregister_class_devname(const char *class_devname, unsigned class_instance);

        /**
         * Take the driver lock.
         *
         * Each driver instance has its own lock/semaphore.
         *
         * Note that we must loop as the wait may be interrupted by a signal.
         *
         * Careful: lock() calls cannot be nested!
         */
        void lock() {
            rt_sem_take(&_lock, RT_WAITING_FOREVER);
        }

        /**
         * Release the driver lock.
         */
        void unlock() {
            rt_sem_release(&_lock)
        }

        struct rt_semaphore _lock {}; /**< lock to protect access to all class members (also for derived classes) */

    private:
        const char *_devname{nullptr}; /**< device node name */

        // px4_pollfd_struct_t **_pollset{nullptr};

        bool _registered{false};      /**< true if device name was registered */

        uint8_t  _max_pollwaiters{0}; /**< size of the _pollset array */
        uint16_t _open_count{0};      /**< number of successful opens */

        /**
         * Store a pollwaiter in a slot where we can find it later.
         *
         * Expands the pollset as required.  Must be called with the driver locked.
         *
         * @return		OK, or -errno on error.
         */
        // inline int store_poll_waiter(px4_pollfd_struct_t *fds);

        /**
         * Remove a poll waiter.
         *
         * @return		OK, or -errno on error.
         */
        // inline int remove_poll_waiter(px4_pollfd_struct_t *fds);
    };

} // namespace cdev:public rt_device

#endif /* _CDEV_HPP */
