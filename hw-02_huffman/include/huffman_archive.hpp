#ifndef HUFFMAN_ARCHIVE_H_
#define HUFFMAN_ARCHIVE_H_

#include "huffman.hpp"
#include "huffman_exception.hpp"
#include <cstddef>
#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>

namespace huffman {

struct ArchiveInfo {
    size_t original_size;
    size_t compressed_size;
    size_t extra_size;

    ArchiveInfo(size_t os, size_t cs, size_t es)
        : original_size(os), compressed_size(cs), extra_size(es) {}
};

class IArchivatorAlgorithm {
public:
    IArchivatorAlgorithm(std::string input, std::string output) : input_path_(input), output_path_(output) {}
    virtual ~IArchivatorAlgorithm() = default;

    virtual ArchiveInfo compress() = 0;
    virtual ArchiveInfo decompress() = 0;

protected:
    std::string input_path_;
    std::string output_path_;
};

class HuffmanArchive : public IArchivatorAlgorithm {
public:
    HuffmanArchive(std::string& input, std::string& output);

    virtual ArchiveInfo compress() override;
    virtual ArchiveInfo decompress() override;

private:
    void open_streams();
    void close_streams();

    template<typename T>
    size_t read_from_file(T& data);
    template<typename T>
    size_t write_to_file(T& data);

    size_t write_meta(size_t bytes_count, std::map<uint8_t, std::string>& codes);
    size_t read_meta(size_t& result_file_size, std::map<std::string, uint8_t>& symbols);
    
    size_t write_compressed_data(std::string& buffer, std::map<uint8_t, std::string>& codes);
    size_t read_compressed_data(size_t expected_orig_size, std::map<std::string, uint8_t>& symbols);

private:
    std::ifstream input_stream_;
    std::ofstream output_stream_;
};

} // namespace huffman

#endif  // HUFFMAN_ARCHIVE_H_