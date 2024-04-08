/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __MODULE_COMMAND_H__
#define __MODULE_COMMAND_H__

#include <stdint.h>
#include <rtthread.h>
#include <rtdbg.h>
#include <module_usage.h>

template <class T, uint8_t N = 1>
class ModuleCommand {
public:
    ModuleCommand() :
        _task_should_exit{false} {
    }

    virtual ~ModuleCommand() {
    }

    /**
     * @brief main Main entry point to the module that should be
     *        called directly from the module's main method.
     * @param argc The task argument count.
     * @param argc Pointer to the task argument variable array.
     * @return Returns 0 iff successful, -1 otherwise.
     */
    static int main(int argc, char *argv[]) {
        if (argc <= 1 ||
            strcmp(argv[1], "-h") == 0 ||
            strcmp(argv[1], "help") == 0 ||
            strcmp(argv[1], "info") == 0 ||
            strcmp(argv[1], "usage") == 0) {
            return T::custom_usage();
        }

        if (strcmp(argv[1], "start") == 0) {
            // Pass the 'start' argument too, because later on px4_getopt() will ignore the first argument.
            return start_command(argc - 1, argv + 1);
        }

        if (strcmp(argv[1], "status") == 0) {
            return status_command();
        }

        if (strcmp(argv[1], "stop") == 0) {
            return stop_command();
        }

        // lock_module(); // Lock here, as the method could access _object.
        int ret = T::custom_command(argc - 1, argv + 1);
        // unlock_module();

        return ret;
    }

    /**
     * @brief Stars the command, ('command start'), checks if if is already
     *        running and calls T::task_spawn() if it's not.
     * @param argc The task argument count.
     * @param argc Pointer to the task argument variable array.
     * @return Returns 0 iff successful, -1 otherwise.
     */

    static int start_command(int argc, char *argv[]) {
        // 获取实例个数
        int count = 0;
        lock_module();
        for (int idx = 0; idx < N; idx++) {
            if (_object[idx].load()) {
                count++;
            }
        }
        unlock_module();

        if (count >= N) {
            LOG_E("already have %d instance, can't start more", N);
            return -1;
        }

        // 创建实例
        T *object = instantiate(argc, argv);
        if (!object) {
            LOG_E("create instance fail");
            return -1;
        }

        // 初始化实例
        if (object->init() != 0) {
            LOG_E("init instance fail");
            delete object;
            return -1;
        }

        // 添加到列表中
        int inst = -1;
        lock_module();
        for (int idx = 0; inst < N; idx++) {
            if (!_object[idx].load()) {
                inst = idx;
                _object[idx].store(object);
                break;
            }
        }
        unlock_module();

        if (inst != -1) {
            LOG_I("start instance #%d ok", inst);
        }

        return 0;
    }

    /**
     * @brief Stops the command, ('command stop'), checks if it is running and if it is, request the module to stop
     *        and waits for the task to complete.
     * @return Returns 0 iff successful, -1 otherwise.
     */
    static int stop_command(int inst = -1) {
        int start = 0;
        int end   = N;

        if (inst >= N) {
            // LOG_E("")
            return -1;
        } else if (inst >= 0) {
            start = inst;
            end   = inst;
        }

        bool found = false;
        for (int idx = start; idx < end; idx++) {
            T *object = _object[idx].load();
            if (object) {
                found = true;

                // 请求停止
                object->request_stop();
                // TODO：这里应该判断是否真的停止
                // 删除实例
                delete object;
                _object[idx].store(nullptr);
                LOG_I("request stop instance #%d", idx);

                // do {
                //     rt_thread_mdelay(10);

                //     if (++i > 500) {
                //         delete object;
                //         _object[idx].store(nullptr);
                //     }

                // } while (1)
            }
        }

        if (!found) {
            LOG_W("no instance running");
        }

        return 0;
    }

