#pragma once

#include <string>

#include "weight.h"

class File {
public:
    File() = default;
    explicit File(const std::string& path, size_t weight = 0);

    File(const File& file);

    File& operator=(const File& other);

    std::string GetPath() const;
    std::string GetName() const;
    Weight GetWeight() const;

private:
    std::string path_;
    std::string name_;
    Weight weight_;
};
