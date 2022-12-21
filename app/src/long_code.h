#pragma once

#include <sstream>
#include <vector>

class LongCode {
public:
    explicit LongCode(const std::vector<bool>& value) : value_(value){};
    explicit LongCode(size_t size) : value_(size){};
    LongCode(const LongCode& long_code) : value_(long_code.value_){};
    LongCode() = default;

    const bool operator[](size_t index) const;

    LongCode Copy() const;

    bool operator==(const LongCode& other) const;
    bool operator!=(const LongCode& other) const;

    size_t Size() const;
    size_t size() const;  // NOLINT

    void Push(bool x);
    bool Pop();

    LongCode& operator<<(size_t count);

    LongCode& operator++();
    LongCode operator++(int);

private:
    std::vector<bool> value_;
};

std::ostream& operator<<(std::ostream& stream, const LongCode& long_code);
