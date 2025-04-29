#include "huffman.hpp"
#include <memory>
#include <vector>
#include <queue>
#include <bitset>
#include <iostream>
#include <fstream>


// HuffmanTree

huffman::HuffmanTree::HuffmanTree() : root_(nullptr) {}

huffman::HuffmanTree::HuffmanTree(const std::map<char, std::size_t>& freqMap) {
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

void huffman::HuffmanTree::build_tree(const std::map<char, std::size_t>& freqMap) {
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

        auto top = new Node(0, left->freq + right->freq);
        top->left = left;
        top->right = right;

        minHeap.push(top);
    }

    if (!minHeap.empty())
        root_ = minHeap.top();

    generateCodeHelper(root_, "");
}

void huffman::HuffmanTree::generateCodeHelper(Node* node, std::string code) {
    if (node->right)
        generateCodeHelper(node->right, code + "0");
    if (node->left)
        generateCodeHelper(node->left, code + "1");
    
    if (!node->right && !node->left)
        symbolCodes_[node->data] = (code == "" ? "0" : code);
}


std::map<char, std::string> huffman::HuffmanTree::get_codes() const {
    return symbolCodes_;
}

size_t huffman::HuffmanTree::decode(std::string& data, std::ostream& os) const {
    auto current = root_;
    size_t decodedSize = 0;
    
    for (char bit : data) {
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

// HuffmanArchive

huffman::ArchiveInfo huffman::HuffmanArchive::compress(std::istream& is, std::ostream& os) {
    ArchiveInfo stats{0, 0, 0};
    
    std::string inputData = "";
    std::map<char, size_t> freqMap;
    char c;
    while (is.get(c)) {
        freqMap[c]++;
        stats.original_size += 1;
        inputData += c;
    }

    HuffmanTree huffmanTree(freqMap);
    auto codes = huffmanTree.get_codes();
    
    size_t tableSize = freqMap.size();
    os.write(reinterpret_cast<const char*>(&tableSize), sizeof(tableSize));
    
    for (const auto& pair : freqMap) {
        os.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));
        os.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
    }
    
    stats.extra_size = sizeof(tableSize) + tableSize * (sizeof(char) + sizeof(size_t));
    
    std::string encodedData;
    for (char c : inputData)
        encodedData += codes[c];
    
    size_t paddingBits = (8 - (encodedData.size() % 8)) % 8;
    os.write(reinterpret_cast<const char*>(&paddingBits), sizeof(paddingBits));
    stats.extra_size += sizeof(paddingBits);
    
    for (size_t i = 0; i < encodedData.size(); i += 8) {
        std::string byteStr = encodedData.substr(i, 8);
        if (byteStr.size() < 8) {
            byteStr.append(paddingBits, '0');
        }
        std::bitset<8> bits(byteStr);
        char byte = static_cast<char>(bits.to_ulong());
        os.put(byte);
    }
    
    stats.compressed_size = sizeof(tableSize) + tableSize * (sizeof(char) + sizeof(size_t)) + 
                            sizeof(paddingBits) + (encodedData.size() + paddingBits) / 8;
    
    return stats;
}

huffman::ArchiveInfo huffman::HuffmanArchive::decompress(std::istream& is, std::ostream& os) {
    ArchiveInfo stats{0, 0, 0};
    
    size_t tableSize;
    is.read(reinterpret_cast<char*>(&tableSize), sizeof(tableSize));
    if (!is)
        throw HuffmanException("Failed to read frequency table size");
    
    stats.extra_size = sizeof(tableSize);
    
    if (tableSize == 0) return stats;
    
    std::map<char, size_t> freqMap;
    for (size_t i = 0; i < tableSize; ++i) {
        char c;
        size_t freq;
        is.read(reinterpret_cast<char*>(&c), sizeof(c));
        is.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        if (!is)
            throw HuffmanException("Failed to read frequency table");
        
        freqMap[c] = freq;
        stats.extra_size += sizeof(c) + sizeof(freq);
    }
    
    size_t paddingBits;
    is.read(reinterpret_cast<char*>(&paddingBits), sizeof(paddingBits));
    if (!is)
        throw HuffmanException("Failed to read padding bits");
    stats.extra_size += sizeof(paddingBits);
    
    std::vector<char> compressedData((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    
    HuffmanTree huffmanTree(freqMap);
    
    std::string encodedData;
    for (char byte : compressedData) {
        encodedData += std::bitset<8>(byte).to_string();
    }
    
    if (!encodedData.empty() && paddingBits > 0) {
        encodedData.erase(encodedData.size() - paddingBits, paddingBits);
    }

    size_t decodedSize = huffmanTree.decode(encodedData, os);
    
    stats.original_size = decodedSize;
    stats.compressed_size = sizeof(tableSize) + tableSize * (sizeof(char) + sizeof(size_t)) + 
                            sizeof(paddingBits) + compressedData.size();
    
    return stats;
}