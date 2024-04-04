/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include "dynarmic/ir/opcodes.h"
#include <array>
#include "dynarmic/ir/type.h"

namespace Dynarmic::IR {

// Opcode information

namespace OpcodeInfo {

struct Meta {
    const char* name;
    Type type;
    std::array<Type, 8> arg_types; // Assuming a maximum of 8 arguments
    size_t num_args;
};

constexpr Type Void = Type::Void;
// ... (other type definitions)

static const std::array<Meta, 256> opcode_info = {
    // Manually initialize the opcode_info array
    {
        {"AddWithCarry", Type::U32, {Type::U32, Type::U32, Type::U1}, 3},
        {"AddWithCarry64", Type::U64, {Type::U64, Type::U64, Type::U1}, 3},
        {"And", Type::U32, {Type::U32, Type::U32}, 2},
        {"And64", Type::U64, {Type::U64, Type::U64}, 2},
        // Add more opcode information as needed
    }
};

}  // namespace OpcodeInfo

Type GetTypeOf(Opcode op) {
    return OpcodeInfo::opcode_info[static_cast<size_t>(op)].type;
}

size_t GetNumArgsOf(Opcode op) {
    return OpcodeInfo::opcode_info[static_cast<size_t>(op)].num_args;
}

Type GetArgTypeOf(Opcode op, size_t arg_index) {
    return OpcodeInfo::opcode_info[static_cast<size_t>(op)].arg_types[arg_index];
}

std::string GetNameOf(Opcode op) {
    return OpcodeInfo::opcode_info[static_cast<size_t>(op)].name;
}

}  // namespace Dynarmic::IR
