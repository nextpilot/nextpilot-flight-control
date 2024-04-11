/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <rtthread.h>
#include <stdint.h>
#include <ulog/log.h>

// class instance for primary driver of each class
enum CLASS_DEVICE {
    CLASS_DEVICE_PRIMARY   = 0,
    CLASS_DEVICE_SECONDARY = 1,
    CLASS_DEVICE_TERTIARY  = 2
};

class DeviceBase : public rt_device {
public:
    /**
     * Constructor
     *
     * @param name		Driver name
     * @param devname	Device node name
     */
    explicit DeviceBase(const char *devname);

    // no copy, assignment, move, move assignment
    DeviceBase(const DeviceBase &)            = delete;
    DeviceBase &operator=(const DeviceBase &) = delete;
    DeviceBase(DeviceBase &&)                 = delete;
    DeviceBase &operator=(DeviceBase &&)      = delete;

    virtual ~DeviceBase();

    virtual rt_err_t init() {
        return 0;
    }

    virtual rt_err_t open(rt_uint16_t oflag) {
        return 0;
    }

    virtual rt_err_t close() {
        return 0;
    }

    virtual rt_ssize_t read(rt_off_t pos, void *buff, rt_size_t size) {
        return 0;
    }

    virtual rt_ssize_t write(rt_off_t pos, const void *buffer, rt_size_t size) {
        return 0;
    }

    rt_err_t control(int cmd, void *args) {
        return 0;
    }

    int poll() {
        return 0;
    }

    rt_err_t register_device();

    const char *get_devname() const {
        return _devname;
    }

protected:
    virtual int poll_state() {
        return 0;
    }

    void poll_notify() {
    }

    virtual void poll_notify_one() {
    }

    virtual int open_first() {
        return RT_EOK;
    }

    virtual int close_last() {
        return RT_EOK;
    }

    int register_class_devname(const char *class_devname);

    rt_err_t unregister_class_devname(const char *class_devname, unsigned class_instance);

    void lock() {
        do {
        } while (rt_sem_take(&_lock, RT_WAITING_FOREVER) != 0);
    }

    void unlock() {
        rt_sem_release(&_lock);
    }

    struct rt_semaphore _lock;

private:
    const char *_devname{nullptr};

    // px4_pollfd_struct_t **_pollset{nullptr};

    bool     _registered{false};
    uint8_t  _max_pollwaiters{0};
    uint16_t _open_count{0};

    inline int store_poll_waiter() {
        return 0;
    }

    inline int remove_poll_waiter() {
        return 0;
    }
};

class DeviceDriver {
public:
    // no copy, assignment, move, move assignment
    DeviceDriver(const DeviceDriver &)            = delete;
    DeviceDriver &operator=(const DeviceDriver &) = delete;
    DeviceDriver(DeviceDriver &&)                 = delete;
    DeviceDriver &operator=(DeviceDriver &&)      = delete;

    /**
     * Destructor.
     *
     * Public so that anonymous devices can be destroyed.
     */
    virtual ~DeviceDriver() = default;

    /*
     * Direct access methods.
     */

    /**
     * Initialise the driver and make it ready for use.
     *
     * @return	OK if the driver initialized OK, negative errno otherwise;
     */
    virtual rt_err_t init() {
        return RT_EOK;
    }

    /** Device bus types for DEVID */
    enum DeviceBusType {
        DeviceBusType_UNKNOWN    = 0,
        DeviceBusType_I2C        = 1,
        DeviceBusType_SPI        = 2,
        DeviceBusType_UAVCAN     = 3,
        DeviceBusType_SIMULATION = 4,
        DeviceBusType_SERIAL     = 5,
        DeviceBusType_MAVLINK    = 6,
    };

    /*
      broken out device elements. The bitfields are used to keep
      the overall value small enough to fit in a float accurately,
      which makes it possible to transport over the MAVLink
      parameter protocol without loss of information.
     */
    struct DeviceStructure {
        DeviceBusType bus_type : 3;
        uint8_t       bus : 5; // which instance of the bus type
        uint8_t       address; // address on the bus (eg. I2C address)
        uint8_t       devtype; // device class specific device type
    };

