/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include "dynarmic/ir/type.h"

#include <array>
#include <string>

namespace Dynarmic::IR {

std::string GetNameOf(Type type) {
    static constexpr const char* type_names[32] = {
        "Void", "A32Reg", "A32ExtReg", "A64Reg", "A64Vec", "Opaque",
        "U1", "U8", "U16", "U32", "U64", "U128", "CoprocInfo", "NZCV", "Cond", "Table", "AccType"};
    static constexpr size_t type_name_count = sizeof(type_names) / sizeof(type_names[0]);

    std::string result;
    for (size_t i = 0; i < type_name_count; i++) {
        if (static_cast<size_t>(type) & (size_t(1) << i)) {
            if (!result.empty()) {
                result += '|';
            }
            result += type_names[i];
        }
    }

    if (result.empty()) {
        return type_names[0];  // "Void"
    }
    return result;
}

}  // namespace Dynarmic::IR
