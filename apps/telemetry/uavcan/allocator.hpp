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
 * @author Pavel Kirienko <pavel.kirienko@gmail.com>
 */

#pragma once

#include <systemlib/err.h>
#include <uavcan/uavcan.hpp>
#include <uavcan/helpers/heap_based_pool_allocator.hpp>

// TODO: Entire UAVCAN application should be moved into a namespace later; this is the first step.
namespace uavcan_node {

struct AllocatorSynchronizer {
    const ::irqstate_t state = ::enter_critical_section();
    ~AllocatorSynchronizer() {
        ::leave_critical_section(state);
    }
};

struct Allocator : public uavcan::HeapBasedPoolAllocator<uavcan::MemPoolBlockSize, AllocatorSynchronizer> {
    static constexpr unsigned CapacitySoftLimit = 250;
    static constexpr unsigned CapacityHardLimit = 500;

    Allocator() :
        uavcan::HeapBasedPoolAllocator<uavcan::MemPoolBlockSize, AllocatorSynchronizer>(CapacitySoftLimit, CapacityHardLimit) {
    }

    ~Allocator() {
        if (getNumAllocatedBlocks() > 0) {
            PX4_ERR("UAVCAN LEAKS MEMORY: %u BLOCKS (%u BYTES) LOST",
                    getNumAllocatedBlocks(), getNumAllocatedBlocks() * uavcan::MemPoolBlockSize);
        }
    }
};

} // namespace uavcan_node
