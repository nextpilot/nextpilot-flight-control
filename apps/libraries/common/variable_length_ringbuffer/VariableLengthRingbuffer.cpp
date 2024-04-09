/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/




#include "VariableLengthRingbuffer.hpp"

#include <assert.h>
#include <string.h>


VariableLengthRingbuffer::~VariableLengthRingbuffer()
{
	deallocate();
}

bool VariableLengthRingbuffer::allocate(size_t buffer_size)
{
	return _ringbuffer.allocate(buffer_size);
}

void VariableLengthRingbuffer::deallocate()
{
	_ringbuffer.deallocate();
}

bool VariableLengthRingbuffer::push_back(const uint8_t *packet, size_t packet_len)
{
	if (packet_len == 0 || packet == nullptr) {
		// Nothing to add, we better don't try.
		return false;
	}

	size_t space_required = packet_len + sizeof(Header);

	if (space_required > _ringbuffer.space_available()) {
		return false;
	}

	Header header{static_cast<uint32_t>(packet_len)};
	bool result = _ringbuffer.push_back(reinterpret_cast<const uint8_t * >(&header), sizeof(header));
	assert(result);

	result = _ringbuffer.push_back(packet, packet_len);
	assert(result);

	// In case asserts are commented out to prevent unused warnings.
	(void)result;

	return true;
}

size_t VariableLengthRingbuffer::pop_front(uint8_t *buf, size_t buf_max_len)
{
	if (buf == nullptr) {
		// User needs to supply a valid pointer.
		return 0;
	}

	// Check next header
	Header header;

	if (_ringbuffer.pop_front(reinterpret_cast<uint8_t *>(&header), sizeof(header)) < sizeof(header)) {
		return 0;
	}

	// We can't fit the packet into the user supplied buffer.
	// This should never happen as the user has to supply a big // enough buffer.
	assert(static_cast<uint32_t>(header.len) <= buf_max_len);

	size_t bytes_read = _ringbuffer.pop_front(buf, header.len);
	assert(bytes_read == header.len);

	return bytes_read;
}
