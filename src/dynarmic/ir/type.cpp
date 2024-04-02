/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include "dynarmic/ir/type.h"

#include <array>
#include <ostream>
#include <string>

namespace Dynarmic::IR {

static constexpr const char* type_names[] = {
    "Void",
    "A32Reg", "A32ExtReg",
    "A64Reg", "A64Vec",
    "Opaque",
    "U1", "U8", "U16", "U32", "U64", "U128",
    "CoprocInfo",
    "NZCVFlags",
    "Cond",
    "Table"
};

static constexpr size_t type_name_count = std::size(type_names);

std::string GetNameOf(Type type) {
    using UnderlyingType = std::underlying_type_t<Type>;
    const UnderlyingType bits = static_cast<UnderlyingType>(type);

    if (bits == 0) {
        return type_names[0];
    }

    std::string result;
    for (size_t i = 1; i < type_name_count; i++) {
        if (bits & (size_t(1) << (i - 1))) {
            if (!result.empty()) {
                result += '|';
            }
            result += type_names[i];
        }
    }
    return result;
}

bool AreTypesCompatible(Type t1, Type t2) {
    return t1 == t2 || t1 == Type::Opaque || t2 == Type::Opaque;
}

}  // namespace Dynarmic::IR
