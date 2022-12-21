#pragma once

#include <cstdint>

// Char type for storing symbols
using CharT = int16_t;

static const CharT FILENAME_END = 0b100000000;
static const CharT ONE_MORE_FILE = 0b100000001;
static const CharT ARCHIVE_END = 0b100000010;

static const CharT MAX_CHAR_VALUE = 258;

// Archive format fixed char size for compressing and decompressing
static const size_t ARCHIVE_FIXED_CHAR_SIZE = 9;
static const size_t FILE_FIXED_CHAR_SIZE = 8;
