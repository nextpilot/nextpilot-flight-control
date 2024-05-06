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
#include <ringbuffer/Ringbuffer.hpp>


// FIFO ringbuffer implementation for packets of variable length.
//
// The variable length is implemented using a 4 byte header
// containing a the length.
//
// The buffer is not thread-safe.

class VariableLengthRingbuffer
{
public:
	/* @brief Constructor
	 *
	 * @note Does not allocate automatically.
	 */
	VariableLengthRingbuffer() = default;

	/*
	 * @brief Destructor
	 *
	 * Automatically calls deallocate.
	 */
	~VariableLengthRingbuffer();

	/* @brief Allocate ringbuffer
	 *
	 * @note The variable length requires 4 bytes
	 * of overhead per packet.
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
	 * @brief Copy packet into ringbuffer
	 *
	 * @param packet Pointer to packet to copy from.
	 * @param packet_len Length of packet.
	 *
	 * @returns true if packet could be copied into buffer.
	 */
	bool push_back(const uint8_t *packet, size_t packet_len);

	/*
	 * @brief Get packet from ringbuffer
	 *
	 * @note max_buf_len needs to be bigger equal to any pushed packet.
	 *
	 * @param buf Pointer to where next packet can be copied into.
	 * @param max_buf_len Max size of buf
	 *
	 * @returns 0 if packet is bigger than max_len or buffer is empty.
	 */
	size_t pop_front(uint8_t *buf, size_t max_buf_len);

private:
	struct Header {
		uint32_t len;
	};

	Ringbuffer _ringbuffer {};
};
