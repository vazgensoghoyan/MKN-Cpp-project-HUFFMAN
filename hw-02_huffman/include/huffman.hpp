#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include <cstddef>
#include <memory>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <string>
#include <queue>
#include <bitset>
#include <fstream>

namespace huffman {

class HuffmanTree {
private:
    struct Node {
        uint8_t data;
        size_t freq;
        Node* left;
        Node* right;

        Node(uint8_t data, size_t freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
    };

public:
    explicit HuffmanTree(const std::map<uint8_t, size_t>& freqMap);
    ~HuffmanTree();
    
    std::map<uint8_t, std::string> get_codes() const;

private:
    void build_tree(const std::map<uint8_t, size_t>& freqMap);
    void delete_tree(Node* node);
    void generateCodeHelper(Node* node, const std::string& code);
    
private:
    Node* root_;
    std::map<uint8_t, std::string> symbolCodes_;
};

}

#endif  // HUFFMAN_H_
