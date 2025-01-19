#pragma once

namespace Dynarmic::Common {

#if defined(_MSC_VER)
inline uint64_t RotateRight64(uint64_t value, unsigned int amount) {
    return _rotr64(value, amount);
}
#elif defined(__GNUC__) || defined(__clang__)
inline uint64_t RotateRight64(uint64_t value, unsigned int amount) {
    return __rorq(value, amount);
}
#else
#    error "Unsupported compiler - please implement RotateRight64 for this platform"
#endif

}  // namespace Dynarmic::Common
