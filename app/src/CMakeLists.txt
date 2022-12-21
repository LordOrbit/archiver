add_executable(
        archiver
        archiver.cpp
        utils/parser.cpp utils/file.cpp utils/weight.cpp compressor.cpp decompressor.cpp utils/bit_reader.cpp utils/bit_writer.cpp long_code.cpp)
