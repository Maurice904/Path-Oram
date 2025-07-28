#include "Tree.h"

class Forest {
public:
    std::vector<Tree> trees;
    std::unordered_map<size_t, size_t> positionMap;
    Forest(size_t dataSize, size_t bucketSize = 4);
    void put(size_t position, int val, bool debugMode = false);
    std::optional<int> get(size_t position, bool debugMode = false);
    std::string toString() const;
};