#pragma once

#ifndef __has_builtin           // Optional of course.
#    define __has_builtin(x) 0  // Compatibility with non-clang compilers.
#endif

namespace Dynarmic::Common {

#if __has_builtin(_rotr64)
inline uint64_t RotateRight64(uint64_t value, unsigned int amount) {
    return _rotr64(value, amount);
}
#elif __has_builtin(__builtin_rotateright64)
inline uint64_t RotateRight64(uint64_t value, unsigned int amount) {
    return __builtin_rotateright64(value, amount);
}
#elif __has_builtin(__rorq)
inline uint64_t RotateRight64(uint64_t value, unsigned int amount) {
    return __rorq(value, amount);
}
#endif
}  // namespace Dynarmic::Common
