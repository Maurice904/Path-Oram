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

enum Operation {
    READ,
    WRITE
};

int randomInt(int min, int max);

struct Block {
    int value;
    size_t originalPosition;
    bool isDummy;
    Block();
    Block(int value, size_t originalPosition, bool isDummy = false);
};

class Node {
public:
    std::vector<Block> buckets;
    size_t occupied;
    size_t size;
    bool isData;

    Node(size_t bucketSize = 4);
    void clear();
    void put(Block& block);
    void remove(size_t index);
};

class Tree {
public:
    std::vector<Node> nodes;
    std::unordered_map<size_t, size_t> positionMap;
    std::deque<Block> stash;
    size_t leafStartIndex;
    size_t treeLevel;

    Tree(size_t nodeCount, size_t bucketSize = 4);
    size_t getParent(size_t children);
    void readFromPath(size_t pathID);
    bool isSamePath(size_t curNode, size_t leafNode);
    std::optional<int> access(Operation op, size_t position, int value = 0);
};

#endif // TREE_H