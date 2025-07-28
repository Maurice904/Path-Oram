#include "Forest.h"

Forest::Forest(size_t dataSize, size_t bucketSize) {
    if (dataSize > MAX_TREE_SIZE) {
        int treeCount = (dataSize + MAX_TREE_SIZE - 1) / MAX_TREE_SIZE;
        for (int i = 0; i < treeCount; ++i) {
            trees.push_back(Tree(MAX_TREE_SIZE, bucketSize));
        }
    } else {
        trees.push_back(Tree(dataSize, bucketSize));
    }
}

void Forest::put(size_t position, int val, bool debugMode) {
    if (positionMap.find(position) != positionMap.end()) {
        size_t treeIndex = positionMap[position];
        trees[treeIndex].access(WRITE, position, val, debugMode);
    } else {
        for (size_t i = 0; i < trees.size(); i ++) {
            if (trees[i].occupied < trees[i].capacity) {
                trees[i].access(WRITE, position, val, debugMode);
                positionMap[position] = i;
                return;
            }
        }
        std::cerr <<"all trees are full, cannot write new data." << std::endl;
    }
}

std::optional<int> Forest::get(size_t position, bool debugMode) {
    if (positionMap.find(position) != positionMap.end()) {
        size_t treeIndex = positionMap[position];
        return trees[treeIndex].access(READ, position, 0, debugMode);
    }
    return std::nullopt;
}

std::string Forest::toString() const {
    std::string result;
    for (const auto& tree : trees) {
        result +=  tree.toString() + "\n=====================================\n";
    }

    return result;
}