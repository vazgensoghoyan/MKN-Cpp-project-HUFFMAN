#include "huffman.hpp"
#include <iostream>

std::map<unsigned char, size_t> createFrequencyMap(const std::string& input) {
    std::map<unsigned char, size_t> freqMap;
    
    for (char ch : input) {
        freqMap[ch]++;
    }
    
    return freqMap;
}

void test() {
    huffman::HuffmanTree tree( createFrequencyMap("abhjdjjajajajjj hello") );

    tree.print();

    auto huffmanCode = tree.get_codes();

    for (auto it = huffmanCode.begin(); it != huffmanCode.end(); ++it) {
        std::cout << it->first << ' ' << it->second << std::endl;
    }
}

int main() {
    test();

    return 0;
}