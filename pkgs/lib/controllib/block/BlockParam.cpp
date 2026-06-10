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
 * @file BlockParam.cpp
 *
 * Controller library code
 */

#define LOG_TAG "BlockParam"

#include "BlockParam.hpp"
#include <cstdio>
#include <cstring>
#include <containers/IntrusiveList.hpp>
#include <ulog/log.h>

namespace control
{

BlockParamBase::BlockParamBase(Block *parent, const char *name, bool parent_prefix)
{
	char fullname[blockNameLengthMax];

	if (parent == nullptr) {
		strncpy(fullname, name, blockNameLengthMax - 1);
		fullname[sizeof(fullname) - 1] = '\0';

	} else {
		char parentName[blockNameLengthMax];
		parent->getName(parentName, blockNameLengthMax);

		if (strcmp(name, "") == 0) {
			strncpy(fullname, parentName, blockNameLengthMax);
			// ensure string is terminated
			fullname[sizeof(fullname) - 1] = '\0';

		} else if (parent_prefix) {
			if (snprintf(fullname, blockNameLengthMax, "%s_%s", parentName, name) >= blockNameLengthMax) {
				PX4_ERR("param too long: %s", name);
			}

		} else {
			strncpy(fullname, name, blockNameLengthMax);
			// ensure string is terminated
			fullname[sizeof(fullname) - 1] = '\0';
		}

		parent->getParams().add(this);
	}

	_handle = param_find(fullname);

	if (_handle == PARAM_INVALID) {
		PX4_ERR("error finding param: %s", fullname);
	}
};

template <>
BlockParam<bool>::BlockParam(Block *block, const char *name, bool parent_prefix) :
	BlockParamBase(block, name, parent_prefix),
	_val()
{
	update();
}

template <>
bool BlockParam<bool>::update()
{
	int32_t tmp = 0;
	int ret = param_get(_handle, &tmp);

	if (tmp == 1) {
		_val = true;

	} else {
		_val = false;
	}

	return (ret == PX4_OK);
}

template <>
BlockParam<int32_t>::BlockParam(Block *block, const char *name, bool parent_prefix) :
	BlockParamBase(block, name, parent_prefix),
	_val()
{
	update();
}

template <>
BlockParam<float>::BlockParam(Block *block, const char *name, bool parent_prefix) :
	BlockParamBase(block, name, parent_prefix),
	_val()
{
	update();
}

template <>
BlockParam<int32_t &>::BlockParam(Block *block, const char *name, bool parent_prefix, int32_t &extern_val) :
	BlockParamBase(block, name, parent_prefix),
	_val(extern_val)
{
	update();
}

template <>
BlockParam<float &>::BlockParam(Block *block, const char *name, bool parent_prefix, float &extern_val) :
	BlockParamBase(block, name, parent_prefix),
	_val(extern_val)
{
	update();
}

} // namespace control
