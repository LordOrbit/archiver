#pragma once

#include <cstdint>
#include <memory>
#include <stack>
#include <tuple>
#include <unordered_map>

#include "../long_code.h"

template <typename T>
struct Node {
    T value;
    size_t count = 1;
    std::shared_ptr<Node> left = nullptr;
    std::shared_ptr<Node> right = nullptr;

    bool HasLeft();
    bool HasRight();

    bool IsLeaf();
};

template <typename T>
class Trie {
public:
    Trie();
    explicit Trie(std::shared_ptr<Node<T>> root);

    void Add(const LongCode& code, T value);

    std::unordered_map<T, LongCode> GetMap() const;

    T& TraceValue() const;
    bool IsTraceLeaf() const;
    bool Trace(bool x);
    void ResetTrace();

    static std::shared_ptr<Node<T>> MergeNodes(std::shared_ptr<Node<T>> left, std::shared_ptr<Node<T>> right);

private:
    std::shared_ptr<Node<T>> traced_node_;
    std::shared_ptr<Node<T>> root_;
};

template <typename T>
Trie<T>::Trie(std::shared_ptr<Node<T>> root) : root_(root) {
    traced_node_ = root_;
}

template <typename T>
Trie<T>::Trie() {
    traced_node_ = root_ = std::make_shared<Node<T>>(Node<T>());
}

template <typename T>
std::shared_ptr<Node<T>> Trie<T>::MergeNodes(std::shared_ptr<Node<T>> left, std::shared_ptr<Node<T>> right) {
    if (left->value > right->value) {
        std::swap(left, right);
    }
    return std::make_shared<Node<T>>(
        Node<T>({.value = left->value, .count = left->count + right->count, .left = left, .right = right}));
}

// Walk through the Trie and give codes
template <typename T>
void FillCode(std::shared_ptr<Node<T>> node, std::unordered_map<T, LongCode>& huffman_code, LongCode current) {
    if (node == nullptr) {
        return;
    }
    if (node->IsLeaf()) {
        huffman_code[node->value] = current;
        return;
    }
    if (node->left != nullptr) {
        current.Push(false);
        FillCode(node->left, huffman_code, current);
        current.Pop();
    }
    if (node->right != nullptr) {
        current.Push(true);
        FillCode(node->right, huffman_code, current);
        current.Pop();
    }
}

template <typename T>
std::unordered_map<T, LongCode> Trie<T>::GetMap() const {
    std::unordered_map<T, LongCode> result;
    FillCode(root_, result, LongCode());
    return result;
}

template <typename T>
void Trie<T>::Add(const LongCode& code, T value) {
    auto node = root_;
    for (size_t bit = 0; bit < code.Size(); ++bit) {
        if (!code[bit]) {
            if (node->left == nullptr) {
                node->left = std::make_shared<Node<T>>();
            }
            node = node->left;
        } else {
            if (node->right == nullptr) {
                node->right = std::make_shared<Node<T>>();
            }
            node = node->right;
        }
    }
    node->value = value;
}

// Tracing functions across Trie

template <typename T>
bool Trie<T>::Trace(bool x) {
    if (x) {
        if (!traced_node_->HasRight()) {
            return false;
        }
        traced_node_ = traced_node_->right;
        return true;
    } else {
        if (!traced_node_->HasLeft()) {
            return false;
        }
        traced_node_ = traced_node_->left;
        return true;
    }
}

template <typename T>
void Trie<T>::ResetTrace() {
    traced_node_ = root_;
}

template <typename T>
T& Trie<T>::TraceValue() const {
    return traced_node_->value;
}

template <typename T>
bool Trie<T>::IsTraceLeaf() const {
    return traced_node_->IsLeaf();
}

// Node functions

template <typename T>
bool Node<T>::HasLeft() {
    return left != nullptr;
}

template <typename T>
bool Node<T>::HasRight() {
    return right != nullptr;
}

template <typename T>
bool Node<T>::IsLeaf() {
    return !HasLeft() && !HasRight();
}

// Node comparator for sorting by count of symbol and when equal by symbol
template <typename T>
struct NodeCompare {
    bool operator()(std::shared_ptr<Node<T>> n1, std::shared_ptr<Node<T>> n2) const {
        return std::tie(n1->count, n1->value) > std::tie(n2->count, n2->value);
    }
};
