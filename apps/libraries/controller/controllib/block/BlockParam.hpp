/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file BlockParam.hpp
 *
 * Controller library code
 */

#pragma once

#include "Block.hpp"

#include <containers/IntrusiveList.hpp>
#include <defines.h>
#include <param.h>

namespace control
{

class Block;

// A base class for block params that enables traversing linked list.
class BlockParamBase : public ListNode<BlockParamBase *>
{
public:
	/**
	 * Instantiate a block param base.
	 *
	 * @param parent_prefix Set to true to include the parent name in the parameter name
	 */
	BlockParamBase(Block *parent, const char *name, bool parent_prefix = true);
	virtual ~BlockParamBase() = default;

	virtual bool update() = 0;
	const char *getName() const { return param_get_name(_handle); }

protected:
	param_t _handle{PARAM_INVALID};
};

// Parameters that are tied to blocks for updating and naming.
template <class T>
class __EXPORT BlockParam final : public BlockParamBase
{
public:
	BlockParam(Block *block, const char *name, bool parent_prefix = true);
	BlockParam(Block *block, const char *name, bool parent_prefix, T &extern_val);

	~BlockParam() override = default;

	// no copy, assignment, move, move assignment
	BlockParam(const BlockParam &) = delete;
	BlockParam &operator=(const BlockParam &) = delete;
	BlockParam(BlockParam &&) = delete;
	BlockParam &operator=(BlockParam &&) = delete;

	T get() const { return _val; }

	// Store the parameter value to the parameter storage (@see param_set())
	bool commit() { return (param_set(_handle, &_val) == PX4_OK); }

	// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
	bool commit_no_notification() { return (param_set_no_notification(_handle, &_val) == PX4_OK); }

	void set(T val) { _val = val; }

	bool update() override { return (param_get(_handle, &_val) == PX4_OK); }

protected:
	T _val;
};

template <>
bool BlockParam<bool>::update();

typedef BlockParam<float> BlockParamFloat;
typedef BlockParam<int32_t> BlockParamInt;
typedef BlockParam<bool> BlockParamBool;
typedef BlockParam<float &> BlockParamExtFloat;
typedef BlockParam<int32_t &> BlockParamExtInt;

} // namespace control
