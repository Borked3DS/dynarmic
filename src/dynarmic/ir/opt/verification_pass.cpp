/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */
 
#include <mcl/assert.hpp>
#include <mcl/stdint.hpp>
#include <cstdio>
#include <cstdlib>
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

// Linked list node for the hash table
typedef struct HashNode {
    const IR::Inst* key;
    size_t value;
    struct HashNode* next;
} HashNode;

// Initialize the hash table
static HashNode* hash_table[HASH_TABLE_SIZE] = {0};

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

    // Populate the hash table
    for (const auto& inst : block) {
        for (size_t i = 0; i < inst.NumArgs(); i++) {
            const auto arg = inst.GetArg(i);
            if (!arg.IsImmediate()) {
                size_t hash = IR_Inst_Hash(arg.GetInst());
                HashNode* node = hash_table[hash];
                while (node != nullptr && node->key != arg.GetInst()) {
                    node = node->next;
                }
                if (node == nullptr) {
                    node = static_cast<HashNode*>(std::malloc(sizeof(HashNode)));
                    node->key = arg.GetInst();
                    node->value = 0;
                    node->next = hash_table[hash];
                    hash_table[hash] = node;
                }
                node->value++;
            }
        }
    }

    // Verify the use counts
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* node = hash_table[i];
        while (node != nullptr) {
            ASSERT(node->key->UseCount() == node->value);
            node = node->next;
        }
    }

    // Cleanup the hash table
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* node = hash_table[i];
        while (node != nullptr) {
            HashNode* next = node->next;
            std::free(node);
            node = next;
        }
    }
}

}  // namespace Dynarmic::Optimization
