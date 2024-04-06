/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __MODULE_THREAD_H__
#define __MODULE_THREAD_H__

#include <rtthread.h>
#include <rtdbg.h>

class ModuleThread {
public:
    ModuleThread(const char *name = "Unkown", rt_uint32_t stack_size = 1024, rt_uint8_t priority = 20, rt_uint32_t max_tick = 10) :
        _name{name}, _stack_size{stack_size}, _priority{priority}, _max_tick{max_tick} {
        _tid = rt_thread_create(_name, RunEntry, this, stack_size, priority, max_tick);

        if (!_tid) {
            LOG_E("create thread fail");
            return;
        }

        if (rt_thread_startup(_tid) != 0) {
            LOG_E("startup thread fail");
            rt_thread_delete(_tid);
            _tid = nullptr;
            return;
        }

        LOG_D("create/startup thread ok");
    }

    virtual ~ModuleThread() {
        if (_tid) {
            // TODO:这里为什么会报错呢
            // rt_thread_delete(_tid);
        }
    }

    static void RunEntry(void *param) {
        ModuleThread *object = (ModuleThread *)param;
        object->Run();
    }

    virtual int init() {
        return 0;
    }

    virtual void Run() {
    }

    bool operator<=(const ModuleThread &rhs) const {
        return (rt_strcmp(_name, rhs._name) <= 0);
    }

protected:
    const char *_name;
    rt_uint32_t _stack_size = 2048;
    rt_uint8_t  _priority   = 15;
    rt_uint32_t _max_tick   = 10;
    rt_thread_t _tid        = nullptr;
};

#endif // __MODULE_THREAD_H__
