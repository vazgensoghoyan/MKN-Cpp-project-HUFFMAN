#include "archivator_input_parser.hpp"

namespace parser {

ArchivatorInputParser::ArchivatorInputParser(int argc, char** argv){
    parse_arguments(argc, argv);
}

ArchivatorInputParser::~ArchivatorInputParser() = default;

void ArchivatorInputParser::parse_arguments(int argc, char** argv){
    if (argc != NEEDED_ARGC)
        throw huffman::HuffmanException("argc count must be " + std::to_string(NEEDED_ARGC));

    for (int i = 1; i < NEEDED_ARGC; ++i) {
        if ( std::strcmp(argv[i], "-c") == 0 )
            compression_status = true;
        else if ( std::strcmp(argv[i], "-u") == 0 )
            compression_status = false;
        else if ( std::strcmp(argv[i], "-f") == 0 || std::strcmp(argv[i], "--file") == 0 )
            input_file = argv[++i];
        else if ( std::strcmp(argv[i], "-o") == 0 || std::strcmp(argv[i], "--output") == 0 )
            output_file = argv[++i];
    }
}

template<typename Alg>
void ArchivatorInputParser::run_command() {
    static_assert(std::is_base_of<huffman::IArchivatorAlgorithm, Alg>::value,
                  "Alg must be a descendant of ArchivatorAlgorithm");

    Alg alg(input_file, output_file);
    huffman::ArchiveInfo stats{0, 0, 0};

    if (compression_status) {
        stats = alg.compress();

        std::cout << stats.original_size << std::endl;
        std::cout << stats.compressed_size << std::endl;
        std::cout << stats.extra_size << std::endl;
    }
    else {
        stats = alg.decompress();
        
        std::cout << stats.compressed_size << std::endl;
        std::cout << stats.original_size << std::endl;
        std::cout << stats.extra_size << std::endl;
    }
}

template void ArchivatorInputParser::run_command<huffman::HuffmanArchive>();

std::string ArchivatorInputParser::get_input_file() const {
    return input_file;
}

std::string ArchivatorInputParser::get_output_file() const {
    return output_file;
}

bool ArchivatorInputParser::get_compression_status_file() const {
    return compression_status;
}

} // namespace parser