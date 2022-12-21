#include "long_code.h"

#include <string>

const bool LongCode::operator[](size_t index) const {
    return value_[index];
}

LongCode& LongCode::operator++() {
    ssize_t i = static_cast<ssize_t>(value_.size() - 1);
    while (i >= 0 && value_[i]) {
        value_[i--] = false;
    }

    if (i >= 0) {
        value_[i] = true;
    } else {
        std::vector<bool> new_value(value_.size() + 1);
        new_value[0] = true;
        value_ = new_value;
    }
    return *this;
}

LongCode LongCode::operator++(int) {
    LongCode result = *this;
    ++(*this);
    return result;
}

size_t LongCode::Size() const {
    return value_.size();
}

size_t LongCode::size() const {
    return Size();
}

void LongCode::Push(bool x) {
    return value_.push_back(x);
}

bool LongCode::Pop() {
    bool x = value_.back();
    value_.pop_back();
    return x;
}

LongCode& LongCode::operator<<(size_t count) {
    while (count--) {
        Push(false);
    }
    return *this;
}

LongCode LongCode::Copy() const {
    std::vector<bool> value(Size());
    std::copy(value_.begin(), value_.end(), value.begin());
    return LongCode(value);
}

bool LongCode::operator==(const LongCode& other) const {
    if (Size() != other.Size()) {
        return false;
    }
    for (size_t i = 0; i < Size(); ++i) {
        if ((*this)[i] != other[i]) {
            return false;
        }
    }
    return true;
}

bool LongCode::operator!=(const LongCode& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& stream, const LongCode& long_code) {
    for (size_t i = 0; i < long_code.Size(); ++i) {
        stream << std::to_string(long_code[i]);
    }
    return stream;
}
