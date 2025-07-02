#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "huffman.hpp"
#include "huffman_archive.hpp"
#include <doctest/doctest.h>
#include <map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace huffman;
namespace fs = std::filesystem;


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


TEST_SUITE("HuffmanArchive Tests") {

    void create_test_file(const std::string& path, const std::string& content) {
        std::ofstream f(path);
        f << content;
    }

    bool files_equal(const std::string& path1, const std::string& path2) {
        std::ifstream f1(path1), f2(path2);
        return std::equal(
            std::istreambuf_iterator<char>(f1),
            std::istreambuf_iterator<char>(),
            std::istreambuf_iterator<char>(f2)
        );
    }

    TEST_CASE("Compression Tests") {

        SUBCASE("Empty file compression") {
            std::string input = "empty.txt";
            std::string output = "empty_compressed.bin";

            create_test_file(input, "");
            HuffmanArchive archive(input, output);
            
            ArchiveInfo info = archive.compress();
            CHECK(info.original_size == 0);
            CHECK(info.compressed_size == 0);
            CHECK(info.extra_size > 0); // meta data
            
            fs::remove(input);
            fs::remove(output);
        }

        SUBCASE("Non-empty file compression") {
            std::string input = "sample.txt";
            std::string output = "compressed.bin";

            create_test_file("sample.txt", "hello world");
            HuffmanArchive archive(input, output);
            
            ArchiveInfo info = archive.compress();
            CHECK(info.original_size == 11);
            CHECK(info.compressed_size > 0);
            CHECK(info.compressed_size < info.original_size); // Проверка сжатия
            CHECK(info.extra_size > 0);
            
            fs::remove(input);
            fs::remove(output);
        }
    }

    TEST_CASE("Decompression Tests") {

        SUBCASE("Round-trip compression/decompression") {
            std::string f1 = "original.txt";
            std::string f2 = "compressed.bin";
            std::string f3 = "decompressed.txt";

            create_test_file(f1, "test data for huffman");

            HuffmanArchive compressor(f1, f2);
            ArchiveInfo comp_stats = compressor.compress();
            
            HuffmanArchive decompressor(f2, f3);
            ArchiveInfo decomp_stats = decompressor.decompress();
            
            CHECK(decomp_stats.original_size == comp_stats.original_size);
            CHECK(decomp_stats.compressed_size == comp_stats.compressed_size);
            CHECK(decomp_stats.extra_size == comp_stats.extra_size);
            CHECK(files_equal(f1, f3));
            
            fs::remove(f1);
            fs::remove(f2);
            fs::remove(f3);
        }

        SUBCASE("Throws on corrupted data") {
            std::string input = "corrupted.bin";
            std::string output = "output.txt";

            create_test_file(input, "invalid huffman data");
            HuffmanArchive archive(input, output);
            
            CHECK_THROWS_AS(archive.decompress(), HuffmanException);
            
            fs::remove(input);
            fs::remove(output);
        }
    }
}