    /**
     * @brief Handle 'command status': check if running and call print_status() if it is
     * @return Returns 0 iff successful, -1 otherwise.
     */
    static int status_command() {
        bool found = false;
        // lock_module();
        for (int inst = 0; inst < N; inst++) {
            T *object = _object[inst].load();
            if (object) {
                found = true;
                LOG_I("instance #%d status:", inst);
                object->print_status();
            }
        }
        // unlock_module();

        if (!found) {
            LOG_W("no instance running");
        }
        return 0;
    }

    static int custom_command(int argc, char *argv[]) {
        // support for custom commands
        // it none are supported, just do:
        return custom_usage("unrecognized command");
    }

    static int custom_usage(const char *reason = nullptr) {
        // use the PRINT_MODULE_* methods...
        if (reason) {
            LOG_W(reason);
        }

        LOG_RAW("start\tstart a new instance\n");
        LOG_RAW("stop\tstop a exist instance\n");
        LOG_RAW("status\tshow status\n");
        LOG_RAW("help\tshow help\n");

        return 0;
    }

    static T *instantiate(int argc, char *argv[]) {
        return new T();
    }

    /**
     * @brief Print the status if the module is running. This can be overridden by the module to provide
     * more specific information.
     * @return Returns 0 iff successful, -1 otherwise.
     */
    virtual int print_status() {
        rt_kprintf("running, addr=%x\n", this);
        return 0;
    }

    virtual int init() {
        return 0;
    }

    /**
     * @brief Main loop method for modules running in their own thread. Called from run_trampoline().
     *        This method must return when should_exit() returns true.
     */
    virtual void run() {
    }

    /**
     * @brief Returns the status of the module.
     * @return Returns true if the module is running, false otherwise.
     */
    static bool is_running(int inst = 0) {
        return _object[inst].load != nullptr;
    }

protected:
    /**
     * @brief Tells the module to stop (used from outside or inside the module thread).
     */
    virtual void request_stop() {
        _task_should_exit.store(true);
    }

    /**
     * @brief Checks if the module should stop (used within the module thread).
     * @return Returns True iff successful, false otherwise.
     */
    bool should_exit() const {
        return _task_should_exit.load();
    }

    /**
     * @brief Exits the module and delete the object. Called from within the module's thread.
     *        For work queue modules, this needs to be called from the derived class in the
     *        cycle method, when should_exit() returns true.
     */
    static void exit_and_cleanup() {
        // Take the lock here:
        // - if startup fails and we're faster than the parent thread, it will set
        //   _task_id and subsequently it will look like the task is running.
        // - deleting the object must take place inside the lock.
        // lock_module();

        // delete _object.load();
        // _object.store(nullptr);

        // _task_id = -1; // Signal a potentially waiting thread for the module to exit that it can continue.
        // unlock_module();
    }

    /**
     * @brief Waits until _object is initialized, (from the new thread). This can be called from task_spawn().
     * @return Returns 0 iff successful, -1 on timeout or otherwise.
     */
    static int wait_until_running(int timeout_ms = 1000) {
        // int i = 0;

        // do {
        //     px4_usleep(2000);

        // } while (!_object.load() && ++i < timeout_ms / 2);

        // if (i >= timeout_ms / 2) {
        //     PX4_ERR("Timed out while waiting for thread to start");
        //     return -1;
        // }

        return 0;
    }

    /**
     * @brief Get the module's object instance, (this is null if it's not running).
     */
    static T *get_instacne(int inst = 0) {
        return (T *)_object[inst].load();
    }

    static atomic<T *> _object[N];

private:
    /**
     * @brief lock_module Mutex to lock the module thread.
     */
    static void lock_module() {
        // pthread_mutex_lock(&px4_modules_mutex);
        rt_enter_critical();
    }

    /**
     * @brief unlock_module Mutex to unlock the module thread.
     */
    static void unlock_module() {
        // pthread_mutex_unlock(&px4_modules_mutex);
        rt_exit_critical();
    }

    /** @var _task_should_exit Boolean flag to indicate if the task should exit. */
    atomic_bool _task_should_exit{false};
};

template <class T, uint8_t N>
atomic<T *> ModuleCommand<T, N>::_object[N]{};

#endif // __MODULE_COMMAND_H__
