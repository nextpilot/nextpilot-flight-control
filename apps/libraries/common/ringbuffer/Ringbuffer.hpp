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

#include <stdint.h>
#include <pthread.h>


// FIFO ringbuffer implementation.
//
// The ringbuffer can store up 1 byte less than allocated as
// start and end marker need to be one byte apart when the buffer
// is full, otherwise it would suddenly be empty.
//
// The buffer is not thread-safe.

class Ringbuffer
{
public:
	/* @brief Constructor
	 *
	 * @note Does not allocate automatically.
	 */
	Ringbuffer() = default;

	/*
	 * @brief Destructor
	 *
	 * Automatically calls deallocate.
	 */
	~Ringbuffer();

	/* @brief Allocate ringbuffer
	 *
	 * @param buffer_size Number of bytes to allocate on heap.
	 *
	 * @returns false if allocation fails.
	 */
	bool allocate(size_t buffer_size);

	/*
	 * @brief Deallocate ringbuffer
	 *
	 * @note only required to deallocate and reallocate again.
	 */
	void deallocate();

	/*
	 * @brief Space available to copy bytes into
	 *
	 * @returns number of free bytes.
	 */
	size_t space_available() const;

	/*
	 * @brief Space used to copy data from
	 *
	 * @returns number of used bytes.
	 */
	size_t space_used() const;

	/*
	 * @brief Copy data into ringbuffer
	 *
	 * @param buf Pointer to buffer to copy from.
	 * @param buf_len Number of bytes to copy.
	 *
	 * @returns true if packet could be copied into buffer.
	 */
	bool push_back(const uint8_t *buf, size_t buf_len);

	/*
	 * @brief Get data from ringbuffer
	 *
	 * @param buf Pointer to buffer where data can be copied into.
	 * @param max_buf_len Max number of bytes to copy.
	 *
	 * @returns 0 if buffer is empty.
	 */
	size_t pop_front(uint8_t *buf, size_t max_buf_len);

private:
	size_t _size {0};
	uint8_t *_ringbuffer {nullptr};
	size_t _start{0};
	size_t _end{0};
};
