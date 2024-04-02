/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include "dynarmic/frontend/A64/a64_location_descriptor.h"

#include <string>

namespace Dynarmic::A64 {
    std::string ToString(const LocationDescriptor& descriptor) {
        std::string result;
        result.reserve(32);  // Estimate the maximum size of the string

        result = "{";
        result += std::to_string(descriptor.PC());
        result += ", ";
        result += std::to_string(descriptor.FPCR().Value());

        if (descriptor.SingleStepping()) {
            result += ", step";
        }

        result += "}";

        return result;
    }
}  // namespace Dynarmic::A64
