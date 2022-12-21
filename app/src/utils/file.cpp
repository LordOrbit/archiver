#include "file.h"

std::string File::GetPath() const {
    return path_;
}

std::string File::GetName() const {
    return name_;
}

Weight File::GetWeight() const {
    return weight_;
}

// Constructor File, where we get file_name from file_path
File::File(const std::string& path, size_t weight) : path_(path), weight_(Weight(weight)) {
    for (size_t i = path.size() - 1;; --i) {
        if (path[i] == '/') {
            break;
        }
        name_ += path[i];
        if (i == 0) {
            break;
        }
    }
    std::reverse(name_.begin(), name_.end());
}

File& File::operator=(const File& other) {
    path_ = other.path_;
    name_ = other.name_;
    weight_ = other.weight_;
    return *this;
}

File::File(const File& file) {
    *this = file;
}
