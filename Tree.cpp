#include "Tree.h"

size_t randomSizeT(size_t min, size_t max) {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    std::uniform_int_distribution<size_t> dis(min, max);
    return dis(gen);
}


Block::Block() : value(randomSizeT(0, INT_MAX)), originalPosition(randomSizeT(0, INT_MAX)), isDummy(true) {}

Block::Block(int value, size_t originalPosition, bool isDummy) 
    : value(value), originalPosition(originalPosition), isDummy(isDummy) {}

std::string Block::toString() const {
    if (isDummy) {
        return "[DUMMY]";
    }
    return "[Pos:" + std::to_string(originalPosition) + ", Val:" + std::to_string(value) + "]";
}

Node::Node(size_t bucketSize) : buckets(bucketSize, Block(randomSizeT(0, INT_MAX), randomSizeT(0, INT_MAX), true)), occupied(0), size(bucketSize) {}

void Node::clear() {
    for (auto& block : buckets) {
        block = Block(randomSizeT(0, INT_MAX),randomSizeT(0, INT_MAX), true);
    }
    occupied = 0;
}

void Node::put(Block& block) {
    if (occupied >= size) {
        throw std::runtime_error("buckets is full");
    } else {
        buckets[occupied] = std::move(block);
        occupied++;
    }
}

void Node::remove(size_t index) {
    if (index < occupied) {
        buckets[index] = Block();
        occupied--;
    } else {
        throw std::out_of_range("Invalid index");
    }
}

std::string Node::toString() const {
    std::string result = "Node(occupied:" + std::to_string(occupied) + "/" + std::to_string(size) + ") [";
    for (size_t i = 0; i < buckets.size(); i++) {
        if (i > 0) result += ", ";
        result += buckets[i].toString();
    }
    result += "]";
    return result;
}

Tree::Tree(size_t dataSize, size_t bucketSize, std::optional<int> preDesignedCap) {
    size_t size = 0;
    size_t nodeCount = (dataSize + bucketSize - 1) / bucketSize;
    treeLevel = 0;
    occupied = 0;
    while (nodeCount > size) {
        size = (size << 1) | 1;
        treeLevel++;
    }
    for (size_t i = 0; i < size; i ++) {
        nodes.emplace_back(Node(bucketSize));
    }
    if (preDesignedCap.has_value()) {
        capacity = preDesignedCap.value();
    } else {
        capacity = size * bucketSize;
    }
    leafStartIndex = size/2;
}

size_t Tree::getParent(size_t children) {
    if (children == 0) {
        return 0;
    }
    return (children - 1) / 2;
}

void Tree::readFromPath(size_t pathID) {
    size_t curNode = pathID;
    while (curNode > 0) {
        for (auto& block: nodes[curNode].buckets) {
            if (!block.isDummy) {
                stash.push_back(std::move(block));
            }
        }
        nodes[curNode].clear();
        curNode = getParent(curNode);
    }
    for (auto& block: nodes[0].buckets) {
        if (!block.isDummy) {
            stash.push_back(std::move(block));
        }
    }
    nodes[0].clear();
}

bool Tree::isSamePath(size_t curNode, size_t leafNode) {
    while (leafNode > curNode) {
        leafNode >>= 1;
    }
    return leafNode == curNode;
}

