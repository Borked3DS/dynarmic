#ifndef CUSTOM_PUTS_HPP
#define CUSTOM_PUTS_HPP

inline void custom_puts(const char* str) {
    while (*str != '\0') {
        __builtin_putchar(*str++);
    }
}

#endif // CUSTOM_PUTS_HPP
