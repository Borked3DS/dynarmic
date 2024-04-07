/* This file is part of the dynarmic project.
 * Copyright (c) 2020 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include <cstdlib>
#include "kvec.h"

#include "dynarmic/ir/basic_block.h"
#include "dynarmic/ir/opcodes.h"
#include "dynarmic/ir/opt/passes.h"

namespace Dynarmic::Optimization {

void IdentityRemovalPass(IR::Block& block) {
    kvec_t(IR::Inst*) to_invalidate;
    kv_init(to_invalidate);

    for (auto& inst : block) {
        const size_t num_args = inst.NumArgs();
        kvec_t(IR::Value) args;
        kv_init(args);

        for (size_t i = 0; i < num_args; ++i) {
            IR::Value arg = inst.GetArg(i);
            if (arg.IsIdentity()) {
                arg = arg.GetInst()->GetArg(0);
            }
            kv_push(IR::Value, args, arg);
        }

        for (size_t i = 0; i < num_args; ++i) {
            inst.SetArg(i, kv_A(args, i));
        }

        if (inst.GetOpcode() == IR::Opcode::Identity || inst.GetOpcode() == IR::Opcode::Void) {
            kv_push(IR::Inst*, to_invalidate, &inst);
        }

        kv_destroy(args);
    }

    for (size_t i = 0; i < kv_size(to_invalidate); i++) {
        to_invalidate.a[i]->Invalidate();
    }

    kv_destroy(to_invalidate);
}

}  // namespace Dynarmic::Optimization


