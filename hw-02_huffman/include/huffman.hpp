#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include <cstddef>
#include <memory>
#include <map>
#include <stdexcept>
#include <iostream>
#include <string>

namespace huffman {

class HuffmanTree {
public:
    HuffmanTree();
    explicit HuffmanTree(const std::map<char, std::size_t>& freqMap);
    ~HuffmanTree();
    
    void build_tree(const std::map<char, std::size_t>& freqMap);
    std::map<char, std::string> get_codes() const;
    size_t decode(std::string& data, std::ostream& os) const;

    void print() const {
        HuffmanTreePrinter::printTree(root_);
    }

private:
    struct Node {
        char data;
        std::size_t freq;
        Node* left;
        Node* right;

        Node(char data, std::size_t freq)
                : data(data), freq(freq), left(nullptr), right(nullptr) {}
    };

class HuffmanTreePrinter {
public:
    static void printTree(Node* root) {
        if (!root) {
            std::cout << "Empty tree\n";
            return;
        }
        
        // Print tree structure
        std::cout << "Huffman Tree Structure:\n";
        std::cout << "----------------------\n";
        printTreeHelper(root, "", true);
        
        // Print codes
        std::cout << "\nHuffman Codes:\n";
        std::cout << "--------------\n";
        printCodes(root, "");
    }

private:
    static void printTreeHelper(Node* node, const std::string& prefix, bool isLeft) {
        if (!node) return;
        
        std::cout << prefix;
        std::cout << (isLeft ? "├──" : "└──");
        
        // Print node information
        if (node->data == '\0') {
            std::cout << "INTERNAL (" << node->freq << ")\n";
        } else {
            if (isprint(node->data)) {
                std::cout << "'" << node->data << "' (" << node->freq << ")\n";
            } else {
                std::cout << "0x" << std::hex << (int)node->data << std::dec << " (" << node->freq << ")\n";
            }
        }
        
        // Recursively print children
        printTreeHelper(node->left, prefix + (isLeft ? "│   " : "    "), true);
        printTreeHelper(node->right, prefix + (isLeft ? "│   " : "    "), false);
    }

    static void printCodes(Node* node, const std::string& code) {
        if (!node) return;
        
        if (!node->left && !node->right) {
            if (isprint(node->data)) {
                std::cout << "'" << node->data << "' : " << code << "\n";
            } else {
                std::cout << "0x" << std::hex << (int)node->data << std::dec << " : " << code << "\n";
            }
        }
        
        printCodes(node->left, code + "0");
        printCodes(node->right, code + "1");
    }
};
    
    void delete_tree(Node* node);
    void generateCodeHelper(Node* node, std::string code);
    
private:
    Node* root_;
    std::map<char, std::string> symbolCodes_;
};

struct ArchiveInfo {
    std::size_t original_size;
    std::size_t compressed_size;
    std::size_t extra_size;

    ArchiveInfo(std::size_t os, std::size_t cs, std::size_t es)
        : original_size(os), compressed_size(cs), extra_size(es) {}
};

class HuffmanArchive {
public:
    static ArchiveInfo compress(std::istream&, std::ostream&);
    static ArchiveInfo decompress(std::istream&, std::ostream&);
};

class HuffmanException : public std::runtime_error {
public:
    explicit HuffmanException(const std::string& message) : std::runtime_error(message) {}
};

}

#endif  // HUFFMAN_H_
