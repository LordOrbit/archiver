#include <fstream>
#include <iostream>

#include "compressor.h"
#include "decompressor.h"
#include "utils/parser.h"
#include "utils/round.h"
#include "utils/timer.h"

const int ERROR_CODE = 111;

inline int Program(const Parser& parser) {
    // User didn't write any arguments
    if (!parser.HasArgument("compress") && !parser.HasArgument("decompress") && !parser.HasArgument("help")) {
        std::cerr << "Provide one of the archiver modes." << std::endl;
        std::cerr << "For more information type:" << std::endl;
        std::cerr << "archiver -h (--help)" << std::endl;
        return ERROR_CODE;
    }

    // User wrote -c (--compress)
    if (parser.HasArgument("compress") && !parser.HasArgument("decompress") && !parser.HasArgument("help")) {
        if (parser["compress"].Size() >= 2) {
            std::vector<std::string> files = parser["compress"].SubArray(1);

            Timer clock;
            Compressor compressor(files, parser["compress"].First());

            std::cerr << "Compressing started" << std::endl;

            try {
                clock.Tick();
                compressor.Compress();
                clock.Tock();
            } catch (...) {
                std::cerr << std::endl << "Error occur while compressing:" << std::endl;
                throw;
            }

            if (parser["compress"].Size() >= 3) {
                std::cerr << "Files compressed successfully in " << clock.Duration().count() << "ms." << std::endl;
            } else {
                std::cerr << "File compressed successfully in " << clock.Duration().count() << "ms." << std::endl;
            }

            long double compress_percents = -Round((1 - compressor.ResultWeight() / compressor.RawWeight()) * 100, 2);

            std::cerr << "Archive saved at \"" << compressor.archive_path
                      << "\" with total space: " << compressor.ResultWeight() << " (" << compress_percents << "%)."
                      << std::endl;
            return 0;
        } else {
            std::cerr << "After -c, please, provide archive name and file paths separated by a space." << std::endl;
            std::cerr << "For more information type:" << std::endl;
            std::cerr << "archiver -h (--help)" << std::endl;
            return ERROR_CODE;
        }
    }
    // User wrote -d (--decompress)
    else if (!parser.HasArgument("compress") && parser.HasArgument("decompress") && !parser.HasArgument("help")) {
        if (parser["decompress"].Size() == 1) {
            std::string archive_path = parser["decompress"].First();

            Timer clock;
            Decompressor decompressor(archive_path);

            std::cerr << "Decompressing started" << std::endl;

            try {
                clock.Tick();
                decompressor.Decompress();
                clock.Tock();
            } catch (...) {
                std::cerr << std::endl << "Error occur while decompressing:" << std::endl;
                throw;
            }

            if (decompressor.GetFiles().size() >= 2) {
                std::cerr << "Files decompressed successfully in " << clock.Duration().count() << "ms." << std::endl;
                std::cerr << "Decompressed files:" << std::endl;
                for (const auto& file : decompressor.GetFiles()) {
                    std::cerr << "  - " << file.GetPath() << " (" << file.GetWeight() << ")" << std::endl;
                }
            } else {
                auto file = decompressor.GetFiles().front();
                std::cerr << "File \"" << file.GetPath() << "\" (" << file.GetWeight() << ") ";
                std::cerr << "decompressed successfully in " << clock.Duration().count() << "ms." << std::endl;
            }
            return 0;
        } else {
            if (parser["decompress"].Empty()) {
                std::cerr << "After -d, please, provide archive name." << std::endl;
            } else {
                std::cerr << "After -d, please, provide only archive name." << std::endl;
            }
            std::cerr << "For more information type:" << std::endl;
            std::cerr << "archiver -h (--help)" << std::endl;
            return ERROR_CODE;
        }
    }
    // User wrote -h (--help)
    else if (!parser.HasArgument("compress") && !parser.HasArgument("decompress") && parser.HasArgument("help")) {
        std::cerr << "Help message:" << std::endl;
        std::cerr << "Type \"archiver -c archive_path file1 [file2 ...]\" to compress one or multiple files and save "
                     "compressed archive as archive_path"
                  << std::endl;
        std::cerr << "Type \"archiver -d archive_path\" to decompress archive" << std::endl;
        return 0;
    }
    // User wrote multiple argument
    else {
        std::cerr << "Provide exactly one of the archiver modes." << std::endl;
        std::cerr << "For more information type:" << std::endl;
        std::cerr << "archiver -h (--help)" << std::endl;
        return ERROR_CODE;
    }
}

int main(int argc, char** argv) {
    try {
        // Setup parser arguments for archiver program
        Parser parser(argc, argv, {{'c', "compress"}, {'d', "decompress"}, {'h', "help"}},
                      {"compress", "decompress", "help"});

        return Program(parser);
    }
    // Catching std::exception class errors
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << std::endl << "For more information type:" << std::endl;
        std::cerr << "archiver -h (--help)" << std::endl;
        return ERROR_CODE;
    }
    // Catching other errors
    catch (...) {
        std::cerr << "Something went wrong, please, try again." << std::endl;
        std::cerr << std::endl << "For more information type:" << std::endl;
        std::cerr << "archiver -h (--help)" << std::endl;
        return ERROR_CODE;
    }
}