    union DeviceId {
        struct DeviceStructure devid_s;
        uint32_t               devid{0};
    };

    uint32_t get_device_id() const {
        return _device_id.devid;
    }

    /**
     * Return the bus type the device is connected to.
     *
     * @return The bus type
     */
    DeviceBusType get_device_bus_type() const {
        return _device_id.devid_s.bus_type;
    }
    void set_device_bus_type(DeviceBusType bus_type) {
        _device_id.devid_s.bus_type = bus_type;
    }

    static const char *get_device_bus_string(DeviceBusType bus) {
        switch (bus) {
        case DeviceBusType_I2C:
            return "I2C";

        case DeviceBusType_SPI:
            return "SPI";

        case DeviceBusType_UAVCAN:
            return "UAVCAN";

        case DeviceBusType_SIMULATION:
            return "SIMULATION";

        case DeviceBusType_SERIAL:
            return "SERIAL";

        case DeviceBusType_MAVLINK:
            return "MAVLINK";

        case DeviceBusType_UNKNOWN:
        default:
            return "UNKNOWN";
        }
    }

    /**
     * Return the bus ID the device is connected to.
     *
     * @return The bus ID
     */
    uint8_t get_device_bus() const {
        return _device_id.devid_s.bus;
    }
    void set_device_bus(uint8_t bus) {
        _device_id.devid_s.bus = bus;
    }

    /**
     * Return the bus address of the device.
     *
     * @return The bus address
     */
    uint8_t get_device_address() const {
        return _device_id.devid_s.address;
    }
    void set_device_address(int address) {
        _device_id.devid_s.address = address;
    }

    /**
     * Return the device type
     *
     * @return The device type
     */
    uint8_t get_device_type() const {
        return _device_id.devid_s.devtype;
    }
    void set_device_type(uint8_t devtype) {
        _device_id.devid_s.devtype = devtype;
    }

    /**
     * Print decoded device id string to a buffer.
     *
     * @param buffer                        buffer to write to
     * @param length                        buffer length
     * @param id	                        The device id.
     * @param return                        number of bytes written
     */
    static int device_id_print_buffer(char *buffer, int length, uint32_t id) {
        DeviceId dev_id{};
        dev_id.devid = id;

        int num_written = rt_snprintf(buffer, length, "Type: 0x%02X, %s:%d (0x%02X)", dev_id.devid_s.devtype,
                                      get_device_bus_string(dev_id.devid_s.bus_type), dev_id.devid_s.bus, dev_id.devid_s.address);

        buffer[length - 1] = 0; // ensure 0-termination

        return num_written;
    }

    virtual bool external() const {
        return false;
    }

protected:
    union DeviceId _device_id {}; /**< device identifier information */

    const char *_drvname{nullptr};     /**< driver name */
    bool        _debug_enabled{false}; /**< if true, debug messages are printed */

    DeviceDriver() = delete;
    explicit DeviceDriver(const char *drvname) :
        _drvname(drvname) {
    }

    DeviceDriver(uint8_t devtype, const char *drvname, DeviceBusType bus_type, uint8_t bus, uint8_t address) :
        _drvname(drvname) {
        set_device_type(devtype);
        set_device_bus_type(bus_type);
        set_device_bus(bus);
        set_device_address(address);
    }
};

class Device : public DeviceDriver, public DeviceBase {
public:
    Device(const char *drvname, const char *devname) :
        DeviceDriver(drvname),
        DeviceBase(devname) {
    }

    virtual ~Device() = default;

    virtual rt_err_t init() {
        // base class init first
        int ret = DeviceDriver::init();
        if (ret != RT_EOK) {
            return ret;
        }

        // now register the driver
        if (get_devname()) {
            ret = DeviceBase::register_device();
        }

        return ret;
    }
};

#endif // __DEVICE_H__
