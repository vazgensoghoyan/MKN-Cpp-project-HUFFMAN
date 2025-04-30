#include "huffman.hpp"
#include <memory>
#include <vector>
#include <queue>
#include <bitset>
#include <iostream>
#include <fstream>

// HuffmanTree

huffman::HuffmanTree::HuffmanTree(const std::vector<std::size_t>& freqMap) {
    build_tree(freqMap);
}

huffman::HuffmanTree::~HuffmanTree() {
    delete_tree(root_);
}

void huffman::HuffmanTree::delete_tree(Node* node) {
    if (node) {
        delete_tree(node->left);
        delete_tree(node->right);
        delete node;
    }
}

void huffman::HuffmanTree::build_tree(const std::vector<std::size_t>& freqMap) {
    if (freqMap.empty()) {
        root_ = nullptr;
        return;
    }

    auto compare = [](const Node* left, const Node* right) {
        return left->freq > right->freq;
    };

    std::priority_queue<Node*, std::vector<Node*>, decltype(compare)> minHeap(compare);

    for (size_t i = 0; i < freqMap.size(); ++i)
        if (freqMap[i] != 0)
            minHeap.push(new Node((uint8_t)i, freqMap[i]));

    while (minHeap.size() != 1) {
        Node* left = minHeap.top();
        minHeap.pop();

        Node* right = minHeap.top();
        minHeap.pop();

        auto top = new Node(0, left->freq + right->freq);
        top->left = left;
        top->right = right;

        minHeap.push(top);
    }

    if (!minHeap.empty())
        root_ = minHeap.top();

    generateCodeHelper(root_, "");
}

void huffman::HuffmanTree::generateCodeHelper(Node* node, const std::string& code) {
    if (node->right)
        generateCodeHelper(node->right, code + "1");
    if (node->left)
        generateCodeHelper(node->left, code + "0");
    
    if (node->right || node->left)
        return;
    
    symbolCodes_.emplace(node->data, code == "" ? "0" : code);
}


std::map<uint8_t, std::string> huffman::HuffmanTree::get_codes() const {
    return symbolCodes_;
}

size_t huffman::HuffmanTree::decode(std::string& data, std::ostream& os) const {
    auto current = root_;
    size_t decodedSize = 0;
    
    for (uint8_t bit : data) {
        if (bit == '0')
            current = current->left;
        else
            current = current->right;
        
        if (!current->right && !current->left) {
            os.put(current->data);
            decodedSize++;
            current = root_;
        }
    }

    return decodedSize;
}
