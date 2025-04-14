#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include <cstddef>
#include <memory>
#include <map>

namespace huffman {

class HuffmanTree {
private:
    class Node {
    public:
        unsigned char data;
        std::size_t freq;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        Node(unsigned char data, std::size_t freq)
                : data(data), freq(freq), left(nullptr), right(nullptr) {}
    };

public:
    HuffmanTree();
    explicit HuffmanTree(const std::map<unsigned char, std::size_t>& freqMap);
    
    void build_tree(const std::map<unsigned char, std::size_t>& freqMap);

private:
    std::unique_ptr<Node> root;
};

class HuffmanArchive {
public:
    static ArchiveInfo compress(std::istream& in, std::ostream& out);
    static ArchiveInfo decompress(std::istream& in, std::ostream& out);
};

struct ArchiveInfo {
    std::size_t original_size;
    std::size_t compressed_size;
    std::size_t extra_size;
};

}

#endif  // HUFFMAN_H_
