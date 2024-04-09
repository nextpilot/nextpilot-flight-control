/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <gtest/gtest.h>
#include <stdint.h>
#include <string.h>


#include "Ringbuffer.hpp"

class TempData
{
public:
	TempData(size_t len)
	{
		_size = len;
		_buf = new uint8_t[_size];
	}

	~TempData()
	{
		delete[] _buf;
		_buf = nullptr;
	}

	uint8_t *buf() const
	{
		return _buf;
	}

	size_t size() const
	{
		return _size;
	}

	void paint(unsigned offset = 0)
	{
		for (size_t i = 0; i < _size; ++i) {
			_buf[i] = (uint8_t)((i + offset) % UINT8_MAX);
		}
	}

private:
	uint8_t *_buf {nullptr};
	size_t _size{0};

};

bool operator==(const TempData &lhs, const TempData &rhs)
{
	if (lhs.size() != rhs.size()) {
		return false;
	}

	return memcmp(lhs.buf(), rhs.buf(), lhs.size()) == 0;
}


TEST(Ringbuffer, AllocateAndDeallocate)
{
	Ringbuffer buf;
	ASSERT_TRUE(buf.allocate(100));
	buf.deallocate();

	ASSERT_TRUE(buf.allocate(1000));
	// The second time we forget to clean up, but we expect no leak.
}

TEST(Ringbuffer, PushATooBigMessage)
{
	Ringbuffer buf;
	ASSERT_TRUE(buf.allocate(100));

	TempData data{200};

	// A message that doesn't fit should get rejected.
	EXPECT_FALSE(buf.push_back(data.buf(), data.size()));
}

TEST(Ringbuffer, PushAndPopOne)
{
	Ringbuffer buf;
	ASSERT_TRUE(buf.allocate(100));

	TempData data{20};
	data.paint();

	EXPECT_TRUE(buf.push_back(data.buf(), data.size()));

	EXPECT_EQ(buf.space_used(), 20);
	EXPECT_EQ(buf.space_available(), 79);

	// Get everything
	TempData out{20};
	EXPECT_EQ(buf.pop_front(out.buf(), out.size()), 20);
	EXPECT_EQ(data, out);

	// Nothing remaining
	EXPECT_EQ(buf.pop_front(out.buf(), out.size()), 0);
}

TEST(Ringbuffer, PushAndPopSeveral)
{
	Ringbuffer buf;
	ASSERT_TRUE(buf.allocate(100));

	TempData data{90};
	data.paint();

	// 9 little chunks in
	for (unsigned i = 0; i < 9; ++i) {
		EXPECT_TRUE(buf.push_back(data.buf() + i * 10, 10));
	}

	// 10 won't because of overhead inside the buffer
	EXPECT_FALSE(buf.push_back(data.buf(), 10));

	TempData out{90};
	// Take it back out in 2 big steps
	EXPECT_EQ(buf.pop_front(out.buf(), 50), 50);
	EXPECT_EQ(buf.pop_front(out.buf() + 50, 40), 40);
	EXPECT_EQ(data, out);
}

TEST(Ringbuffer, PushAndTryToPopMore)
{
	Ringbuffer buf;
	ASSERT_TRUE(buf.allocate(100));

	TempData data1{50};
	data1.paint();
	EXPECT_TRUE(buf.push_back(data1.buf(), data1.size()));

	TempData out1{80};
	EXPECT_EQ(buf.pop_front(out1.buf(), out1.size()), data1.size());
}

TEST(Ringbuffer, PushAndPopSeveralInterleaved)
{
	Ringbuffer buf;
	ASSERT_TRUE(buf.allocate(100));

	TempData data1{50};
	data1.paint();
	EXPECT_TRUE(buf.push_back(data1.buf(), data1.size()));

	TempData data2{30};
	data2.paint(50);
	EXPECT_TRUE(buf.push_back(data2.buf(), data2.size()));

	TempData out12{80};
	EXPECT_EQ(buf.pop_front(out12.buf(), out12.size()), out12.size());

	TempData out12_ref{80};
	out12_ref.paint();
	EXPECT_EQ(out12_ref, out12);

	TempData data3{50};
	data3.paint(33);
	EXPECT_TRUE(buf.push_back(data3.buf(), data3.size()));

	TempData out3{50};
	EXPECT_EQ(buf.pop_front(out3.buf(), out3.size()), data3.size());
	EXPECT_EQ(data3, out3);
}

TEST(Ringbuffer, PushEmpty)
{
	Ringbuffer buf;
	ASSERT_TRUE(buf.allocate(100));

	EXPECT_FALSE(buf.push_back(nullptr, 0));
}

TEST(Ringbuffer, PopWithoutBuffer)
{
	Ringbuffer buf;
	ASSERT_TRUE(buf.allocate(100));

	EXPECT_FALSE(buf.push_back(nullptr, 0));

	TempData data{50};
	data.paint();

	EXPECT_TRUE(buf.push_back(data.buf(), data.size()));


	EXPECT_EQ(buf.pop_front(nullptr, 50), 0);
}

TEST(Ringbuffer, EmptyAndNoSpaceForHeader)
{
	// Addressing a corner case where start and end are at the end
	// and the same.

	Ringbuffer buf;
	// Allocate 1 bytes more than the packet, 1 for the start/end logic.
	ASSERT_TRUE(buf.allocate(21));

	{
		TempData data{20};
		data.paint();
		EXPECT_TRUE(buf.push_back(data.buf(), data.size()));
		TempData out{20};
		EXPECT_EQ(buf.pop_front(out.buf(), out.size()), out.size());
		EXPECT_EQ(data, out);
	}

	{
		TempData data{10};
		data.paint();
		EXPECT_TRUE(buf.push_back(data.buf(), data.size()));
		TempData out{10};
		EXPECT_EQ(buf.pop_front(out.buf(), out.size()), out.size());
		EXPECT_EQ(data, out);
	}
}
