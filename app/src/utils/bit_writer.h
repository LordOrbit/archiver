#pragma once

#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <string>

#include "../long_code.h"

class BitWriter {
protected:
    static const signed char WRITER_CHAR_START_POINTER = 7;
    static const size_t WRITER_BUFFER_SIZE = 1024;
    using BufferT = char;

public:
    explicit BitWriter(std::ostream& stream);

    ~BitWriter();

    template <typename T>
    void Write(const T& t, size_t bit_count);

    void Write(const LongCode& long_code, size_t bit_count);
    void Write(const LongCode& long_code);
    void Write(bool b);

    size_t ByteCount() const;

    void Complete();
    virtual void Close();

    void operator<<(bool b);

protected:
    void Update();

    std::ostream& stream_;
    BufferT buffer_[WRITER_BUFFER_SIZE];
    ssize_t char_pointer_;
    std::streamsize index_;
    size_t byte_count_;
};

// Write first bit_count bits from t
template <typename T>
void BitWriter::Write(const T& t, size_t bit_count) {
    for (ssize_t bit = static_cast<ssize_t>(bit_count - 1); bit >= 0; --bit) {
        Write((t >> bit) & 1);
    }
}

class FileBitWriter : public BitWriter {
public:
    explicit FileBitWriter(const std::string path)
        : BitWriter(stream_), stream_(std::ofstream(path, std::ios::binary | std::ios::out)){};

    void Close() override;

private:
    std::ofstream stream_;
};
