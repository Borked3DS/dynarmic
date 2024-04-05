/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include "dynarmic/backend/x64/a64_jitstate.h"

#include <emmintrin.h>

#include "dynarmic/frontend/A64/a64_location_descriptor.h"

namespace Dynarmic::Backend::X64 {

/**
 * Comparing MXCSR and FPCR
 * ========================
 *
 * SSE MSCSR exception masks
 * -------------------------
 * PM   bit 12  Precision Mask
 * UM   bit 11  Underflow Mask
 * OM   bit 10  Overflow Mask
 * ZM   bit 9   Divide By Zero Mask
 * DM   bit 8   Denormal Mask
 * IM   bit 7   Invalid Operation Mask
 *
 * A64 FPCR exception trap enables
 * -------------------------------
 * IDE  bit 15  Input Denormal exception trap enable
 * IXE  bit 12  Inexact exception trap enable
 * UFE  bit 11  Underflow exception trap enable
 * OFE  bit 10  Overflow exception trap enable
 * DZE  bit 9   Division by Zero exception trap enable
 * IOE  bit 8   Invalid Operation exception trap enable
 *
 * SSE MXCSR mode bits
 * -------------------
 * FZ   bit 15  Flush To Zero
 * DAZ  bit 6   Denormals Are Zero
 * RN   bits 13-14  Round to {0 = Nearest, 1 = Negative, 2 = Positive, 3 = Zero}
 *
 * A64 FPCR mode bits
 * ------------------
 * AHP  bit 26  Alternative half-precision
 * DN   bit 25  Default NaN
 * FZ   bit 24  Flush to Zero
 * RMode    bits 22-23  Round to {0 = Nearest, 1 = Positive, 2 = Negative, 3 = Zero}
 * FZ16 bit 19  Flush to Zero for half-precision
 */

constexpr u32 FPCR_MASK = 0x07C89F00;

u32 A64JitState::GetFpcr() const {
    return fpcr;
}

void A64JitState::SetFpcr(u32 value) {
    // Use SIMD instructions on x64
    __m128i vValue = _mm_set_epi32(0, 0, 0, value);

    // FPCR
    fpcr = _mm_cvtsi128_si32(_mm_and_si128(vValue, _mm_set1_epi32(FPCR_MASK)));

    // MXCSR
    asimd_MXCSR &= 0x0000003D;
    guest_MXCSR &= 0x0000003D;
    asimd_MXCSR |= 0x00001f80;
    guest_MXCSR |= 0x00001f80;  // Mask all exceptions

    // RMode
    __m128i vRMode = _mm_and_si128(_mm_srli_epi32(vValue, 22), _mm_set1_epi32(0x3));
    const __m128i vMXCSR_RMode = _mm_set_epi32(0x6000, 0x2000, 0x4000, 0x0);
    guest_MXCSR |= _mm_cvtsi128_si32(_mm_shuffle_epi32(_mm_and_si128(vRMode, vMXCSR_RMode), _MM_SHUFFLE(0, 0, 0, 0)));

    // Flush to Zero
    __m128i vFlushToZero = _mm_and_si128(_mm_srli_epi32(vValue, 24), _mm_set1_epi32(1));
    if (_mm_cvtsi128_si32(vFlushToZero)) {
        guest_MXCSR |= (1 << 15);  // SSE Flush to Zero
        guest_MXCSR |= (1 << 6);   // SSE Denormals are Zero
    }
}

/**
 * Comparing MXCSR and FPSR
 * ========================
 *
 * SSE MXCSR exception flags
 * -------------------------
 * PE   bit 5   Precision Flag
 * UE   bit 4   Underflow Flag
 * OE   bit 3   Overflow Flag
 * ZE   bit 2   Divide By Zero Flag
 * DE   bit 1   Denormal Flag                                 // Appears to only be set when MXCSR.DAZ = 0
 * IE   bit 0   Invalid Operation Flag
 *
 * A64 FPSR cumulative exception bits
 * ----------------------------------
 * QC   bit 27  Cumulative saturation bit
 * IDC  bit 7   Input Denormal cumulative exception bit       // Only ever set when FPCR.FTZ = 1
 * IXC  bit 4   Inexact cumulative exception bit
 * UFC  bit 3   Underflow cumulative exception bit
 * OFC  bit 2   Overflow cumulative exception bit
 * DZC  bit 1   Division by Zero cumulative exception bit
 * IOC  bit 0   Invalid Operation cumulative exception bit
 */

u32 A64JitState::GetFpsr() const {
    // Use SIMD instructions on x64
    const u32 mxcsr = guest_MXCSR | asimd_MXCSR;
    __m128i vMXCSR = _mm_set_epi32(0, 0, 0, mxcsr);

    __m128i vIOC = _mm_and_si128(_mm_set1_epi32(0b0000000000001), vMXCSR);
    __m128i vIXCUFCOFCDZC = _mm_and_si128(_mm_set1_epi32(0b0000000111100), vMXCSR);

    u32 fpsr = 0;
    fpsr |= _mm_cvtsi128_si32(vIOC);                 // IOC = IE
    fpsr |= _mm_cvtsi128_si32(_mm_srli_epi32(vIXCUFCOFCDZC, 1)); // IXC, UFC, OFC, DZC = PE, UE, OE, ZE
    fpsr |= fpsr_exc;
    fpsr |= (fpsr_qc == 0 ? 0 : 1) << 27;
    return fpsr;
}

void A64JitState::SetFpsr(u32 value) {
    // Use SIMD instructions on x64
    __m128i vValue = _mm_set_epi32(0, 0, 0, value);

    // Clear the relevant bits in guest_MXCSR and asimd_MXCSR
    guest_MXCSR &= ~0x0000003D;
    asimd_MXCSR &= ~0x0000003D;

    // Set the Q flag
    __m128i vQFlag = _mm_and_si128(_mm_srli_epi32(vValue, 27), _mm_set1_epi32(1));
    fpsr_qc = _mm_cvtsi128_si32(vQFlag);

    // Set the exception flags
    fpsr_exc = _mm_cvtsi128_si32(_mm_and_si128(vValue, _mm_set1_epi32(0x9F)));
}

}  // namespace Dynarmic::Backend::X64
