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
 * @file module_params.hpp
 *
 * @class ModuleParams is a C++ base class for modules/classes using configuration parameters.
 */

#pragma once

#include <IntrusiveList.hpp>
#include "param_wrap.hpp"
#include "param_macros.h"

class ModuleParams : public ListNode<ModuleParams *> {
public:
    ModuleParams(ModuleParams *parent) {
        setParent(parent);
    }

    /**
     * @brief Sets the parent module. This is typically not required,
     *         only in cases where the parent cannot be set via constructor.
     */
    void setParent(ModuleParams *parent) {
        if (parent) {
            parent->_children.add(this);
        }

        _parent = parent;
    }

    virtual ~ModuleParams() {
        if (_parent) { _parent->_children.remove(this); }
    }

    // Disallow copy construction and move assignment.
    ModuleParams(const ModuleParams &)            = delete;
    ModuleParams &operator=(const ModuleParams &) = delete;
    ModuleParams(ModuleParams &&)                 = delete;
    ModuleParams &operator=(ModuleParams &&)      = delete;

protected:
    /**
     * @brief Call this method whenever the module gets a parameter change notification.
     *        It will automatically call updateParams() for all children, which then call updateParamsImpl().
     */
    virtual void updateParams() {
        for (const auto &child : _children) {
            child->updateParams();
        }

        updateParamsImpl();
    }

    /**
     * @brief The implementation for this is generated with the macro DEFINE_PARAMETERS()
     */
    virtual void updateParamsImpl() {
    }

private:
    /** @list _children The module parameter list of inheriting classes. */
    List<ModuleParams *> _children;
    ModuleParams        *_parent{nullptr};
};
