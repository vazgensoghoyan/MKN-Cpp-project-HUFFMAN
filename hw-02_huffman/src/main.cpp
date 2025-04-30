#include "huffman.hpp"
#include "huffman_archive.hpp"
#include <iostream>
#include <fstream>
#include <cstring>


int main(int argc, char **argv) {
    int NEEDED_ARGC = 6;

    if (argc != NEEDED_ARGC)
        return 1;

    bool COMPRESS = true;
    std::string INPUT;
    std::string OUTPUT;

    for (int i = 1; i < NEEDED_ARGC; ++i) {
        if ( std::strcmp(argv[i], "-c") == 0 )
            COMPRESS = true;
        else if ( std::strcmp(argv[i], "-u") == 0 )
            COMPRESS = false;
        else if ( std::strcmp(argv[i], "-f") == 0 || std::strcmp(argv[i], "--file") == 0 )
            INPUT = argv[++i];
        else if ( std::strcmp(argv[i], "-o") == 0 || std::strcmp(argv[i], "--output") == 0 )
            OUTPUT = argv[++i];
        else
            return 1;
    }

    huffman::ArchiveInfo stats{0, 0, 0};

    if (COMPRESS)
        stats = huffman::HuffmanArchive::compress(INPUT, OUTPUT);
    else
        stats = huffman::HuffmanArchive::decompress(INPUT, OUTPUT);

    std::cout << stats.original_size << std::endl;
    std::cout << stats.compressed_size << std::endl;
    std::cout << stats.extra_size << std::endl;

    return 0;
}