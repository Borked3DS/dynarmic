/* This file is part of the dynarmic project.
 * Copyright (c) 2018 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include "dynarmic/frontend/A64/a64_types.h"

#include <array>
#include <string>

namespace Dynarmic::A64 {
    static constexpr std::array<const char*, 16> cond_strs = {
        "eq", "ne", "hs", "lo", "mi", "pl", "vs", "vc",
        "hi", "ls", "ge", "lt", "gt", "le", "al", "nv"
    };

    const char* CondToString(Cond cond) {
        return cond_strs[static_cast<size_t>(cond)];
    }

    std::string RegToString(Reg reg) {
        if (reg == Reg::R31) {
            return "sp|zr";
        }

        std::string result;
        result.reserve(3);  // Estimated maximum size of the string
        result += 'r';
        result += static_cast<char>(static_cast<size_t>(reg) + '0');
        return result;
    }

    std::string VecToString(Vec vec) {
        std::string result;
        result.reserve(3);  // Estimated maximum size of the string
        result += 'v';
        result += static_cast<char>(static_cast<size_t>(vec) + '0');
        return result;
    }
}  // namespace Dynarmic::A64
