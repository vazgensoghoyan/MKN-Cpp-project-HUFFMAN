#include "huffman.hpp"

namespace huffman {

// HuffmanTree

HuffmanTree::HuffmanTree(const std::map<uint8_t, size_t>& freqMap) {
    build_tree(freqMap);
}

HuffmanTree::~HuffmanTree() {
    delete_tree(root_);
}

void HuffmanTree::delete_tree(Node* node) {
    if (node) {
        delete_tree(node->left);
        delete_tree(node->right);
        delete node;
    }
}

void HuffmanTree::build_tree(const std::map<uint8_t, size_t>& freqMap) {
    if (freqMap.empty()) {
        root_ = nullptr;
        return;
    }

    auto compare = [](const Node* left, const Node* right) {
        return left->freq > right->freq;
    };

    std::priority_queue<Node*, std::vector<Node*>, decltype(compare)> minHeap(compare);

    for (auto& pair : freqMap)
        minHeap.push(new Node(pair.first, pair.second));

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

void HuffmanTree::generateCodeHelper(Node* node, const std::string& code) {
    if (node->right)
        generateCodeHelper(node->right, code + "1");
    if (node->left)
        generateCodeHelper(node->left, code + "0");
    
    if (node->right || node->left)
        return;
    
    symbolCodes_.emplace(node->data, code == "" ? "0" : code);
}

std::map<uint8_t, std::string> HuffmanTree::get_codes() const {
    return symbolCodes_;
}

} // namespace huffman