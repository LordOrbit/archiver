#include "bit_writer.h"

// Update buffer
void BitWriter::Update() {
    if (char_pointer_ < 0) {
        char_pointer_ = WRITER_CHAR_START_POINTER;
        ++byte_count_;
        ++index_;
        if (index_ < WRITER_BUFFER_SIZE) {
            buffer_[index_] = 0;
        }
    }
    if (index_ >= WRITER_BUFFER_SIZE) {
        stream_.write(buffer_, WRITER_BUFFER_SIZE);
        char_pointer_ = WRITER_CHAR_START_POINTER;
        buffer_[index_ = 0] = 0;
    }
}

// Write one bit
void BitWriter::Write(bool b) {
    buffer_[index_] = static_cast<char>(buffer_[index_] | b << char_pointer_--);

    Update();
}

// Write first bit_count bits of long_code
void BitWriter::Write(const LongCode& long_code, size_t bit_count) {
    for (size_t bit = 0; bit < bit_count; ++bit) {
        Write(long_code[bit]);
    }
}

// Write all bits of long_code
void BitWriter::Write(const LongCode& long_code) {
    for (size_t bit = 0; bit < long_code.Size(); ++bit) {
        Write(long_code[bit]);
    }
}

// Write one bit
void BitWriter::operator<<(bool b) {
    Write(b);
}

// Complete last buffer to write last bits to file
void BitWriter::Complete() {
    while (char_pointer_ < WRITER_CHAR_START_POINTER) {
        Write(false);
    }
    if (index_ > 0) {
        stream_.write(buffer_, index_);
        index_ = 0;
    }
}

// Close stream
void BitWriter::Close() {
    Complete();
}

BitWriter::~BitWriter() {
    BitWriter::Close();
}

size_t BitWriter::ByteCount() const {
    return byte_count_;
}

BitWriter::BitWriter(std::ostream& stream)
    : stream_(stream), char_pointer_(WRITER_CHAR_START_POINTER), index_(0), byte_count_(0) {
    buffer_[index_] = 0;
}

void FileBitWriter::Close() {
    BitWriter::Close();
    stream_.close();
}
