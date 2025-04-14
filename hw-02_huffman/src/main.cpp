#include "huffman.hpp"
#include <iostream>

std::map<unsigned char, size_t> createFrequencyMap(const std::string& input) {
    std::map<unsigned char, size_t> freqMap;
    
    for (char ch : input) {
        freqMap[ch]++;
    }
    
    return freqMap;
}

int main() {
    huffman::HuffmanTree tree( createFrequencyMap("") );

    tree.print();
    
    return 0;
}