add_subdirectory(src)
add_subdirectory(tests)
add_catch(unit_test_archiver test.cpp src/compressor.cpp src/decompressor.cpp src/long_code.cpp src/utils/bit_reader.cpp src/utils/bit_writer.cpp src/utils/file.cpp src/utils/parser.cpp src/utils/weight.cpp)
