#pragma once

#include <fstream>
#include <ios>
#include <iostream>
#include <istream>

#include "../long_code.h"

class BitReader {
protected:
    static const signed char READER_CHAR_START_POINTER = 7;
    static const size_t READER_BUFFER_SIZE = 1024;
    using BufferT = char;

public:
    explicit BitReader(std::istream& stream) : stream_(stream), buffer_size_(0), index_(0), byte_count_(0){};

    bool IsEOF();

    size_t ByteCount() const;

    void Reset();

    bool Get(bool& b);

    template <typename T>
    bool Get(T& t, size_t bit_count);

    bool Get(LongCode& long_code, size_t bit_count);

    void operator>>(bool& b);

protected:
    bool Update();

    std::istream& stream_;
    BufferT buffer_[READER_BUFFER_SIZE];
    size_t buffer_size_;
    ssize_t char_pointer_;   // Pointer to buffer char
    std::streamsize index_;  // Pointer to buffer char bit
    size_t byte_count_;      // Byte count that we read in total
};

// Get bit_count bit and write it to t
template <typename T>
bool BitReader::Get(T& t, size_t bit_count) {
    T new_t = 0;

    for (size_t bit = bit_count - 1;; --bit) {
        bool bit_value = false;
        if (!Get(bit_value)) {
            return false;
        }
        new_t |= (bit_value << bit);
        if (bit == 0) {
            break;
        }
    }

    t = new_t;
    return true;
}

class FileBitReader : public BitReader {
public:
    class FileNotExists : public std::exception {
    public:
        explicit FileNotExists(const std::string& file_name);

        const char* what() const noexcept override;

    private:
        char* description_;
    };

    explicit FileBitReader(const std::string path);

private:
    std::ifstream stream_;
};
