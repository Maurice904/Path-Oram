#include "Tree.h"

#define BUCKET_SIZE 5

class Forest {
public:
    std::vector<Tree> trees;
    std::vector<std::optional<size_t>> positionMap;
    Forest(size_t dataSize, size_t bucketSize = BUCKET_SIZE, size_t maxSize = MAX_TREE_SIZE);
    void put(size_t position, int val, bool debugMode = false, std::optional<double> randomReadRatio = std::nullopt, bool ringFlag = false);
    std::optional<int> get(size_t position, bool debugMode = false, std::optional<double> randomReadRatio = std::nullopt, bool ringFlag = false);
    std::string toString() const;
    std::string getSizes() const;
    size_t getPosRange() const;
};