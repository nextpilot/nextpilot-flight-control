/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once


#include <nuttx/config.h>
#include <nuttx/fs/fs.h>
#include <poll.h>
#include <errno.h>
#include <cstdio>
#include <ctime>
#include <cstring>

#include <uavcan/uavcan.hpp>

namespace uavcan_socketcan
{

class CanDriver;


/**
 * All bus events are reported as POLLIN.
 */
class BusEvent : uavcan::Noncopyable
{
	using SignalCallbackHandler = void(*)();

	SignalCallbackHandler signal_cb_{nullptr};
	sem_t sem_;
public:

	BusEvent(CanDriver &can_driver);
	~BusEvent();

	void registerSignalCallback(SignalCallbackHandler handler) { signal_cb_ = handler; }

	bool wait(uavcan::MonotonicDuration duration);

	void signalFromInterrupt();
};

class Mutex
{
	pthread_mutex_t mutex_;

public:
	Mutex()
	{
		init();
	}

	int init()
	{
		return pthread_mutex_init(&mutex_, UAVCAN_NULLPTR);
	}

	int deinit()
	{
		return pthread_mutex_destroy(&mutex_);
	}

	void lock()
	{
		(void)pthread_mutex_lock(&mutex_);
	}

	void unlock()
	{
		(void)pthread_mutex_unlock(&mutex_);
	}
};


class MutexLocker
{
	Mutex &mutex_;

public:
	MutexLocker(Mutex &mutex)
		: mutex_(mutex)
	{
		mutex_.lock();
	}
	~MutexLocker()
	{
		mutex_.unlock();
	}
};

}
