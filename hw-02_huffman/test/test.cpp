#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "huffman.hpp"
#include "huffman_archive.hpp"
#include <map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace huffman;
namespace fs = std::filesystem;

void create_test_file(const std::string& filename, const std::string& content) {
    std::ofstream out(filename, std::ios::binary);
    out.write(content.data(), content.size());
}

std::map<uint8_t, size_t> createFrequencyMap(const std::string& input) {
    std::map<uint8_t, size_t> freqMap;
    for (char c : input) {
        freqMap[static_cast<uint8_t>(c)]++;
    }
    return freqMap;
}


TEST_SUITE("HuffmanTree") {

    TEST_CASE("HuffmanTree construction") {
        
        SUBCASE("Empty frequency map") {
            std::map<uint8_t, size_t> freqMap;
            HuffmanTree tree(freqMap);
            
            CHECK(tree.get_codes().empty());
        }
        
        SUBCASE("Single symbol frequency map") {
            std::map<uint8_t, size_t> freqMap = {{'A', 1}};
            HuffmanTree tree(freqMap);
            auto codes = tree.get_codes();
            
            CHECK(codes.size() == 1);
            CHECK(codes.at('A') == "0");
        }
        
        SUBCASE("Multiple symbols frequency map") {
            std::map<uint8_t, size_t> freqMap = {
                {'A', 5},
                {'B', 9},
                {'C', 12},
                {'D', 13},
                {'E', 16},
                {'F', 45}
            };
            HuffmanTree tree(freqMap);
            auto codes = tree.get_codes();
            
            CHECK(codes.size() == 6);

            CHECK(codes.at('F').length() < codes.at('A').length());
            CHECK(codes.at('E').length() < codes.at('B').length());
        }
    }
    
    TEST_CASE("HuffmanTree code generation") {

        SUBCASE("Two symbols with equal frequency") {
            std::map<uint8_t, size_t> freqMap = {
                {'A', 1},
                {'B', 1}
            };
            HuffmanTree tree(freqMap);
            auto codes = tree.get_codes();
            
            CHECK(codes.size() == 2);
            
            CHECK(codes.at('A') != codes.at('B'));

            bool first = (codes.at('A')[0] == '1' && codes.at('B')[0] == '0');
            bool second = (codes.at('A')[0] == '0' && codes.at('B')[0] == '1');
            CHECK((first || second));
        }
        
        SUBCASE("Three symbols with different frequencies") {
            std::map<uint8_t, size_t> freqMap = {
                {'A', 3},
                {'B', 2},
                {'C', 1}
            };
            HuffmanTree tree(freqMap);
            auto codes = tree.get_codes();
            
            CHECK(codes.size() == 3);
            
            size_t a_len = codes.at('A').length();
            size_t b_len = codes.at('B').length();
            size_t c_len = codes.at('C').length();

            CHECK(a_len <= b_len);
            CHECK(b_len <= c_len);
        }
    }
    
    TEST_CASE("HuffmanTree edge cases") {
        
        SUBCASE("All symbols have same frequency") {
            std::map<uint8_t, size_t> freqMap = {
                {'A', 1},
                {'B', 1},
                {'C', 1},
                {'D', 1}
            };
            HuffmanTree tree(freqMap);
            auto codes = tree.get_codes();
            
            CHECK(codes.size() == 4);
            
            size_t code_length = codes.begin()->second.length();
            for (const auto& pair : codes) {
                CHECK(pair.second.length() == code_length);
            }
        }
        
        SUBCASE("Large frequency differences") {
            std::map<uint8_t, size_t> freqMap = {
                {'A', 1000},
                {'B', 1},
                {'C', 1},
                {'D', 1}
            };
            HuffmanTree tree(freqMap);
            auto codes = tree.get_codes();
            
            CHECK(codes.size() == 4);
            
            size_t a_len = codes.at('A').length();
            for (const auto& pair : codes) {
                if (pair.first != 'A') {
                    CHECK(pair.second.length() > a_len);
                }
            }
        }
    }
}


