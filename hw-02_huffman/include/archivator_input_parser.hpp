#ifndef ARCHIVATOR_INPUT_PARSER_H_
#define ARCHIVATOR_INPUT_PARSER_H_

#include "huffman_archive.hpp"
#include <string>
#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace parser {

class ArchivatorInputParser {
public:
    ArchivatorInputParser(int argc, char** argv);
    ~ArchivatorInputParser();

    void run_command();

    std::string get_input_file() const;
    std::string get_output_file() const;
    bool get_compression_status_file() const;

private:
    void parse_arguments(int argc, char** argv);

private:
    const int NEEDED_ARGC = 6;

    std::string input_file;
    std::string output_file;
    bool compression_status;
};

} // namespace parser

#endif