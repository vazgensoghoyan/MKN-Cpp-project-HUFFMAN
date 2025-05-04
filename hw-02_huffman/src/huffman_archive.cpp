#include "huffman.hpp"
#include "huffman_archive.hpp"

namespace huffman {

namespace {

uint8_t convert_string_to_byte(const std::string &str) {
    uint8_t result = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] != '0' && str[i] != '1')
            throw HuffmanException("String in convert_string_to_byte must be from 0 and 1");

        result |= (str[i] - '0') << (7 - i);
    }
    return result;
}

} // anonymous namespace

// HuffmanArchive helper methods

HuffmanArchive::HuffmanArchive(std::string& input, std::string& output) : IArchivatorAlgorithm(input, output) {}

void HuffmanArchive::open_streams() {
    input_.open(input_path_, std::ios::binary);
    if (!input_)
        throw HuffmanException("Failed to open input stream");

    output_.open(output_path_, std::ios::binary);
    if (!output_)
        throw HuffmanException("Failed to open output stream");
}

template<typename T>
size_t HuffmanArchive::read_from_file(T& data) {
    input_.read(reinterpret_cast<char*>(&data), sizeof(T));
    if (!input_)
        throw HuffmanException("Failed to read from file");
    return sizeof(T);
}

template<typename T>
size_t HuffmanArchive::write_to_file(T& data) {
    output_.write(reinterpret_cast<const char*>(&data), sizeof(T));
    if (!output_)
        throw HuffmanException("Failed to write in file");
    return sizeof(T);
}

ArchiveInfo HuffmanArchive::compress() {
    open_streams();

    // Meta data
    std::string buffer;
    std::map<uint8_t, size_t> freqMap;
    char c;
    while (input_.get(c)) {
        freqMap[static_cast<uint8_t>(c)]++;
        buffer += c;
    }

    HuffmanTree huffmanTree(freqMap);
    auto codes = huffmanTree.get_codes();

    ArchiveInfo stats {
        buffer.size(),                              // original size
        0,                                          // yet no compressed size
        write_meta(buffer.size(), codes)            // extra size
    };

    // Useful data

    std::string cur_byte;
    cur_byte.reserve(8);
    for (size_t i = 0; i < buffer.size(); ++i) {
        auto new_byte = buffer[i];
        auto value = codes[static_cast<uint8_t>(new_byte)];

        size_t cur_index = 0;
        while (cur_index < value.size()) {
            size_t prev_byte_size = std::min(value.size() - cur_index, 8 - cur_byte.size());

            cur_byte += value.substr(cur_index, prev_byte_size);
            cur_index += prev_byte_size;

            if (cur_byte.size() < 8) break;

            uint8_t byte = convert_string_to_byte(cur_byte);
            stats.compressed_size += write_to_file(byte);
            cur_byte = "";
        }
    }

    if (!cur_byte.empty()) {
        cur_byte.insert(cur_byte.size(), 8 - cur_byte.size(), '0');
        uint8_t byte = convert_string_to_byte(cur_byte);
        stats.compressed_size += write_to_file(byte);
    }

    return stats;
}

ArchiveInfo HuffmanArchive::decompress() {
    open_streams();
    
    std::map<std::string, uint8_t> symbols;
    ArchiveInfo stats{0, 0, 0};

    size_t result_file_size = 0;
    stats.extra_size += read_from_file(result_file_size);

    size_t codes_count = 0;
    stats.extra_size += read_from_file(codes_count);

    for (size_t i = 0; i < codes_count; ++i) {
        uint8_t byte;
        stats.extra_size += read_from_file(byte);
        size_t value_size;
        stats.extra_size += read_from_file(value_size);

        std::string value = "";
        for (size_t i = 0; i < value_size; ++i)
            value += input_.get();

        symbols.emplace(value, byte);
        stats.extra_size += value_size;
    }

    char cur_byte;
    std::string cur_value;

    while (input_.get(cur_byte)) {
        stats.compressed_size++;

        for (int i = 7; i >= 0; --i) {
            bool cur_bit = (1 << i) & cur_byte;
            cur_value += ('0' + cur_bit);

            if (symbols.find(cur_value) != symbols.end() && stats.original_size < result_file_size) {
                stats.original_size += write_to_file(symbols[cur_value]);
                cur_value = "";
            }
        }
    }

    if (result_file_size != stats.original_size || cur_value.size() >= 8 || convert_string_to_byte(cur_value) != 0)
        throw HuffmanException("Incorrect file format");

    return stats;
}

size_t HuffmanArchive::write_meta(size_t bytes_count, std::map<uint8_t, std::string> &codes) {
    size_t extra_size = 0;

    extra_size += write_to_file(bytes_count);
   
    size_t codes_size = codes.size();
    extra_size += write_to_file(codes_size);

    for (auto& pair : codes) {
        extra_size += write_to_file(pair.first);
        
        size_t len = pair.second.size();
        extra_size += write_to_file(len);

        output_.write(pair.second.data(), len);
        extra_size += len;
    }

    return extra_size;
}

} // namespace huffman