#include "huffman.hpp"
#include "huffman_archive.hpp"

// needed

uint8_t convert_string_to_byte(const std::string &str) {
    uint8_t result = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] != '0' && str[i] != '1')
            throw huffman::HuffmanException("String in convert_string_to_byte must be from 0 and 1");

        result |= (str[i] - '0') << (7 - i);
    }
    return result;
}

// HuffmanArchive


size_t huffman::HuffmanArchive::write_to_file(std::ofstream& ofs, const char* data, size_t size) {
    ofs.write(data, (long)size);
    if (!ofs)
        throw huffman::HuffmanException("Failed to write in file");
    return size;
}

huffman::ArchiveInfo huffman::HuffmanArchive::compress(std::string& input, std::string& output) {
    // Opening files
    std::ifstream ifs(input, std::ios::binary);
    if (!ifs.is_open())
        throw huffman::HuffmanException("Input file is not opened!");
    
    std::ofstream ofs(output, std::ios::binary);
    if (!ofs.is_open())
        throw huffman::HuffmanException("Output file is not opened!");

    ArchiveInfo stats{0, 0, 0};
    
    // Meta data
    std::string buffer = "";
    std::vector<size_t> freqMap(256, 0);
    char c;
    while (ifs.get(c)) {
        freqMap[static_cast<size_t>(c)]++;
        stats.original_size += 1;
        buffer += c;
    }

    HuffmanTree huffmanTree(freqMap);
    auto codes = huffmanTree.get_codes();

    stats.extra_size = write_meta(ofs, buffer.size(), codes);

    // Useful data

    std::string cur_byte;
    cur_byte.reserve(8);
    for (size_t i = 0; i < buffer.size(); ++i) {
        auto new_byte = buffer[i];
        auto value = codes[new_byte];

        size_t cur_index = 0;
        while (cur_index < value.size()) {
            size_t size_for_previous_byte = std::min(value.size() - cur_index, 8 - cur_byte.size());

            cur_byte += value.substr(cur_index, size_for_previous_byte);
            cur_index += size_for_previous_byte;

            if (cur_byte.size() < 8) break;

            auto byte = convert_string_to_byte(cur_byte);
            stats.compressed_size += write_to_file(ofs, reinterpret_cast<const char *>(&byte), sizeof(uint8_t));
            cur_byte = "";
        }
    }

    if (!cur_byte.empty()) {
        cur_byte.insert(cur_byte.size(), 8 - cur_byte.size(), '0');
        auto byte = convert_string_to_byte(cur_byte);
        stats.compressed_size += write_to_file(ofs, reinterpret_cast<const char*>(&byte), sizeof(uint8_t));
    }

    return stats;
}

huffman::ArchiveInfo huffman::HuffmanArchive::decompress(std::string& input, std::string& output) {
    ArchiveInfo stats{0, 0, 0};

    std::ifstream ifs(input, std::ios::binary);
    if (!ifs.is_open())
        throw huffman::HuffmanException("Input file is not opened!");
    
    std::ofstream ofs(output, std::ios::binary);
    if (!ofs.is_open())
        throw huffman::HuffmanException("Output file is not opened!");

    return stats;
}

size_t huffman::HuffmanArchive::write_meta(std::ofstream& ofs, size_t bytes_count, std::map<uint8_t, std::string> &codes) {
    size_t extra_size = 0;

    extra_size += write_to_file(ofs, reinterpret_cast<const char*>(&bytes_count), sizeof(size_t));
    size_t codes_size = codes.size();
    extra_size += write_to_file(ofs, reinterpret_cast<const char*>(&codes_size), sizeof(size_t));

    for (auto& pair : codes) {
        extra_size += write_to_file(ofs, reinterpret_cast<const char*>(&pair.first), sizeof(uint8_t));
        size_t len = pair.second.size();
        extra_size += write_to_file(ofs, reinterpret_cast<const char*>(&len), sizeof(size_t));
        extra_size += write_to_file(ofs, pair.second.data(), len);
    }

    return extra_size;
}