/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#pragma once

#include <cstddef>
#include <vector>

namespace Dynarmic::Common {

class Pool {
public:
    Pool(size_t object_size, size_t initial_pool_size);
    ~Pool();
    void* Alloc();
private:
    void AllocateNewSlab();
    size_t object_size;
    size_t slab_size;
    size_t remaining = 0;
    char* current_slab = nullptr;
    char* current_ptr = nullptr;
    std::vector<char*> slabs;
};

}  // namespace Dynarmic::Common
