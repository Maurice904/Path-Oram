#include "Forest.h"

Forest::Forest(size_t dataSize, size_t bucketSize, size_t maxSize) {
    dataCount = dataSize;
    if ((dataSize + bucketSize - 1)/bucketSize > maxSize) {
        size_t treeCount = (dataSize + maxSize - 1) / maxSize;
        for (size_t i = 0; i < treeCount; ++i) {
            trees.push_back(Tree(maxSize, bucketSize, (dataSize + treeCount - 1) / treeCount));
        }
    } else {
        trees.push_back(Tree(dataSize, bucketSize));
    }
}

void Forest::put(size_t position, int val, bool debugMode, std::optional<double> randomReadRatio, bool ringFlag) {
    if (positionMap.find(position) != positionMap.end()) {
        size_t treeIndex = positionMap[position];
        trees[treeIndex].access(WRITE, position, val, debugMode, randomReadRatio, ringFlag);
    } else {
        for (size_t i = 0; i < trees.size(); i ++) {
            if (trees[i].occupied < trees[i].capacity) {
                trees[i].access(WRITE, position, val, debugMode, randomReadRatio, ringFlag);
                positionMap[position] = i;
                return;
            }
        }
        std::cerr <<"all trees are full, cannot write new data." << std::endl;
    }
}

std::optional<int> Forest::get(size_t position, bool debugMode, std::optional<double> randomReadRatio, bool ringFlag) {
    if (positionMap.find(position) != positionMap.end()) {
        size_t treeIndex = positionMap[position];
        return trees[treeIndex].access(READ, position, 0, debugMode, randomReadRatio, ringFlag);
    }
    return std::nullopt;
}

std::string Forest::getSizes() const {
    std::string ret;
    ret = "Forest has " + std::to_string(trees.size()) + " trees.\n";
    ret += "Each tree has " + std::to_string(trees[0].capacity) + " capacity.\n";
    for (size_t i = 0; i < trees.size(); i ++) {
        ret += "Tree[" +std::to_string(i)+ "] have occupied: " + std::to_string(trees[i].occupied) +
               ", capacity: " + std::to_string(trees[i].capacity) +" stash size: " + std::to_string(trees[i].stash.size()) + "\n";
    }
    return ret;
}


std::string Forest::toString() const {
    std::string result;
    for (const auto& tree : trees) {
        result +=  tree.toString() + "\n=====================================\n";
    }

    return result;
}

size_t Forest::getPosRange() const {
    return dataCount;
}