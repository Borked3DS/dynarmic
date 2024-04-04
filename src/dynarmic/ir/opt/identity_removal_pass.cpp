/* This file is part of the dynarmic project.
 * Copyright (c) 2020 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include <array>

#include "dynarmic/ir/basic_block.h"
#include "dynarmic/ir/opcodes.h"
#include "dynarmic/ir/opt/passes.h"

namespace Dynarmic::Optimization {

void IdentityRemovalPass(IR::Block& block) {
    constexpr size_t max_instructions = 128; // Assuming a maximum of 128 instructions in the block
    std::array<IR::Inst*, max_instructions> to_invalidate;
    size_t to_invalidate_count = 0;

    for (auto& inst : block) {
        const size_t num_args = inst.NumArgs();
        std::array<IR::Value, 8> args; // Assuming a maximum of 8 arguments per instruction
        size_t args_count = 0;

        for (size_t i = 0; i < num_args; ++i) {
            args[args_count++] = inst.GetArg(i);
            if (args[args_count - 1].IsIdentity()) {
                args[args_count - 1] = args[args_count - 1].GetInst()->GetArg(0);
            }
        }

        for (size_t i = 0; i < args_count; ++i) {
            inst.SetArg(i, args[i]);
        }

        if (inst.GetOpcode() == IR::Opcode::Identity || inst.GetOpcode() == IR::Opcode::Void) {
            to_invalidate[to_invalidate_count++] = &inst;
        }
    }

    for (size_t i = 0; i < to_invalidate_count; ++i) {
        to_invalidate[i]->Invalidate();
    }
}

}  // namespace Dynarmic::Optimization


