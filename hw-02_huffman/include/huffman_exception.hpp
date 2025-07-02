#ifndef HUFFMAN_EXCEPTION_H_
#define HUFFMAN_EXCEPTION_H_

#include <stdexcept>

namespace huffman {

class HuffmanException : public std::runtime_error {
public:
    explicit HuffmanException(const std::string& message) : std::runtime_error(message) {}
};

}

#endif  // HUFFMAN_EXCEPTION_H_
