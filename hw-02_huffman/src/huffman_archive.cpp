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
    input_stream_.open(input_path_, std::ios::binary);
    if (!input_stream_)
        throw HuffmanException("Failed to open input stream");

    output_stream_.open(output_path_, std::ios::binary);
    if (!output_stream_)
        throw HuffmanException("Failed to open output stream");
}

void HuffmanArchive::close_streams() {
    input_stream_.close();
    output_stream_.close();
}

template<typename T>
size_t HuffmanArchive::read_from_file(T& data) {
    input_stream_.read(reinterpret_cast<char*>(&data), sizeof(T));
    if (!input_stream_)
        throw HuffmanException("Failed to read from file");
    return sizeof(T);
}

template<typename T>
size_t HuffmanArchive::write_to_file(T& data) {
    output_stream_.write(reinterpret_cast<const char*>(&data), sizeof(T));
    if (!output_stream_)
        throw HuffmanException("Failed to write in file");
    return sizeof(T);
}

ArchiveInfo HuffmanArchive::compress() {
    open_streams();

    std::string buffer;
    std::map<uint8_t, size_t> freq_map;
    char c;
    while (input_stream_.get(c)) {
        freq_map[static_cast<uint8_t>(c)]++;
        buffer += c;
    }

    HuffmanTree huffmanTree(freq_map);
    auto codes = huffmanTree.get_codes();

    ArchiveInfo stats{0, 0, 0};

    stats.original_size = buffer.size();
    stats.extra_size = write_meta(buffer.size(), codes);
    stats.compressed_size = write_compressed_data(buffer, codes);

    close_streams();

    return stats;
}

ArchiveInfo HuffmanArchive::decompress() {
    open_streams();
    
    std::map<std::string, uint8_t> symbols;
    size_t orig_size_from_meta;

    ArchiveInfo stats{0, 0, 0};

    stats.extra_size = read_meta(orig_size_from_meta, symbols);
    stats.original_size = orig_size_from_meta;
    stats.compressed_size = read_compressed_data(orig_size_from_meta, symbols);

    close_streams();

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

        output_stream_.write(pair.second.data(), len);
        extra_size += len;
    }

    return extra_size;
}

size_t HuffmanArchive::read_meta(size_t& result_file_size, std::map<std::string, uint8_t> &symbols) {
    size_t extra_size = 0;
    
    extra_size += read_from_file(result_file_size);

    size_t codes_count = 0;
    extra_size += read_from_file(codes_count);

    for (size_t i = 0; i < codes_count; ++i) {
        uint8_t byte;
        extra_size += read_from_file(byte);
        size_t value_size;
        extra_size += read_from_file(value_size);

        std::string value = "";
        for (size_t i = 0; i < value_size; ++i)
            value += input_stream_.get();

        symbols.emplace(value, byte);
        extra_size += value_size;
    }

    return extra_size;
}

size_t HuffmanArchive::write_compressed_data(std::string& buffer, std::map<uint8_t, std::string>& codes) {
    size_t compressed_size = 0;
    std::bitset<8> current_byte_bits;
    size_t bits_written = 0;

    for (const char c : buffer) {
        std::string& code = codes.at(static_cast<uint8_t>(c));

        for (const char bit : code) {
            if (bit == '1')
                current_byte_bits.set(7 - bits_written);

            if (++bits_written == 8) {
                // Convert and write full byte
                const uint8_t byte = static_cast<uint8_t>(current_byte_bits.to_ulong());
                compressed_size += write_to_file(byte);
                
                // Reset for next byte
                current_byte_bits.reset();
                bits_written = 0;
            }
        }
    }

    // Write remaining bits if any
    if (bits_written > 0) {
        const uint8_t byte = static_cast<uint8_t>(current_byte_bits.to_ulong());
        compressed_size += write_to_file(byte);
    }

    return compressed_size;
}

size_t HuffmanArchive::read_compressed_data(size_t expected_orig_size, std::map<std::string, uint8_t>& symbols) {
    size_t compressed_size = 0;
    char cur_byte;
    std::string cur_code;

    while (input_stream_.get(cur_byte)) {
        compressed_size++;

        for (int i = 7; i >= 0; --i) {
            bool cur_bit = (1 << i) & cur_byte;
            cur_code += (cur_bit ? '1' : '0');

            if (symbols.find(cur_code) != symbols.end() && expected_orig_size > 0) {
                expected_orig_size -= write_to_file(symbols[cur_code]);
                cur_code = "";
            }
        }
    }

    if (expected_orig_size != 0) {
        throw HuffmanException("Decompressed size doesn't match expected size from meta");
    }

    if (cur_code.size() >= 8 && convert_string_to_byte(cur_code) != 0) {
        throw HuffmanException("Trailing bits are not zero-padded correctly");
    }

    return compressed_size;
}

} // namespace huffman