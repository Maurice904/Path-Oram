#include "Forest.h"

Forest::Forest(size_t dataSize, size_t bucketSize, size_t maxSize): positionMap(dataSize + 1, std::nullopt) {
    size_t treeCount = 1;
    if ((dataSize + bucketSize - 1)/bucketSize > maxSize) {
        treeCount = (dataSize + maxSize - 1) / maxSize;
        for (size_t i = 0; i < treeCount; ++i) {
            trees.push_back(Tree(maxSize, bucketSize, (dataSize + treeCount - 1) / treeCount));
        }
    } else {
        trees.push_back(Tree(dataSize, bucketSize));
    }
}

void Forest::put(size_t position, int val, bool debugMode, std::optional<double> randomReadRatio, bool ringFlag) {
    if (position >= positionMap.size()) {
        std::cerr << "Position out of bounds in forest positionMap." << std::endl;
        std::cerr<< "max position: " << positionMap.size() - 1 << ", looking for position: " << position << std::endl;
        return;
    }
    if (positionMap[position].has_value()) {
        size_t treeIndex = positionMap[position].value();
        trees[treeIndex].access(WRITE, position, val, debugMode, randomReadRatio, ringFlag);
    } else {
        for (size_t i = 0; i < trees.size(); i ++) {
            if (trees[i].occupied < trees[i].capacity) {
                trees[i].access(WRITE, position - (i * trees[i].capacity), val, debugMode, randomReadRatio, ringFlag);
                positionMap[position] = i;
                return;
            }
        }
        std::cerr <<"all trees are full, cannot write new data." << std::endl;
    }
}

std::optional<int> Forest::get(size_t position, bool debugMode, std::optional<double> randomReadRatio, bool ringFlag) {
    if (position >= positionMap.size()) {
        std::cerr << "Position out of bounds in forest positionMap." << std::endl;
        return std::nullopt;
    }
    if (positionMap[position].has_value()) {
        size_t treeIndex = positionMap[position].value();
        return trees[treeIndex].access(READ, position - (treeIndex * trees[treeIndex].capacity), 0, debugMode, randomReadRatio, ringFlag);
    }
    return std::nullopt;
}

std::string Forest::getSizes() const {
    std::string ret;
    ret = "Forest has " + std::to_string(trees.size()) + " trees.\n";
    ret += "Each tree has " + std::to_string(trees[0].capacity) + " capacity.\n";
    for (size_t i = 0; i < trees.size(); i ++) {
        ret += "Tree[" +std::to_string(i)+ "] have occupied: " + std::to_string(trees[i].occupied) +
               ", capacity: " + std::to_string(trees[i].capacity) +" max stash size: " + std::to_string(trees[i].maxStashSize) + "\n";
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
    return positionMap.size();
}