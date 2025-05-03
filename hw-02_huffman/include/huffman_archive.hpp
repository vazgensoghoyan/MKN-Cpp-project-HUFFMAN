#ifndef HUFFMAN_ARCHIVE_H_
#define HUFFMAN_ARCHIVE_H_

#include <cstddef>
#include <memory>
#include <vector>
#include <map>
#include <stdexcept>
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


// TODO interface
// yet is not used
class IArchivatorAlgorithm {
public:
    IArchivatorAlgorithm(std::string& input, std::string& output) : input_(input), output_(output) {}
    virtual ~IArchivatorAlgorithm() = default;

    virtual ArchiveInfo compress() = 0;
    virtual ArchiveInfo decompress() = 0;

protected:
    std::string input_;
    std::string output_;
};

class HuffmanArchive : public IArchivatorAlgorithm {
public:
    HuffmanArchive(std::string& input, std::string& output) : IArchivatorAlgorithm(input, output) {}

    virtual ArchiveInfo compress() override;
    virtual ArchiveInfo decompress() override;

private:
    template<typename T>
    static size_t write_to_file(std::ofstream& ofs, T& data);

    static size_t write_meta(std::ofstream& ofs, size_t bytes_count, std::map<uint8_t, std::string> &codes);

    friend class HuffmanArchiveTest;
};

} // namespace huffman

#endif  // HUFFMAN_ARCHIVE_H_