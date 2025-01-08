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

struct CachedMemoryEntry {
    bool is_read_only;
    u64 value;
};

void A32ConstantMemoryReads(IR::Block& block, A32::UserCallbacks* cb) {
    constexpr size_t kCacheSize = 1024;
    CachedMemoryEntry cache[kCacheSize] = {};
    bool cache_valid[kCacheSize] = {};

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
            const size_t index = vaddr % kCacheSize;

            if (cache_valid[index] && cache[index].is_read_only) {
                inst.ReplaceUsesWith(IR::Value{cache[index].value});
            } else {
                bool is_read_only = cb->IsReadOnlyMemory(vaddr);
                u64 value_from_memory = is_read_only ? cb->MemoryRead64(vaddr) : 0;
                cache[index] = {is_read_only, value_from_memory};
                cache_valid[index] = true;
            }
            break;
        }
        default:
            break;
        }
    }
}

}  // namespace Dynarmic::Optimization
