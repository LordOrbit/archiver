#pragma once

#include <chrono>

template <typename DT = std::chrono::milliseconds, typename ClockT = std::chrono::steady_clock>
class Timer {
    using TimepT = decltype(ClockT::now());

    TimepT start_ = ClockT::now();
    TimepT end_ = {};

public:
    void Tick() {
        end_ = TimepT{};
        start_ = ClockT::now();
    }

    void Tock() {
        end_ = ClockT::now();
    }

    template <class DurationT = DT>
    auto Duration() const {
        return std::chrono::duration_cast<DurationT>(end_ - start_);
    }
};
