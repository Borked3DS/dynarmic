/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include "dynarmic/frontend/A32/a32_location_descriptor.h"

#include <string>

namespace Dynarmic::A32 {

std::string ToString(const LocationDescriptor& descriptor) {
    static constexpr char kOpenBrace = '{';
    static constexpr char kCloseBrace = '}';
    static constexpr char kComma = ',';
    static constexpr char kTFlag[] = "T";
    static constexpr char kETFlag[] = "!T";
    static constexpr char kEFlag[] = "E";
    static constexpr char kEEFlag[] = "!E";
    static constexpr char kStepFlag[] = ",step";

    std::string result;
    result.reserve(32);  // Preallocate some space to avoid reallocations

    result += kOpenBrace;
    result += std::to_string(descriptor.PC());
    result += kComma;
    result += (descriptor.TFlag() ? kTFlag : kETFlag);
    result += kComma;
    result += (descriptor.EFlag() ? kEFlag : kEEFlag);
    result += kComma;
    result += std::to_string(descriptor.FPSCR().Value());

    if (descriptor.SingleStepping()) {
        result += kComma;
        result += kStepFlag;
    }

    result += kCloseBrace;
    return result;
}

}  // namespace Dynarmic::A32
