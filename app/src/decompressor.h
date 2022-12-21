#pragma once

#include <string>
#include <vector>

#include "service_symbols.h"
#include "utils/file.h"

class Decompressor {
public:
    class ArchiveDamagedError : public std::exception {
    public:
        explicit ArchiveDamagedError(const std::string& description);

        const char* what() const noexcept override;

    private:
        char* description_;
    };

    explicit Decompressor(std::string& archive_path) : archive_file_(File(archive_path)){};

    void Decompress();

    std::vector<File> GetFiles() const;

private:
    std::vector<File> files_;
    const File archive_file_;
};
