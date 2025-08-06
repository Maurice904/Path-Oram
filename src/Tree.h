#ifndef TREE_H
#define TREE_H


#include <vector>
#include <string>
#include <stdexcept>
#include <random>
#include <optional>
#include <deque>
#include <climits>
#include <iostream>
#include <list>

#define MAX_TREE_SIZE 65535

enum Operation {
    READ,
    WRITE
};


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
    void deFrag();
    void put(Block& block);
    void remove(size_t index);
    std::string toString() const;
};

class Tree {
public:
    std::vector<Node> nodes;
    std::vector<std::optional<size_t>> positionMap;
    std::deque<Block> stash;
    size_t leafStartIndex;
    size_t treeLevel;
    size_t capacity;
    size_t occupied;
    size_t leafCount;
    size_t ringPath;
    size_t mid;
    size_t maxStashSize = 0;
    Tree(size_t nodeCount, size_t bucketSize = 4, std::optional<int> preDesignedCap = std::nullopt);
    size_t getRange() const;
    size_t getParent(size_t children);
    void readFromPath(size_t pathID,size_t target,bool debugMode = false, std::optional<double> randomReadRatio = std::nullopt);
    bool isSamePath(size_t curNode, size_t leafNode);
    std::optional<int> access(Operation op, size_t position, int value = 0, bool debugMode = false, std::optional<double> randomReadRatio = std::nullopt, bool ringFlag = false);
    std::string toString() const;
    void evict(size_t evictPathID, bool debugMode = false);
    void emptyStashTo(size_t nodeID, bool debugMode = false);
};

#endif // TREE_H