std::optional<int> Tree::access(Operation op, size_t position, int value, bool debugMode) {
    if (debugMode) {
        std::cout<<"occupied: " << occupied << ", capacity: " << capacity << std::endl;
    }
    if (positionMap.find(position) != positionMap.end()) {
        size_t prevPath = positionMap[position];
        readFromPath(prevPath);
    } else {
        if (op == Operation::READ) {
            std::cerr << "Position not found in positionMap for READ operation." << std::endl;
            return std::nullopt;
        } else if (op == Operation::WRITE && occupied >= capacity) {
            std::cerr << "Tree is full, cannot write new data." << std::endl;
            return std::nullopt;
        }
        readFromPath(randomSizeT(leafStartIndex, nodes.size() - 1));
        stash.push_back(Block(value, position, false));
        occupied++;
    }

    size_t newPath = randomSizeT(leafStartIndex, nodes.size() - 1);
    if (debugMode) {
        std::cout<<"newPath: "<< newPath << std::endl;
    }
    positionMap[position] = newPath;
    int returnValue = 0;
    for (auto& block : stash) {
        if (block.originalPosition == position) {
            if (op == Operation::READ) {
                returnValue = block.value;
            } else {
                block.value = value;
            }
            break;
        }
    }
    
    size_t stashBeforeEvict = stash.size();
    standardEvict();
    size_t stashAfterEvict = stash.size();

    if (debugMode) {
        std::cout << "Evict: stash size " << stashBeforeEvict << " -> " << stashAfterEvict << std::endl;
    }
    // size_t curLevel = treeLevel - 1;
    // size_t evictPathID = randomSizeT(leafStartIndex, nodes.size() - 1);
    // if (debugMode) {
    //     std::cout<<"Evicting pathID: " << evictPathID << std::endl;
    // }
    // while (evictPathID > 0) {
    //     Node& curNode = nodes[evictPathID];
    //     // propagate the block towards the leaf
    //     if (debugMode) {
    //         std::cout<<"curLevel:"<<curLevel<<std::endl;
    //         std::cout<<"curNodeID: " << evictPathID << std::endl;
    //     }
    //     curLevel --;
    //     size_t stashSize = stash.size();
    //     for (size_t i = 0; i < stashSize; i++) {
    //         if (curNode.occupied == curNode.size) {
    //             if (debugMode) {
    //                 std::cout << "cur node is full, cannot place more blocks." << std::endl;
    //             }
    //             break;
    //         }
    //         Block stashBlock = std::move(stash.front());
    //         stash.pop_front();
    //         if (isSamePath(evictPathID, positionMap[stashBlock.originalPosition])) {
    //             if (debugMode) {
    //                 std::cout << "Evict block from stash to curNode[" << evictPathID << "]"<< std::endl;
    //             }
    //             curNode.put(stashBlock);
    //         } else {
    //             if (debugMode) {
    //                 std::cout << "Block[" << i << "] is not on the same path or is dummy data, skipping." << std::endl;
    //             }
    //             stash.push_back(std::move(stashBlock));
    //         }
    //     }
    //     evictPathID = getParent(evictPathID);
    // }
    // size_t stashSize = stash.size();
    // Node& curNode = nodes[evictPathID];
    // for (size_t i = 0; i < stashSize; i++) {
    //     if (curNode.occupied == curNode.size) {
    //         if (debugMode) {
    //             std::cout << "cur node is full, cannot place more blocks." << std::endl;
    //         }
    //         break;
    //     }
    //     Block stashBlock = std::move(stash.front());
    //     stash.pop_front();
    //     if (isSamePath(evictPathID, positionMap[stashBlock.originalPosition])) {
    //         if (debugMode) {
    //             std::cout << "Evict cur block"<<stashBlock.toString()<<" from stash to curNode[" << evictPathID << "]"<< std::endl;
    //         }
    //         curNode.put(stashBlock);
    //     } else {
    //         if (debugMode) {
    //             std::cout << "Block[" << i << "] is not on the same path or is dummy data, skipping." << std::endl;
    //         }
    //         stash.push_back(std::move(stashBlock));
    //     }
    // }

    if (op == Operation::READ) {
        return returnValue;
    } else {
        return std::nullopt;
    }
}


void Tree::evict() {
    if (treeLevel == 1) {
        emptyStashTo(0);
        return;
    }
    
    std::vector<size_t> nodesToProcess;
    nodesToProcess.push_back(0);

    for (size_t level = 0; level < treeLevel - 1; ++level) {
        std::vector<size_t> nextLevelNodes;
        
        for (size_t nodeId : nodesToProcess) {
            emptyStashTo(nodeId);
            
            size_t leftChild = 2 * nodeId + 1;
            size_t rightChild = 2 * nodeId + 2;
            
            if (leftChild < nodes.size()) {
                nextLevelNodes.push_back(leftChild);
            }
            if (rightChild < nodes.size()) {
                nextLevelNodes.push_back(rightChild);
            }
        }
        
        nodesToProcess = nextLevelNodes;
    }
    
    for (size_t nodeId : nodesToProcess) {
        emptyStashTo(nodeId);
    }
}

void Tree::emptyStashTo(size_t nodeID) {
    if (stash.empty() || nodes[nodeID].occupied >= nodes[nodeID].size) {
        return;
    }
    std::deque<Block> remainingBlocks;
    size_t placedCount = 0;
    
    while (!stash.empty()) {
        Block curBlock = std::move(stash.front());
        stash.pop_front();
        
        if (nodes[nodeID].occupied < nodes[nodeID].size && 
            isSamePath(nodeID, positionMap[curBlock.originalPosition])) {
            nodes[nodeID].put(curBlock);
            placedCount++;
        } else {
            remainingBlocks.push_back(std::move(curBlock));
        }
    }
    stash = std::move(remainingBlocks);
}

void Tree::standardEvict() {
    if (treeLevel == 1) {
        emptyStashTo(0);
        return;
    }
    std::vector<size_t> currentLevel = {0};
    
    for (size_t level = 0; level < treeLevel; ++level) {
        std::vector<size_t> nextLevel;
        
        for (size_t nodeId : currentLevel) {
            emptyStashTo(nodeId);
            
            size_t leftChild = 2 * nodeId + 1;
            size_t rightChild = 2 * nodeId + 2;
            
            if (leftChild < nodes.size()) {
                nextLevel.push_back(leftChild);
            }
            if (rightChild < nodes.size()) {
                nextLevel.push_back(rightChild);
            }
        }
        
        currentLevel = nextLevel;
    }
}


std::string Tree::toString() const {
    std::string result = "Tree(levels:" + std::to_string(treeLevel) + 
                        ", leafStart:" + std::to_string(leafStartIndex) + ")\n";

    result += "Nodes:\n";
    for (size_t i = 0; i < nodes.size(); i++) {
        result += "  " + std::to_string(i) + ": " + nodes[i].toString() + "\n";
    }

    result += "Position Map:\n";
    for (const auto& pair : positionMap) {
        result += "  Pos " + std::to_string(pair.first) + " -> Path " + std::to_string(pair.second) + "\n";
    }

    result += "Stash (" + std::to_string(stash.size()) + " blocks):\n";
    for (const auto& block : stash) {
        result += "  " + block.toString() + "\n";
    }

    return result;
}

size_t Tree::getStashSize() const {
    return stash.size();
}
