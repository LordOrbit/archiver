#include "bit_reader.h"

#include <cstring>

// Update buffer of BitReader and return true, if there is one bit on the way
bool BitReader::Update() {
    if (char_pointer_ < 0) {
        ++index_;
        char_pointer_ = READER_CHAR_START_POINTER;
        ++byte_count_;
    }
    if (index_ >= buffer_size_) {
        if (stream_.eof()) {
            return false;
        }
        stream_.read(buffer_, READER_BUFFER_SIZE);
        buffer_size_ = stream_.gcount();
        if (stream_.eof() && buffer_size_ == 0) {
            return false;
        }
        index_ = 0;
        char_pointer_ = READER_CHAR_START_POINTER;
    }
    return true;
}

// Get one bit
bool BitReader::Get(bool& b) {
    if (!Update()) {
        return false;
    }

    b = (buffer_[index_] >> char_pointer_--) & 1;
    return true;
}

// Is end of line
bool BitReader::IsEOF() {
    return !Update();
}

size_t BitReader::ByteCount() const {
    return byte_count_;
}

// Read one bit
void BitReader::operator>>(bool& b) {
    Get(b);
}

// Get bit_count bit and write it to long_code
bool BitReader::Get(LongCode& long_code, size_t bit_count) {
    std::vector<bool> new_code(bit_count);

    for (size_t bit = bit_count - 1;; --bit) {
        bool bit_value = false;
        if (!Get(bit_value)) {
            return false;
        }
        new_code[bit] = bit_value;
        if (bit == 0) {
            break;
        }
    }

    long_code = LongCode(new_code);
    return true;
}

void BitReader::Reset() {
    stream_.clear();
    stream_.seekg(0);
    buffer_size_ = 0;
    index_ = 0;
    byte_count_ = 0;
}

// Constructor of BitReader by read path
FileBitReader::FileBitReader(const std::string path)
    : BitReader(stream_), stream_(std::ifstream(path, std::ios::binary | std::ios::in)) {
    if (stream_.fail()) {
        throw FileNotExists(path);
    }
}

// Exceptions

// Constructor of FileNotExists
FileBitReader::FileNotExists::FileNotExists(const std::string& file_name) {
    std::string full_description = "File \"" + file_name + "\" not exists";
    description_ = new char[full_description.size()];
    std::strcpy(description_, full_description.c_str());
}

// Return more information about FileNotExists exception
const char* FileBitReader::FileNotExists::what() const noexcept {
    return description_;
}
