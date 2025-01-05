/* This file is part of the dynarmic project.
 * Copyright (c) 2023 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include "dynarmic/backend/x64/verbose_debugging_output.h"

#include <cinttypes>
#include <cstdarg>
#include <iterator>

#include "dynarmic/backend/x64/hostloc.h"

namespace Dynarmic::Backend::X64 {

// Custom formatting function
size_t FormatString(char* buffer, size_t buffer_size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    size_t len = vsnprintf(buffer, buffer_size, format, args);
    va_end(args);
    return len;
}

void PrintVerboseDebuggingOutputLine(RegisterData& reg_data, HostLoc hostloc, size_t inst_index, size_t bitsize) {
    constexpr size_t buffer_size = 128;
    char buffer[buffer_size];
    size_t len = 0;

    len += FormatString(buffer, buffer_size, "dynarmic debug: %05zu = ", inst_index);

    constexpr std::array<const char*, 5> hex_formats = {"%02x", "%04x", "%08x", "%016" PRIx64, "%016" PRIx64 "%016" PRIx64};
    const char* hex_format = hex_formats[bitsize / 8 - 1];

    Vector value = [&]() -> Vector {
        if (HostLocIsGPR(hostloc)) {
            return {reg_data.gprs[HostLocToReg64(hostloc).getIdx()], 0};
        } else if (HostLocIsXMM(hostloc)) {
            return reg_data.xmms[HostLocToXmm(hostloc).getIdx()];
        } else if (HostLocIsSpill(hostloc)) {
            return (*reg_data.spill)[static_cast<size_t>(hostloc) - static_cast<size_t>(HostLoc::FirstSpill)];
        } else {
            len += FormatString(buffer + len, buffer_size - len, "invalid hostloc! ");
            return {0, 0};
        }
    }();

    len += FormatString(buffer + len, buffer_size - len, hex_format, value[0] & ((1ULL << bitsize) - 1));
    if (bitsize == 128) {
        len += FormatString(buffer + len, buffer_size - len, "%016" PRIx64, value[1]);
    }

    buffer[len++] = '\n';
    buffer[len] = '\0';
    fputs(buffer, stdout);
}

}  // namespace Dynarmic::Backend::X64
