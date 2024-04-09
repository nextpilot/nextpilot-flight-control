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
 * @file Block.hpp
 *
 * Controller library code
 */

#pragma once

#include <containers/List.hpp>
#include <controllib/block/BlockParam.hpp>
#include <cstdint>

namespace control
{

static constexpr uint8_t maxChildrenPerBlock = 100;
static constexpr uint8_t maxParamsPerBlock = 110;
static constexpr uint8_t blockNameLengthMax = 40;

// forward declaration
class BlockParamBase;
class SuperBlock;

/**
 */
class __EXPORT Block : public ListNode<Block *>
{
public:
	friend class BlockParamBase;

	Block(SuperBlock *parent, const char *name);
	virtual ~Block() = default;

	// no copy, assignment, move, move assignment
	Block(const Block &) = delete;
	Block &operator=(const Block &) = delete;
	Block(Block &&) = delete;
	Block &operator=(Block &&) = delete;

	void getName(char *name, size_t n);

	virtual void updateParams();

	virtual void setDt(float dt) { _dt = dt; }
	float getDt() { return _dt; }

protected:

	virtual void updateParamsSubclass() {}

	SuperBlock *getParent() { return _parent; }
	List<BlockParamBase *> &getParams() { return _params; }

	const char *_name;
	SuperBlock *_parent;
	float _dt{0.0f};

	List<BlockParamBase *> _params;
};

class __EXPORT SuperBlock :
	public Block
{
public:
	friend class Block;

	SuperBlock(SuperBlock *parent, const char *name) : Block(parent, name) {}
	~SuperBlock() = default;

	// no copy, assignment, move, move assignment
	SuperBlock(const SuperBlock &) = delete;
	SuperBlock &operator=(const SuperBlock &) = delete;
	SuperBlock(SuperBlock &&) = delete;
	SuperBlock &operator=(SuperBlock &&) = delete;

	void setDt(float dt) override;

	void updateParams() override
	{
		Block::updateParams();

		if (getChildren().getHead() != nullptr) { updateChildParams(); }
	}

protected:
	List<Block *> &getChildren() { return _children; }
	void updateChildParams();

	List<Block *> _children;
};


} // namespace control