TEST_SUITE("Support functions") {
    TEST_CASE("convert_string_to_byte") {
        /*SUBCASE("Valid input") {
            CHECK((convert_string_to_byte("00000000") == 0));
            CHECK((convert_string_to_byte("11111111") == 0b11111111));
            CHECK(convert_string_to_byte("10101010") == 0b10101010);
            CHECK(convert_string_to_byte("00001111") == 0b00001111);
            CHECK(convert_string_to_byte("1") == 0b10000000);
            CHECK(convert_string_to_byte("01") == 0b01000000);
        }
        
        SUBCASE("Invalid input throws exception") {
            CHECK_THROWS_AS(convert_string_to_byte("01234567"), HuffmanException);
            CHECK_THROWS_AS(convert_string_to_byte("abc"), HuffmanException);
            CHECK_THROWS_AS(convert_string_to_byte("111111111"), HuffmanException);
        }*/
    }
    
    TEST_CASE("write_to_file") {
        /*std::ofstream ofs("test.bin", std::ios::binary);
        REQUIRE(ofs.is_open());
        
        SUBCASE("Writing integers") {
            int test_int = 42;
            size_t bytes_written = HuffmanArchive::write_to_file(ofs, test_int);
            CHECK(bytes_written == sizeof(int));
        }
        
        SUBCASE("Writing chars") {
            char test_char = 'A';
            size_t bytes_written = HuffmanArchive::write_to_file(ofs, test_char);
            CHECK(bytes_written == sizeof(char));
        }
        
        ofs.close();
        fs::remove("test.bin");*/
    }

    TEST_CASE("write_meta") {
        std::string filename = "meta_test.bin";

        std::ofstream ofs(filename, std::ios::binary);
        REQUIRE(ofs.is_open());
        
        std::map<uint8_t, std::string> codes = {
            {'A', "0"},
            {'B', "10"},
            {'C', "11"}
        };
        size_t bytes_count = 100;
        
        size_t extra_size = HuffmanArchive::write_meta(ofs, bytes_count, codes);
        
        size_t expected_size = 2 * sizeof(size_t) + 
                               (sizeof(uint8_t) + sizeof(size_t) + 1) + 
                               (sizeof(uint8_t) + sizeof(size_t) + 2) + 
                               (sizeof(uint8_t) + sizeof(size_t) + 2);
                               
        CHECK(extra_size == expected_size);
        
        ofs.close();
        fs::remove(filename);
    }
}

TEST_SUITE("Test archivator") {
    TEST_CASE("compress and decompress") {
        SUBCASE("Empty file") {
            std::ofstream ofs("empty.txt", std::ios::binary);
            ofs.close();
            
            std::string input = "empty.txt";
            std::string compressed = "empty_compressed.bin";
            std::string decompressed = "empty_decompressed.txt";
            
            HuffmanArchive archive;
            auto compress_stats = archive.compress(input, compressed);
            
            CHECK(compress_stats.original_size == 0);
            CHECK(compress_stats.compressed_size == 0);
            CHECK(compress_stats.extra_size > 0);
            
            auto decompress_stats = archive.decompress(compressed, decompressed);
            
            CHECK(decompress_stats.original_size == compress_stats.original_size);
            CHECK(decompress_stats.compressed_size == compress_stats.compressed_size);
            CHECK(decompress_stats.extra_size == compress_stats.extra_size);
            
            std::ifstream ifs(decompressed, std::ios::binary);
            ifs.seekg(0, std::ios::end);
            CHECK(ifs.tellg() == 0);
            
            fs::remove(input);
            fs::remove(compressed);
            fs::remove(decompressed);
        }
        
        SUBCASE("Single character file") {
            std::string input = "single.txt";
            create_test_file(input, "A");

            std::string compressed = "single_compressed.bin";
            std::string decompressed = "single_decompressed.txt";
            
            HuffmanArchive archive;
            auto compress_stats = archive.compress(input, compressed);
            
            CHECK(compress_stats.original_size == 1);
            
            auto decompress_stats = archive.decompress(compressed, decompressed);
            
            CHECK(decompress_stats.original_size == 1);
            
            std::ifstream ifs(decompressed, std::ios::binary);
            char c;
            ifs.get(c);
            CHECK(c == 'A');
            
            fs::remove(input);
            fs::remove(compressed);
            fs::remove(decompressed);
        }
        
        SUBCASE("Multiple characters file") {
            std::string test_content = "ABRACADABRA";
            std::string input = "multi.txt";
            create_test_file(input, test_content);
            
            std::string compressed = "multi_compressed.bin";
            std::string decompressed = "multi_decompressed.txt";
            
            HuffmanArchive archive;
            auto compress_stats = archive.compress(input, compressed);
            
            CHECK(compress_stats.original_size == test_content.size());
            
            auto decompress_stats = archive.decompress(compressed, decompressed);
            
            CHECK(decompress_stats.original_size == test_content.size());
            
            std::ifstream ifs(decompressed, std::ios::binary);
            std::stringstream buffer;
            buffer << ifs.rdbuf();
            CHECK(buffer.str() == test_content);
            
            fs::remove(input);
            fs::remove(compressed);
            fs::remove(decompressed);
        }
    }
    
    /*TEST_CASE("error handling") {
        HuffmanArchive archive;
        
        SUBCASE("Non-existent input file for compression") {
            std::string input = "nonexistent.txt";
            std::string output = "output.bin";
            
            CHECK_THROWS_AS(archive.compress(input, output), HuffmanException);
        }
        
        SUBCASE("Non-existent input file for decompression") {
            std::string input = "nonexistent.bin";
            std::string output = "output.txt";
            
            CHECK_THROWS_AS(archive.decompress(input, output), HuffmanException);
        }
        
        SUBCASE("Invalid compressed file format") {
            std::ofstream ofs("invalid.bin", std::ios::binary);
            ofs << "This is not a valid compressed file";
            ofs.close();
            
            std::string input = "invalid.bin";
            std::string output = "output.txt";
            
            CHECK_THROWS_AS(archive.decompress(input, output), HuffmanException);
            
            fs::remove("invalid.bin");
        }
    }*/
}