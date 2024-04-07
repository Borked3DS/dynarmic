/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */
 
#include <mcl/assert.hpp>
#include <mcl/stdint.hpp>
#include <cstdio>
#include <cstring>

#include "dynarmic/ir/basic_block.h"
#include "dynarmic/ir/microinstruction.h"
#include "dynarmic/ir/opcodes.h"
#include "dynarmic/ir/opt/passes.h"
#include "dynarmic/ir/type.h"

namespace Dynarmic::Optimization {

// Define the hash table size
#define HASH_TABLE_SIZE 1024

// Hash function for IR::Inst*
static size_t IR_Inst_Hash(const IR::Inst* inst) {
    return reinterpret_cast<size_t>(inst) % HASH_TABLE_SIZE;
}

// Hash table node
struct HashNode {
    const IR::Inst* key;
    size_t value;
    HashNode* next;
};

// Hash table
static HashNode hash_table[HASH_TABLE_SIZE * 2] = {0};
static size_t hash_table_size = 0;

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

    // Initialize the hash table
    memset(hash_table, 0, sizeof(hash_table));
    hash_table_size = 0;

    // Populate the hash table
    for (const auto& inst : block) {
        for (size_t i = 0; i < inst.NumArgs(); i++) {
            const auto arg = inst.GetArg(i);
            if (!arg.IsImmediate()) {
                size_t hash = IR_Inst_Hash(arg.GetInst());
                HashNode* node = &hash_table[hash];
                while (node->key != nullptr && node->key != arg.GetInst()) {
                    node++;
                    hash_table_size++;
                    if (hash_table_size >= sizeof(hash_table) / sizeof(HashNode)) {
                        ASSERT_FALSE("Hash table overflow");
                    }
                }
                if (node->key == nullptr) {
                    node->key = arg.GetInst();
                    node->value = 0;
                    node->next = nullptr;
                }
                node->value++;
            }
        }
    }

    // Verify the use counts
    for (size_t i = 0; i < hash_table_size; i++) {
        const HashNode* node = &hash_table[i];
        if (node->key != nullptr) {
            ASSERT(node->key->UseCount() == node->value);
        }
    }
}

}  // namespace Dynarmic::Optimization
