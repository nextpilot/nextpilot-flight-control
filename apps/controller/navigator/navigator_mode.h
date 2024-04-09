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
 * @file navigator_mode.h
 *
 * Base class for different modes in navigator
 *
 * @author Julian Oes <julian@oes.ch>
 * @author Anton Babushkin <anton.babushkin@me.com>
 */

#pragma once

class Navigator;

class NavigatorMode {
public:
    NavigatorMode(Navigator *navigator);
    virtual ~NavigatorMode()                        = default;
    NavigatorMode(const NavigatorMode &)            = delete;
    NavigatorMode &operator=(const NavigatorMode &) = delete;
    virtual void   initialize()                     = 0;

    void run(bool active);

    /**
     * This function is called while the mode is inactive
     */
    virtual void on_inactive();

    /**
     * This function is called one time when mode becomes active, pos_sp_triplet must be initialized here
     */
    virtual void on_activation();

    /**
     * This function is called one time when mode becomes inactive
     */
    virtual void on_inactivation();

    /**
     * This function is called while the mode is active
     */
    virtual void on_active();

protected:
    Navigator *_navigator{nullptr};

private:
    bool _active{false};
};
