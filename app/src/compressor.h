#pragma once

#include <string>
#include <vector>

#include "service_symbols.h"
#include "utils/file.h"
#include "utils/weight.h"

class Compressor {
public:
    const std::string archive_path;

public:
    explicit Compressor(std::vector<std::string>& files, const std::string& archive_name = "result.arc");

    void AddFile(std::string& file);

    void Compress();

    Weight ResultWeight() const;
    Weight RawWeight() const;

private:
    std::vector<File> files_;
    Weight result_weight_;
    Weight raw_weight_;
};
