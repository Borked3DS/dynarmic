#ifndef CUSTOM_OSTREAM_H
#define CUSTOM_OSTREAM_H

#include <string>
#include <cstring>

namespace Dynarmic::A32 {
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
