/* This file is part of the dynarmic project.
 * Copyright (c) 2018 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include "dynarmic/common/math_util.h"

#include <array>

namespace Dynarmic::Common {

constexpr std::array<u8, 256> GenerateRecipEstimateLUT() {
    std::array<u8, 256> lut{};
    for (u64 i = 0; i < lut.size(); i++) {
        u64 a = i + 256;
        a = a * 2 + 1;
        u64 b = (1u << 19) / a;
        lut[i] = static_cast<u8>((b + 1) / 2);
    }
    return lut;
}

constexpr std::array<u8, 512> GenerateRecipSqrtEstimateLUT() {
    std::array<u8, 512> lut{};
    for (u64 i = 128; i < lut.size(); i++) {
        u64 a = i;
        if (a < 256) {
            a = a * 2 + 1;
        } else {
            a = (a | 1) * 2;
        }
        u64 b = 512;
        while (a * (b + 1) * (b + 1) < (1u << 28)) {
            b++;
        }
        lut[i] = static_cast<u8>((b + 1) / 2);
    }
    return lut;
}

constexpr auto RecipEstimateLUT = GenerateRecipEstimateLUT();
constexpr auto RecipSqrtEstimateLUT = GenerateRecipSqrtEstimateLUT();

u8 RecipEstimate(u64 a) {
    return RecipEstimateLUT[a - 256];
}

u8 RecipSqrtEstimate(u64 a) {
    return RecipSqrtEstimateLUT[a & 0x1FF];
}

}  // namespace Dynarmic::Common
