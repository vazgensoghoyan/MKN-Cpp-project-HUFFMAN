#include "huffman.hpp"
#include <iostream>
#include <fstream>

void compress(std::string input, std::string output) {
    std::ifstream input_(input);
    std::ofstream output_(input);

    huffman::HuffmanArchive::compress(input_, output_);
}

void decompress(std::string input, std::string output) {
    std::ifstream input_(input);
    std::ofstream output_(input);

    huffman::HuffmanArchive::decompress(input_, output_);
}

int main(int argc, char **argv) {
    int NEEDED_ARGC = 6;

    if (argc != NEEDED_ARGC)
        return 1;

    bool COMPRESS = true;
    std::string INPUT;
    std::string OUTPUT;

    for (int i = 1; i < NEEDED_ARGC; ++i) {
        if (argv[i] == "-c")
            COMPRESS = true;
        else if (argv[i] == "-u")
            COMPRESS = false;
        else if (argv[i] == "-f" || argv[i] == "--file")
            INPUT = argv[++i];
        else if (argv[i] == "-o" || argv[i] == "--output")
            INPUT = argv[++i];
        else
            return 1;
    }

    if (COMPRESS)
        compress(INPUT, OUTPUT);
    else
        decompress(INPUT, OUTPUT);

    return 0;
}