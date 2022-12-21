#pragma once

#include <algorithm>
#include <array>
#include <memory>
#include <tuple>

#include "long_code.h"
#include "service_symbols.h"
#include "utils/counter.h"
#include "utils/priority_queue.h"
#include "utils/trie.h"

template <typename T>
class CanonicalCodeGenerator {
public:
    explicit CanonicalCodeGenerator(Counter<T>& counter);

    size_t Size() const;

    LongCode& operator[](const T& t);
    std::vector<T> Order() const;
    std::vector<size_t> CodeSizesCount() const;

private:
    std::array<LongCode, MAX_CHAR_VALUE + 1> translate_;
    std::vector<T> order_;
    std::vector<size_t> code_sizes_count_;
};

template <typename T>
struct Symbol {
    T t;
    LongCode code;
};

// Symbol comparator to sort to build canonical code
template <typename T>
bool SymbolComp(const Symbol<T>& a, const Symbol<T>& b) {
    if (a.code.Size() != b.code.Size()) {
        return a.code.Size() < b.code.Size();
    }
    return a.t < b.t;
}

template <typename T>
CanonicalCodeGenerator<T>::CanonicalCodeGenerator(Counter<T>& counter) {
    // Create Trie nodes
    std::vector<std::shared_ptr<Node<T>>> vector_nodes;
    for (const auto& [value, count] : counter) {
        vector_nodes.push_back(std::make_shared<Node<T>>(Node<T>({.value = value, .count = count})));
    }
    PriorityQueue<std::shared_ptr<Node<T>>, NodeCompare<T>> nodes(vector_nodes);

    // Merge nodes and build Trie
    while (nodes.Size() >= 2) {
        auto first = nodes.Pop();
        auto second = nodes.Pop();

        nodes.Push(Trie<T>::MergeNodes(first, second));
    }

    Trie<T> trie(nodes.Top());

    // Get huffman code
    std::unordered_map<T, LongCode> huffman_code = trie.GetMap();

    // Sort symbols by code length
    std::vector<Symbol<T>> symbols;
    for (const auto& [t, code] : huffman_code) {
        symbols.push_back({.t = t, .code = code});
    }
    std::sort(symbols.begin(), symbols.end(), SymbolComp<T>);

    // Build canonical huffman code
    LongCode current_code(symbols[0].code.Size());
    code_sizes_count_.resize(symbols.back().code.Size());
    for (size_t i = 0; i < symbols.size(); ++i) {
        translate_[symbols[i].t] = current_code;
        order_.push_back(symbols[i].t);
        ++code_sizes_count_[current_code.Size() - 1];
        ++current_code;
        if (i < symbols.size() - 1) {
            current_code = current_code << (symbols[i + 1].code.Size() - symbols[i].code.Size());
        }
    }
}

// Get canonical code for symbol t
template <typename T>
LongCode& CanonicalCodeGenerator<T>::operator[](const T& t) {
    return translate_[t];
}

// Number of symbols in alphabet
template <typename T>
size_t CanonicalCodeGenerator<T>::Size() const {
    return order_.size();
}

// Order of symbols in canonical codes order
template <typename T>
std::vector<T> CanonicalCodeGenerator<T>::Order() const {
    return order_;
}

// Get count of specific code size
template <typename T>
std::vector<size_t> CanonicalCodeGenerator<T>::CodeSizesCount() const {
    return code_sizes_count_;
}
