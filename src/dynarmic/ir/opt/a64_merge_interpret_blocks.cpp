/* This file is part of the dynarmic project.
 * Copyright (c) 2018 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include <boost/variant/get.hpp>
#include <mcl/stdint.hpp>

#include "dynarmic/frontend/A64/a64_location_descriptor.h"
#include "dynarmic/frontend/A64/translate/a64_translate.h"
#include "dynarmic/interface/A64/config.h"
#include "dynarmic/ir/basic_block.h"
#include "dynarmic/ir/opt/passes.h"

namespace Dynarmic::Optimization {

void A64MergeInterpretBlocksPass(IR::Block& block, A64::UserCallbacks* cb) {
    IR::Terminal terminal = block.GetTerminal();
    if (auto term = boost::get<IR::Term::Interpret>(&terminal)) {
        A64::LocationDescriptor location{term->next};
        size_t num_instructions = 1;

        while (true) {
            std::optional<u32> instruction_opt = cb->MemoryReadCode(location.PC());
            if (!instruction_opt.has_value())
                break;

            const u32 instruction = instruction_opt.value();
            IR::Block new_block{location};
            A64::TranslateSingleInstruction(new_block, location, instruction);

            if (!new_block.Instructions().empty())
                break;

            IR::Terminal new_terminal = new_block.GetTerminal();
            if (auto new_term = boost::get<IR::Term::Interpret>(&new_terminal)) {
                if (new_term->next != location.AdvancePC(static_cast<int>(num_instructions * 4)))
                    break;
                num_instructions++;
                location = A64::LocationDescriptor{new_term->next};
            } else {
                break;
            }
        }

        term->num_instructions = num_instructions;
        block.ReplaceTerminal(terminal);
        block.CycleCount() += num_instructions - 1;
    }
}

}  // namespace Dynarmic::Optimization
