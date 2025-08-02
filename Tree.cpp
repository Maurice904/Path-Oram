#include "Tree.h"
#include "rgen.h"


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


void Node::deFrag() {
    std::vector<Block> newBckets;
    for (const auto& block : buckets) {
        if (!block.isDummy) {
            newBckets.push_back(block);
        }
    }
    occupied = newBckets.size();
    for (size_t i = occupied; i < size; i++) {
        newBckets.push_back(Block(randomSizeT(0, INT_MAX), randomSizeT(0, INT_MAX), true));
    }
    buckets = std::move(newBckets);
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

void Tree::readFromPath(size_t pathID, size_t target, bool debugMode, std::optional<double> randomReadRatio) {
    size_t curNode = pathID;
    while (curNode > 0) {
        if (debugMode) {
            std::cout << "Reading from pathID: " << curNode << std::endl;
        }
        for (auto& block: nodes[curNode].buckets) {
            if (!block.isDummy) {
                if (debugMode) {
                    std::cout<< "Stash block: " << block.toString() << std::endl;
                }
                if (randomReadRatio.has_value()) {
                    if (block.originalPosition != target) {
                        double rDouble = randomDouble(0.0, 1.0);
                        if (rDouble < randomReadRatio.value()) {
                            stash.push_back(std::move(block));
                            block = Block();
                        } else {
                            if (debugMode) {
                                std::cout << "skipping instead"<< std::endl;
                            }
                        }
                    } else {
                        stash.push_back(std::move(block));
                        block = Block();
                    }
                } else {
                    stash.push_back(std::move(block));
                }
            }
        }
        if (!randomReadRatio.has_value()) {
            nodes[curNode].clear();
        } else {
            nodes[curNode].deFrag();
        }
        curNode = getParent(curNode);
    }
    for (auto& block: nodes[0].buckets) {
        if (!block.isDummy) {
            if (debugMode) {
                std::cout<< "Stash block: " << block.toString() << std::endl;
            }
            if (randomReadRatio.has_value()) {
                if (block.originalPosition != target) {
                    double rDouble = randomDouble(0.0, 1.0);
                    if (rDouble < randomReadRatio.value()) {
                        stash.push_back(std::move(block));
                        block = Block();
                    } else {
                        if (debugMode) {
                            std::cout << "skipping instead"<< std::endl;
                        }
                    }
                } else {
                    stash.push_back(std::move(block));
                    block = Block();
                }
            } else {
                stash.push_back(std::move(block));
            }
        }
    }
    if (!randomReadRatio.has_value()) {
        nodes[0].clear();
    } else {
        nodes[0].deFrag();
    }
    if (debugMode) {
        std::cout<<"==========================="<<std::endl;
    }
}

bool Tree::isSamePath(size_t curNode, size_t leafNode) {
    while (leafNode > curNode) {
        leafNode = getParent(leafNode);
    }
    return leafNode == curNode;
}

std::optional<int> Tree::access(Operation op, size_t position, int value, bool debugMode, std::optional<double> randomReadRatio) {
    if (debugMode) {
        std::cout<<"occupied: " << occupied << ", capacity: " << capacity << std::endl;
    }
    size_t prevPath = leafStartIndex;
    if (positionMap.find(position) != positionMap.end()) {
        prevPath = positionMap[position];
        if (debugMode) {
            std::cout<<"position found in map, prevPath: " << prevPath << std::endl;
        }
        readFromPath(prevPath,position,debugMode, randomReadRatio);
    } else {
        if (op == Operation::READ) {
            std::cerr << "Position not found in positionMap for READ operation." << std::endl;
            return std::nullopt;
        } else if (op == Operation::WRITE && occupied >= capacity) {
            std::cerr << "Tree is full, cannot write new data." << std::endl;
            return std::nullopt;
        }
        prevPath = randomSizeT(leafStartIndex, nodes.size() - 1);
        if (debugMode) {
            std::cout<<"position not found in map, generating new path: "<< prevPath << std::endl;
        }
        readFromPath(prevPath, position, debugMode, randomReadRatio);
        stash.push_back(Block(value, position, false));
        occupied++;
    }

    size_t newPath = randomSizeT(leafStartIndex, nodes.size() - 1);
    if (debugMode) {
        std::cout<<"newPath: "<< newPath << std::endl;
    }
    positionMap[position] = newPath;
    int returnValue = 0;
    bool found = false;
    for (auto& block : stash) {
        if (block.originalPosition == position) {
            found = true;
            if (debugMode) {
                std::cout << "Found block in stash: " << block.toString() << std::endl;
            }
            if (op == Operation::READ) {
                returnValue = block.value;
            } else {
                block.value = value;
                if (debugMode) {
                    std::cout << "Updating value from " << block.value << " to " << value << std::endl;
                }
            }
            break;
        }
    }
    if (!found) {
        std::cerr << "Block with position " << position << " not found in stash." << std::endl;
        return std::nullopt;
    }
    evict(prevPath, debugMode);

    if (op == Operation::READ) {
        return returnValue;
    } else {
        return std::nullopt;
    }
}


void Tree::evict(size_t evictPathID, bool debugMode) {
    while (evictPathID > 0) {
        emptyStashTo(evictPathID, debugMode);
        evictPathID = getParent(evictPathID);
    }
    emptyStashTo(0, debugMode);
    if (debugMode) {
        std::cout<<"==========================="<<std::endl;
    }
}

void Tree::emptyStashTo(size_t nodeID, bool debugMode) {
    size_t stashSize = stash.size();
    for (size_t i = 0; i < stashSize; i ++) {
        if (nodes[nodeID].occupied == nodes[nodeID].size) {
            break;
        }
        Block curBlock = std::move(stash.front());
        stash.pop_front();
        if (isSamePath(nodeID, positionMap[curBlock.originalPosition])) {
            if (debugMode) {
                std::cout<<positionMap[curBlock.originalPosition]<<" is on the same path as nodeID: " << nodeID << std::endl;
                std::cout<<"putting block: " << curBlock.toString() << " to node: " << nodeID << std::endl;
            }
            nodes[nodeID].put(curBlock);
        } else {
            stash.push_back(std::move(curBlock));
        }
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
