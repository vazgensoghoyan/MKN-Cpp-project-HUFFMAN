#include "huffman_archive.hpp"
#include "archivator_input_parser.hpp"

int main(int argc, char **argv) {
    
    try {
        parser::ArchivatorInputParser prsr(argc, argv);

        prsr.run_command<huffman::HuffmanArchive>();

    } catch (huffman::HuffmanException& exc) {
        
        std::cout << exc.what() << std::endl;

        return 1;
    }

    return 0;
}