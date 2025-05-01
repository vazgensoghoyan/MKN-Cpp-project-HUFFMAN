#include "huffman_archive.hpp"
#include "archivator_input_parser.hpp"
#include <iostream>
#include <fstream>
#include <cstring>


int main(int argc, char **argv) {
    
    parser::ArchivatorInputParser prsr(argc, argv);

    prsr.run_command<huffman::HuffmanArchive>();

    return 0;
}