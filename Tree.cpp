#include "Tree.h"

int randomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}


Block::Block() : value(randomInt(INT_MIN, INT_MAX)), originalPosition(randomInt(INT_MIN, INT_MAX)), isDummy(true) {}

Block::Block(int value, size_t originalPosition, bool isDummy) 
    : value(value), originalPosition(originalPosition), isDummy(isDummy) {}

std::string Block::toString() const {
    if (isDummy) {
        return "[DUMMY]";
    }
    return "[Pos:" + std::to_string(originalPosition) + ", Val:" + std::to_string(value) + "]";
}

Node::Node(size_t bucketSize) : buckets(bucketSize, Block(randomInt(INT_MIN, INT_MAX), randomInt(INT_MIN, INT_MAX), true)), occupied(0), size(bucketSize) {}

void Node::clear() {
    for (auto& block : buckets) {
        block = Block(randomInt(INT_MIN, INT_MAX),randomInt(INT_MIN, INT_MAX), true);
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

Tree::Tree(size_t nodeCount, size_t bucketSize) {
    size_t size = 0;
    treeLevel = 0;
    while (nodeCount > size) {
        size = (size << 1) | 1;
        treeLevel++;
    }
    for (size_t i = 0; i < size; i ++) {
        nodes.emplace_back(Node(bucketSize));
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
    if (positionMap.find(position) != positionMap.end()) {
        size_t prevPath = positionMap[position];
        readFromPath(prevPath);
    } else {
        readFromPath(randomInt(leafStartIndex, nodes.size() - 1));
        stash.push_back(Block(value, position, false));
    }

    size_t newPath = randomInt(leafStartIndex, nodes.size() - 1);
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
    size_t curLevel = treeLevel - 1;
    size_t evictPathID = randomInt(leafStartIndex, nodes.size() - 1);
    if (debugMode) {
        std::cout<<"Evicting pathID: " << evictPathID << std::endl;
    }
    while (evictPathID > 0) {
        Node& curNode = nodes[evictPathID];
        // propagate the block towards the leaf
        if (debugMode) {
            std::cout<<"curLevel:"<<curLevel<<std::endl;
            std::cout<<"curNodeID: " << evictPathID << std::endl;
        }
        curLevel --;
        size_t stashSize = stash.size();
        for (size_t i = 0; i < stashSize; i++) {
            if (curNode.occupied == curNode.size) {
                if (debugMode) {
                    std::cout << "cur node is full, cannot place more blocks." << std::endl;
                }
                break;
            }
            Block stashBlock = std::move(stash.front());
            stash.pop_front();
            if (isSamePath(evictPathID, positionMap[stashBlock.originalPosition])) {
                if (debugMode) {
                    std::cout << "Evict block from stash to curNode[" << evictPathID << "]"<< std::endl;
                }
                curNode.put(stashBlock);
            } else {
                if (debugMode) {
                    std::cout << "Block[" << i << "] is not on the same path or is dummy data, skipping." << std::endl;
                }
                stash.push_back(std::move(stashBlock));
            }
        }
        evictPathID = getParent(evictPathID);
    }
    size_t stashSize = stash.size();
    Node& curNode = nodes[evictPathID];
    for (size_t i = 0; i < stashSize; i++) {
        if (curNode.occupied == curNode.size) {
            if (debugMode) {
                std::cout << "cur node is full, cannot place more blocks." << std::endl;
            }
            break;
        }
        Block stashBlock = std::move(stash.front());
        stash.pop_front();
        if (isSamePath(evictPathID, positionMap[stashBlock.originalPosition])) {
            if (debugMode) {
                std::cout << "Evict block from stash to curNode[" << evictPathID << "]"<< std::endl;
            }
            curNode.put(stashBlock);
        } else {
            if (debugMode) {
                std::cout << "Block[" << i << "] is not on the same path or is dummy data, skipping." << std::endl;
            }
            stash.push_back(std::move(stashBlock));
        }
    }

    if (op == Operation::READ) {
        return returnValue;
    } else {
        return std::nullopt;
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
