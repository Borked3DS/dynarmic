/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include <cstdio>
#include <array>

#include <mcl/assert.hpp>
#include <mcl/stdint.hpp>

#include "dynarmic/ir/basic_block.h"
#include "dynarmic/ir/microinstruction.h"
#include "dynarmic/ir/opcodes.h"
#include "dynarmic/ir/opt/passes.h"
#include "dynarmic/ir/type.h"

namespace Dynarmic::Optimization {

void VerificationPass(const IR::Block& block) {
    for (const auto& inst : block) {
        for (size_t i = 0; i < inst.NumArgs(); i++) {
            const IR::Type t1 = inst.GetArg(i).GetType();
            const IR::Type t2 = IR::GetArgTypeOf(inst.GetOpcode(), i);
            if (!IR::AreTypesCompatible(t1, t2)) {
                std::puts(IR::DumpBlock(block).c_str());
                ASSERT_FALSE("above block failed validation");
            }
        }
    }

    constexpr size_t max_instructions = 128;
    std::array<size_t, max_instructions> actual_uses = {};

    for (const auto& inst : block) {
        for (size_t i = 0; i < inst.NumArgs(); i++) {
            const auto arg = inst.GetArg(i);
            if (!arg.IsImmediate()) {
                actual_uses[reinterpret_cast<size_t>(arg.GetInst())]++;
            }
        }
    }

    for (const auto& inst : block) {
        ASSERT(inst.UseCount() == actual_uses[reinterpret_cast<size_t>(&inst)]);
    }
}

}  // namespace Dynarmic::Optimization

