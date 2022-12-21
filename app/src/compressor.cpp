#include "compressor.h"

#include <cstring>
#include <ios>

#include "canonical_code.h"
#include "service_symbols.h"
#include "utils/bit_reader.h"
#include "utils/bit_writer.h"
#include "utils/counter.h"

// Get total weight of archive
Weight Compressor::ResultWeight() const {
    return result_weight_;
}

// Get total weight of given files
Weight Compressor::RawWeight() const {
    return raw_weight_;
}

void DeleteFile(const std::string path) {
    char* remove_file_path = new char[path.size()];
    std::strcpy(remove_file_path, path.c_str());
    std::remove(remove_file_path);
    delete[] remove_file_path;
}

// Compress given files and save compressed data in archive
void Compressor::Compress() {
    FileBitWriter bit_writer(archive_path);

    for (size_t file_index = 0; file_index < files_.size(); ++file_index) {
        const File& file = files_[file_index];

        try {

            // Counting the number of all characters
            FileBitReader reader(file.GetPath());
            Counter<CharT> counter;

            for (char c : file.GetName()) {
                counter.Add(c);
            }

            counter.Process(reader, FILE_FIXED_CHAR_SIZE);
            counter.Add(FILENAME_END);
            counter.Add(ONE_MORE_FILE);
            counter.Add(ARCHIVE_END);

            raw_weight_ += reader.ByteCount();

            CanonicalCodeGenerator canonical_code(counter);

            // Write file data
            bit_writer.Write(canonical_code.Size(), ARCHIVE_FIXED_CHAR_SIZE);
            for (const auto& character : canonical_code.Order()) {
                bit_writer.Write(character, ARCHIVE_FIXED_CHAR_SIZE);
            }
            for (const auto& size_count : canonical_code.CodeSizesCount()) {
                bit_writer.Write(size_count, ARCHIVE_FIXED_CHAR_SIZE);
            }
            for (char c : file.GetName()) {
                bit_writer.Write(canonical_code[c]);
            }
            bit_writer.Write(canonical_code[FILENAME_END]);

            // Write file content
            reader.Reset();
            CharT c = 0;
            while (reader.Get(c, FILE_FIXED_CHAR_SIZE)) {
                bit_writer.Write(canonical_code[c]);
            }

            if (file_index < files_.size() - 1) {
                bit_writer.Write(canonical_code[ONE_MORE_FILE]);
            } else {
                bit_writer.Write(canonical_code[ARCHIVE_END]);
            }

        } catch (const FileBitReader::FileNotExists& e) {
            DeleteFile(archive_path);
            throw;
        }
    }

    bit_writer.Complete();

    // Save compress weight result
    result_weight_ = Weight(bit_writer.ByteCount());
}

// Compressor constructor
Compressor::Compressor(std::vector<std::string>& files, const std::string& archive_name) : archive_path(archive_name) {
    files_.resize(files.size());
    for (size_t file_index = 0; file_index < files.size(); ++file_index) {
        files_[file_index] = File(files[file_index]);
    }
}

// Add extra file to compressing
void Compressor::AddFile(std::string& file) {
    files_.push_back(File(file));
}
