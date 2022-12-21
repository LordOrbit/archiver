#pragma once

#include <ostream>
#include <string>

class Weight {
public:
    Weight() : weight_in_bytes_(0){};
    explicit Weight(size_t weight_in_bytes) : weight_in_bytes_(weight_in_bytes){};

    Weight operator+(const Weight& other) const;
    void operator+=(const Weight other);

    long double operator/(const Weight& other) const;

    std::strong_ordering operator<=>(const Weight& other) const;
    bool operator==(const Weight& other) const;

    Weight operator+(size_t other) const;
    void operator+=(size_t other);

    operator std::string() const;  // NOLINT

private:
    size_t weight_in_bytes_;
};

std::ostream& operator<<(std::ostream& stream, const Weight& weight);
