#pragma once

#include <unordered_map>
#include <vector>

#include "bit_reader.h"

template <typename T>
class Counter {
public:
    Counter() = default;

    size_t operator[](const T& value);

    void Add(const T& t, size_t count = 1);

    auto begin() const;  // NOLINT
    auto begin();        // NOLINT
    auto end() const;    // NOLINT
    auto end();          // NOLINT

    template <typename V>
    void Process(const V& values);

    void Process(BitReader& reader, size_t bit_count);

private:
    std::unordered_map<T, size_t> count_;
};

// Get count of specific value
template <typename T>
size_t Counter<T>::operator[](const T& value) {
    return count_[value];
}

// Add count to total count of specific value
template <typename T>
void Counter<T>::Add(const T& t, size_t count) {
    count_[t] += count;
}

// Count number of all given values
template <typename T>
template <typename V>
void Counter<T>::Process(const V& values) {
    for (T value : values) {
        Add(value);
    }
}

// Read and count all values from reader
template <typename T>
void Counter<T>::Process(BitReader& reader, size_t bit_count) {
    T t;
    while (reader.Get(t, bit_count)) {
        Add(t);
    }
}

// Iterators to count_
template <typename T>
auto Counter<T>::begin() {
    return count_.begin();
}

template <typename T>
auto Counter<T>::begin() const {
    return count_.begin();
}

template <typename T>
auto Counter<T>::end() {
    return count_.end();
}

template <typename T>
auto Counter<T>::end() const {
    return count_.end();
}
