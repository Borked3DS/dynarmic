/* This file is part of the dynarmic project.
 * Copyright (c) 2020 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include <vector>

#include "dynarmic/ir/basic_block.h"
#include "dynarmic/ir/opcodes.h"
#include "dynarmic/ir/opt/passes.h"

namespace Dynarmic::Optimization {

void IdentityRemovalPass(IR::Block& block) {
    std::vector<IR::Inst*> to_invalidate;

    for (auto& inst : block) {
        const size_t num_args = inst.NumArgs();
        std::vector<IR::Value> args(num_args);

        for (size_t i = 0; i < num_args; ++i) {
            args[i] = inst.GetArg(i);
            if (args[i].IsIdentity()) {
                args[i] = args[i].GetInst()->GetArg(0);
            }
        }

        for (size_t i = 0; i < num_args; ++i) {
            inst.SetArg(i, args[i]);
        }

        if (inst.GetOpcode() == IR::Opcode::Identity || inst.GetOpcode() == IR::Opcode::Void) {
            to_invalidate.push_back(&inst);
        }
    }

    for (IR::Inst* inst : to_invalidate) {
        inst->Invalidate();
    }
}

}  // namespace Dynarmic::Optimization


