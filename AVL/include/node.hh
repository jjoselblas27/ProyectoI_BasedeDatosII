#pragma once
#include "record.hh"

constexpr long NULL_POSITION = -1;
constexpr long INITIAL_POSITION = 0;

#pragma pack(push, 1)
template<typename TK = char[13]>
struct Node {
    TK key{};
    long physical_position = NULL_POSITION;
    long left = NULL_POSITION, right = NULL_POSITION, 
         next = NULL_POSITION, height = 0;

    explicit Node() = default;

    explicit Node(const char* key, long physicalPosition) :
        physical_position(physicalPosition) {
        std::memcpy(&this->key, key, sizeof(TK));
    }
    
    explicit Node(const TK& key, long physicalPosition) :
        physical_position(physicalPosition) {
        std::memcpy(&this->key, &key, sizeof(TK));
    }

    explicit Node(TK& key, long physicalPosition) :
        physical_position(physicalPosition) {
        std::memcpy(&this->key, &key, sizeof(TK));
    }

    friend std::ostream& operator<<(std::ostream& stream, const Node& node) {
        stream.write(reinterpret_cast<const char*>(&node), sizeof(Node));
        return stream;
    }

    friend std::istream& operator>>(std::istream& stream, Node& node) {
        stream.read(reinterpret_cast<char*>(&node), sizeof(Node));
        return stream;
    }

    std::string to_string() {
        std::stringstream stream;
        stream << "(key: " << key << ", 'physical position': " << physical_position 
               << ", left: " << left << ", right: " << right << ", next: " << next 
               << ", height: " << height << ")";
        return stream.str();
    }
};
#pragma pack(pop)
