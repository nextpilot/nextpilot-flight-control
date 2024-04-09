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
 * @file Block.cpp
 *
 * Controller library code
 */

#include "Block.hpp"
#include "BlockParam.hpp"

#include <cstdio>
#include <cstring>

#include <px4_platform_common/log.h>

namespace control
{

Block::Block(SuperBlock *parent, const char *name) :
	_name(name),
	_parent(parent)
{
	if (getParent() != nullptr) {
		getParent()->getChildren().add(this);
	}
}

void Block::getName(char *buf, size_t n)
{
	if (getParent() == nullptr) {
		strncpy(buf, _name, n);
		// ensure string is terminated
		buf[n - 1] = '\0';

	} else {
		char parentName[blockNameLengthMax];
		getParent()->getName(parentName, n);

		if (!strcmp(_name, "")) {
			strncpy(buf, parentName, n);
			// ensure string is terminated
			buf[n - 1] = '\0';

		} else {
			snprintf(buf, n, "%s_%s", parentName, _name);
		}
	}
}

void Block::updateParams()
{
	BlockParamBase *param = getParams().getHead();
	int count = 0;

	while (param != nullptr) {
		if (count++ > maxParamsPerBlock) {
			char name[blockNameLengthMax];
			getName(name, blockNameLengthMax);
			PX4_ERR("exceeded max params for block: %s", name);
			break;
		}

		//printf("updating param: %s\n", param->getName());
		param->update();
		param = param->getSibling();
	}

	updateParamsSubclass();
}

void SuperBlock::setDt(float dt)
{
	Block::setDt(dt);
	Block *child = getChildren().getHead();
	int count = 0;

	while (child != nullptr) {
		if (count++ > maxChildrenPerBlock) {
			char name[blockNameLengthMax];
			getName(name, blockNameLengthMax);
			PX4_ERR("exceeded max children for block: %s", name);
			break;
		}

		child->setDt(dt);
		child = child->getSibling();
	}
}

void SuperBlock::updateChildParams()
{
	Block *child = getChildren().getHead();
	int count = 0;

	while (child != nullptr) {
		if (count++ > maxChildrenPerBlock) {
			char name[blockNameLengthMax];
			getName(name, blockNameLengthMax);
			PX4_ERR("exceeded max children for block: %s", name);
			break;
		}

		child->updateParams();
		child = child->getSibling();
	}
}

} // namespace control

template class List<control::BlockParamBase *>;
