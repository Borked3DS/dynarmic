/* This file is part of the dynarmic project.
 * Copyright (c) 2018 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#include "dynarmic/backend/x64/perf_map.h"

#include <cstddef>
#include <string>

#ifdef __linux__

#    include <cstdio>
#    include <cstdlib>
#    include <atomic>

#    include <fmt/format.h>
#    include <mcl/stdint.hpp>
#    include <sys/types.h>
#    include <unistd.h>

namespace Dynarmic::Backend::X64 {

namespace {
std::atomic<bool> lock = false;
std::FILE* file = nullptr;

void OpenFile() {
    const char* perf_dir = std::getenv("PERF_BUILDID_DIR");
    if (!perf_dir) {
        file = nullptr;
        return;
    }

    const pid_t pid = getpid();
    const std::string filename = fmt::format("{:s}/perf-{:d}.map", perf_dir, pid);

    file = std::fopen(filename.c_str(), "w");
    if (!file) {
        return;
    }

    std::setvbuf(file, nullptr, _IONBF, 0);
}

bool TryLock() {
    bool expected = false;
    return lock.compare_exchange_strong(expected, true);
}

void Unlock() {
    lock.store(false, std::memory_order_release);
}
}  // anonymous namespace

namespace detail {
void PerfMapRegister(const void* start, const void* end, std::string_view friendly_name) {
    if (start == end) {
        // Nothing to register
        return;
    }

    std::lock_guard guard{mutex};

    if (!file) {
        OpenFile();
        if (!file) {
            return;
        }
    }

    while (!TryLock()) {
        // Spin
    }

    const std::string line = fmt::format("{:016x} {:016x} {:s}\n", reinterpret_cast<u64>(start), reinterpret_cast<u64>(end) - reinterpret_cast<u64>(start), friendly_name);
    std::fwrite(line.data(), sizeof *line.data(), line.size(), file);

    Unlock();
}
}  // namespace detail

void PerfMapClear() {
    while (!TryLock()) {
        // Spin
    }

    if (!file) {
        Unlock();
        return;
    }

    std::fclose(file);
    file = nullptr;
    OpenFile();

    Unlock();
}

}  // namespace Dynarmic::Backend::X64

#else

namespace Dynarmic::Backend::X64 {

namespace detail {
void PerfMapRegister(const void*, const void*, std::string_view) {}
}  // namespace detail

void PerfMapClear() {}

}  // namespace Dynarmic::Backend::X64

#endif
