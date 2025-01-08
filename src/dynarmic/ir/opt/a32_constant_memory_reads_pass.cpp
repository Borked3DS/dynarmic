/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include <unordered_map>

#include "dynarmic/interface/A32/config.h"
#include "dynarmic/ir/basic_block.h"
#include "dynarmic/ir/opcodes.h"
#include "dynarmic/ir/opt/passes.h"

namespace Dynarmic::Optimization {

void A32ConstantMemoryReads(IR::Block& block, A32::UserCallbacks* cb) {
    std::unordered_map<u32, std::pair<bool, u64>> cache;

    for (auto& inst : block) {
        switch (inst.GetOpcode()) {
        case IR::Opcode::A32ReadMemory8:
        case IR::Opcode::A32ReadMemory16:
        case IR::Opcode::A32ReadMemory32:
        case IR::Opcode::A32ReadMemory64: {
            if (!inst.AreAllArgsImmediates()) {
                break;
            }

            const u32 vaddr = inst.GetArg(1).GetU32();

            auto it = cache.find(vaddr);
            if (it == cache.end()) {
                bool is_read_only = cb->IsReadOnlyMemory(vaddr);
                u64 value_from_memory = is_read_only ? cb->MemoryRead64(vaddr) : 0;
                it = cache.emplace(vaddr, std::make_pair(is_read_only, value_from_memory)).first;
            }

            if (it->second.first) {
                inst.ReplaceUsesWith(IR::Value{it->second.second});
            }
            break;
        }
        default:
            break;
        }
    }
}

}  // namespace Dynarmic::Optimization
