#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include <cstddef>
#include <memory>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <string>

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
    HuffmanTree();
    explicit HuffmanTree(const std::vector<size_t>& freqMap);
    ~HuffmanTree();
    
    void build_tree(const std::vector<size_t>& freqMap);
    void serializeTree(Node* root, std::ofstream& os);

    std::map<uint8_t, std::string> get_codes() const;
    size_t decode(std::string& data, std::ostream& os) const;

private:   
    void delete_tree(Node* node);
    void generateCodeHelper(Node* node, std::string code);
    
private:
    Node* root_;
    std::map<uint8_t, std::string> symbolCodes_;
};

class HuffmanException : public std::runtime_error {
public:
    explicit HuffmanException(const std::string& message) : std::runtime_error(message) {}
};

}

#endif  // HUFFMAN_H_
