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
 * @file bitmask.h
 * @brief Provides SFINAE for type safe templated bitmask operations
 * @author Nuno Marques <nuno.marques@dronesolutions.io>
 *
 */

#pragma once

#if defined(__cplusplus) && (defined(__PX4_POSIX) || defined(__PX4_LINUX))

#   include <type_traits>

template <typename E>
struct enable_bitmask_operators {
    static const bool enable = false;
};

namespace px4 {

#   define ENABLE_BIT_OPERATORS(E)          \
       template <>                          \
       struct enable_bitmask_operators<E> { \
           static const bool enable = true; \
       };

template <typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
operator==(E lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
        static_cast<underlying>(lhs) == static_cast<underlying>(rhs));
}

template <typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
operator~(E lhs) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
        ~static_cast<underlying>(lhs));
}

template <typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
operator|(E lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
        static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template <typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E &>::type
operator|=(E &lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    lhs = static_cast<E>(
        static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
    return lhs;
}

template <typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
operator&(E lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template <typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E &>::type
operator&=(E &lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    lhs = static_cast<E>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
    return lhs;
}

template <typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
operator^(E lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
        static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}

template <typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E &>::type
operator^=(E &lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    lhs = static_cast<E>(
        static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
    return lhs;
}

} /* namespace px4 */

#endif /* __cplusplus && (__PX4_POSIX || __PX4_LINUX) */
