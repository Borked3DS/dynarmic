#ifndef CUSTOM_OSTREAM_H
#define CUSTOM_OSTREAM_H

#include <string>

namespace Dynarmic::A32 {

size_t strlen(const char* str) {
    const char* start = str;

    // Unroll the loop for the first few iterations
    for (int i = 0; i < 16; ++i) {
        if (*str == '\0') {
            return static_cast<size_t>(str - start);
        }
        ++str;
    }

    // Use a simple loop for the rest of the string
    while (*str != '\0') {
        ++str;
    }
    return static_cast<size_t>(str - start);
}

class ostream {
public:
    using OutputFunction = void (*)(const char*, size_t);

    ostream(OutputFunction output_function)
        : output_function(output_function) {}

    ostream& operator<<(const char* str) {
        output_function(str, strlen(str));
        return *this;
    }

    ostream& operator<<(const std::string& str) {
        output_function(str.c_str(), str.length());
        return *this;
    }

private:
    OutputFunction output_function;
};

} // namespace Dynarmic::A32

#endif // CUSTOM_OSTREAM_H
