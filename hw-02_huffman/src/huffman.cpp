#include "huffman.hpp"
#include <memory>
#include <vector>
#include <queue>


huffman::HuffmanTree::HuffmanTree() : root_(nullptr) {}

huffman::HuffmanTree::HuffmanTree(const std::map<unsigned char, std::size_t>& freqMap) {
    build_tree(freqMap);
}

huffman::HuffmanTree::~HuffmanTree() {
    delete_tree(root_);
}
        
void huffman::HuffmanTree::build_tree(const std::map<unsigned char, std::size_t>& freqMap) {
    if (freqMap.empty()) {
        root_ = nullptr;
        return;
    }

    auto compare = [](const Node* left, const Node* right) {
        return left->freq > right->freq;
    };

    std::priority_queue<Node*, std::vector<Node*>, decltype(compare)> minHeap(compare);

    for (const auto& pair : freqMap)
        minHeap.push(new Node(pair.first, pair.second));

    while (minHeap.size() != 1) {
        Node* left = minHeap.top();
        minHeap.pop();

        Node* right = minHeap.top();
        minHeap.pop();

        auto top = new Node('\0', left->freq + right->freq);
        top->left = left;
        top->right = right;

        minHeap.push(top);
    }

    if (!minHeap.empty())
        root_ = minHeap.top();

    generateCodeHelper(root_, "", symbolCodes_);
}

void huffman::HuffmanTree::delete_tree(Node* node) {
    if (node) {
        delete_tree(node->left);
        delete_tree(node->right);
        delete node;
    }
}

std::map<unsigned char, std::string> huffman::HuffmanTree::get_codes() const {
    return symbolCodes_;
}

void huffman::HuffmanTree::generateCodeHelper(Node* node, std::string code, 
                            std::map<unsigned char, std::string>& huffmanCode) {
    if (node->right)
        generateCodeHelper(node->right, code + "0", huffmanCode);
    if (node->left)
        generateCodeHelper(node->left, code + "1", huffmanCode);
    
    if (!node->right && !node->left)
        huffmanCode[node->data] = (code == "" ? "0" : code);
}