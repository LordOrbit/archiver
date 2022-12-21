#pragma once

#include <algorithm>
#include <vector>

template <typename T, typename Comp = std::less<T>>
class PriorityQueue {
public:
    PriorityQueue() : values_(), compare_(Comp()){};
    explicit PriorityQueue(const std::vector<T>& values);

    size_t Size() const;
    bool Empty() const;

    const T& Top() const;
    const T Pop();
    void Push(const T& t);

private:
    void SiftDown(size_t i);
    void SiftUp(size_t i);

    std::vector<T> values_;
    Comp compare_;
};

template <typename T, typename Comp>
size_t PriorityQueue<T, Comp>::Size() const {
    return values_.size();
}

template <typename T, typename Comp>
bool PriorityQueue<T, Comp>::Empty() const {
    return Size() == 0;
}

template <typename T, typename Comp>
const T& PriorityQueue<T, Comp>::Top() const {
    return values_[0];
}

template <typename T, typename Comp>
const T PriorityQueue<T, Comp>::Pop() {
    T pop_value = Top();

    if (values_.size() > 1) {
        std::swap(values_[0], values_[values_.size() - 1]);
        values_.pop_back();
        SiftDown(0);
    } else {
        values_.pop_back();
    }

    return pop_value;
}

template <typename T, typename Comp>
void PriorityQueue<T, Comp>::Push(const T& t) {
    values_.push_back(t);
    SiftUp(values_.size() - 1);
}

template <typename T, typename Comp>
void PriorityQueue<T, Comp>::SiftDown(size_t i) {
    while (2 * i + 1 < values_.size()) {
        size_t left = i * 2 + 1;
        size_t right = i * 2 + 2;

        size_t largest = left;
        if (right < values_.size() && compare_(values_[left], values_[right])) {
            largest = right;
        }

        if (compare_(values_[largest], values_[i])) {
            break;
        }
        std::swap(values_[i], values_[largest]);
        i = largest;
    }
}

template <typename T, typename Comp>
void PriorityQueue<T, Comp>::SiftUp(size_t i) {
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (!compare_(values_[parent], values_[i])) {
            break;
        }
        std::swap(values_[parent], values_[i]);
        i = parent;
    }
}

template <typename T, typename Comp>
PriorityQueue<T, Comp>::PriorityQueue(const std::vector<T>& values) : values_(values), compare_(Comp()) {
    for (ssize_t i = static_cast<ssize_t>(values.size() / 2 - 1); i >= 0; --i) {
        SiftDown(i);
    }
}
