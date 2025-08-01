#ifndef TREE_H
#define TREE_H

#include <vector>
#include <string>
#include <stdexcept>
#include <random>
#include <unordered_map>
#include <optional>
#include <deque>
#include <climits>
#include <iostream>

#define MAX_TREE_SIZE 65535

enum Operation {
    READ,
    WRITE
};

size_t randomSizeT(size_t min, size_t max);

struct Block {
    int value;
    size_t originalPosition;
    bool isDummy;
    Block();
    Block(int value, size_t originalPosition, bool isDummy = false);
    std::string toString() const;
};

class Node {
public:
    std::vector<Block> buckets;
    size_t occupied;
    size_t size;

    Node(size_t bucketSize = 4);
    void clear();
    void put(Block& block);
    void remove(size_t index);
    std::string toString() const;
};

class Tree {
public:
    std::vector<Node> nodes;
    std::unordered_map<size_t, size_t> positionMap;
    std::deque<Block> stash;
    size_t leafStartIndex;
    size_t treeLevel;
    size_t capacity;
    size_t occupied;

    Tree(size_t nodeCount, size_t bucketSize = 4, std::optional<int> preDesignedCap = std::nullopt);
    size_t getParent(size_t children);
    void readFromPath(size_t pathID);
    bool isSamePath(size_t curNode, size_t leafNode);
    std::optional<int> access(Operation op, size_t position, int value = 0, bool debugMode = false);
    std::string toString() const;
    void evict();
    void emptyStashTo(size_t nodeID);
    void standardEvict();
    size_t getStashSize() const;
};

#endif // TREE_H
