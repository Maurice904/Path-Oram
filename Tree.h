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

int randomInt(int min, int max);

struct Block {
    int value;
    uint32_t originalPosition;
    bool isDummy;
    Block();
    Block(int value, uint32_t originalPosition, bool isDummy = false);
    std::string toString() const;
};

class Node {
public:
    std::vector<Block> buckets;
    uint32_t occupied;
    uint32_t size;

    Node(uint32_t bucketSize = 4);
    void clear();
    void put(Block& block);
    void remove(uint32_t index);
    std::string toString() const;
};

class Tree {
public:
    std::vector<Node> nodes;
    std::unordered_map<uint32_t, uint32_t> positionMap;
    std::deque<Block> stash;
    uint32_t leafStartIndex;
    uint32_t treeLevel;

    Tree(uint32_t leafCount, uint32_t bucketSize = 4);
    uint32_t getParent(uint32_t children) const;
    std::vector<uint32_t> getPath(uint32_t leafID) const;  
    void readFromPath(uint32_t pathID);
    void readFromNodes(const std::vector<uint32_t>& nodeIDs);
    bool isSamePath(uint32_t curNode, uint32_t leafNode) const;
    std::optional<int> access(Operation op,
                            uint32_t position,
                            int   value     = 0,
                            bool  debugMode = false);
    std::string toString() const;
};

#endif // TREE_H