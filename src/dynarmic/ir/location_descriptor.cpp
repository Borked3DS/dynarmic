/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include "dynarmic/ir/location_descriptor.h"

#include <cstring>

namespace Dynarmic::IR {
std::string ToString(const LocationDescriptor& descriptor) {
    char buffer[18]; // 16 hex digits + { } + null terminator
    char* ptr = buffer;
    *ptr++ = '{';

    uint64_t value = descriptor.Value();
    for (int i = 0; i < 16; i++) {
        *ptr++ = "0123456789ABCDEF"[(value >> ((15 - i) * 4)) & 0xF];
    }

    *ptr++ = '}';
    *ptr = '\0';

    return std::string(buffer, ptr - buffer);
}
}  // namespace Dynarmic::IR
