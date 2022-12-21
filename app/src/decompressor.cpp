#include "decompressor.h"

#include <cstring>
#include <unordered_map>
#include <vector>

#include "long_code.h"
#include "service_symbols.h"
#include "utils/bit_reader.h"
#include "utils/bit_writer.h"
#include "utils/trie.h"

void Decompressor::Decompress() {
    FileBitReader bit_reader(archive_file_.GetPath());

    while (true) {
        size_t symbols_count = 0;

        // Read symbols count
        bit_reader.Get(symbols_count, ARCHIVE_FIXED_CHAR_SIZE);

        // Read symbols order
        std::vector<CharT> symbols_order(symbols_count);
        for (size_t i = 0; i < symbols_count; ++i) {
            CharT symbol = 0;
            if (!bit_reader.Get(symbol, ARCHIVE_FIXED_CHAR_SIZE)) {
                throw ArchiveDamagedError("Can't read symbols order");
            }
            symbols_order[i] = symbol;
        }

        // Read code sizes count
        size_t sum_size_count = 0;
        std::vector<size_t> size_counts;
        while (sum_size_count < symbols_count) {
            size_t size_count = 0;
            if (!bit_reader.Get(size_count, ARCHIVE_FIXED_CHAR_SIZE)) {
                break;
            }
            sum_size_count += size_count;
            size_counts.push_back(size_count);
        }
        if (sum_size_count != symbols_count) {
            throw ArchiveDamagedError("Can't read code sizes count");
        }

        // Recovery canonical code table
        std::unordered_map<CharT, LongCode> canonical_codes;
        size_t cur_size = 0;
        while (cur_size < size_counts.size() && size_counts[cur_size] == 0) {
            ++cur_size;
        }
        if (cur_size >= size_counts.size()) {
            throw ArchiveDamagedError("Can't start building code table");
        }
        size_t cur_count_sum = size_counts[cur_size];

        LongCode cur_code(cur_size + 1);
        for (size_t i = 0; i < symbols_count; ++i) {
            canonical_codes[symbols_order[i]] = cur_code.Copy();

            if (i < symbols_count - 1) {
                size_t new_size = cur_size;
                while (i + 1 >= cur_count_sum) {
                    cur_count_sum += size_counts[++new_size];
                }
                ++cur_code;
                cur_code = cur_code << (new_size - cur_size);
                cur_size = new_size;
            }
        }

        // Build trie with recovered canonical codes
        Trie<CharT> trie;
        for (const auto& symbol : symbols_order) {
            trie.Add(canonical_codes[symbol], symbol);
        }

        // Read and decompress file name
        std::string file_name;
        bool filename_end = false;

        bool cur_bit = false;
        while (bit_reader.Get(cur_bit)) {
            if (!trie.Trace(cur_bit)) {
                return throw ArchiveDamagedError("Can't decode file_name char code");
            }
            if (trie.IsTraceLeaf()) {
                if (trie.TraceValue() == FILENAME_END) {
                    trie.ResetTrace();
                    filename_end = true;
                    break;
                }
                file_name += static_cast<char>(trie.TraceValue());
                trie.ResetTrace();
            }
        }
        if (!filename_end) {
            return throw ArchiveDamagedError("Can't read file_name");
        }

        bool one_more_file = false;
        bool archive_end = false;

        FileBitWriter bit_writer(file_name);

        size_t file_size = 0;

        // Read and decompress file content
        while (bit_reader.Get(cur_bit)) {
            if (!trie.Trace(cur_bit)) {
                return throw ArchiveDamagedError("Can't decode content char code");
            }
            if (trie.IsTraceLeaf()) {
                if (trie.TraceValue() == ONE_MORE_FILE) {
                    one_more_file = true;
                    break;
                }
                if (trie.TraceValue() == ARCHIVE_END) {
                    archive_end = true;
                    break;
                }
                ++file_size;
                bit_writer.Write(trie.TraceValue(), FILE_FIXED_CHAR_SIZE);
                trie.ResetTrace();
            }
        }

        // Give a command to decompressor to finish decompressing or continue with next file

        if (!one_more_file && !archive_end) {
            return throw ArchiveDamagedError("Can't get information about next file or archive is end");
        }

        bit_writer.Complete();

        // Save decompress file data to show information at finish
        files_.push_back(File(file_name, file_size));

        if (archive_end) {
            break;
        }
    }
}

// Get files data
std::vector<File> Decompressor::GetFiles() const {
    return files_;
}

// Exceptions

// Constructor of ArchiveDamagedError
Decompressor::ArchiveDamagedError::ArchiveDamagedError(const std::string& description) {
    std::string full_description = "Archive is damaged, can't decompress it (" + description + ").";
    description_ = new char[full_description.size()];
    std::strcpy(description_, full_description.c_str());
}

// Return more information about ArchiveDamagedError exception
const char* Decompressor::ArchiveDamagedError::what() const noexcept {
    return description_;
}