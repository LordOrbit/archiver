#include "weight.h"

#include <cmath>
#include <vector>

#include "round.h"

std::string RemoveTrailingZeros(const std::string& s) {
    std::string result;
    bool has_point = std::find(s.begin(), s.end(), '.') != s.end();
    if (!has_point) {
        return s;
    }

    size_t last_trailing_zero_index = s.find_last_not_of('0') + 1;

    for (size_t i = 0; i < last_trailing_zero_index && !(s[i] == '.' && i + 1 >= last_trailing_zero_index); ++i) {
        result += s[i];
    }
    return result;
}

// Get easy to view weight of something
Weight::operator std::string() const {
    if (weight_in_bytes_ < 1024) {
        return std::to_string(weight_in_bytes_) + "B";
    }

    std::vector<std::pair<std::string, size_t>> weight_names = {
        {"Kb", 1024}, {"Mb", 1024 * 1024}, {"Gb", 1024 * 1024 * 1024}};
    for (const auto& [weight_name, weight] : weight_names) {
        if (weight_in_bytes_ < 10 * weight) {
            return RemoveTrailingZeros(std::to_string(
                       Round(static_cast<long double>(weight_in_bytes_) / static_cast<long double>(weight), 3))) +
                   weight_name;
        }
        if (weight_in_bytes_ < 100 * weight) {
            return RemoveTrailingZeros(std::to_string(
                       Round(static_cast<long double>(weight_in_bytes_) / static_cast<long double>(weight), 2))) +
                   weight_name;
        }
        if (weight_in_bytes_ < 1024 * weight) {
            return RemoveTrailingZeros(std::to_string(
                       Round(static_cast<long double>(weight_in_bytes_) / static_cast<long double>(weight), 1))) +
                   weight_name;
        }
    }

    return RemoveTrailingZeros(std::to_string(Round(
               static_cast<long double>(weight_in_bytes_) / static_cast<long double>(weight_names.back().second), 1))) +
           weight_names.back().first;
}

void Weight::operator+=(const Weight other) {
    weight_in_bytes_ += other.weight_in_bytes_;
}

Weight Weight::operator+(const Weight& other) const {
    Weight new_weight = *this;
    new_weight += other;
    return new_weight;
}

long double Weight::operator/(const Weight& other) const {
    return static_cast<long double>(weight_in_bytes_) / static_cast<long double>(other.weight_in_bytes_);
}

void Weight::operator+=(size_t other) {
    weight_in_bytes_ += other;
}

Weight Weight::operator+(size_t other) const {
    Weight new_weight = *this;
    new_weight += other;
    return new_weight;
}

std::strong_ordering Weight::operator<=>(const Weight& weight) const {
    return this->weight_in_bytes_ <=> weight.weight_in_bytes_;
}

bool Weight::operator==(const Weight& weight) const {
    return weight_in_bytes_ == weight.weight_in_bytes_;
}

// Print weight in easy to view format
std::ostream& operator<<(std::ostream& stream, const Weight& weight) {
    stream << static_cast<std::string>(weight);
    return stream;
}
