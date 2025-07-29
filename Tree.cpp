#include "Tree.h"

int randomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}


Block::Block() : value(randomInt(INT_MIN, INT_MAX)), originalPosition(randomInt(INT_MIN, INT_MAX)), isDummy(true) {}

Block::Block(int value, uint32_t originalPosition, bool isDummy) 
    : value(value), originalPosition(originalPosition), isDummy(isDummy) {}

std::string Block::toString() const {
    if (isDummy) {
        return "[DUMMY]";
    }
    return "[Pos:" + std::to_string(originalPosition) + ", Val:" + std::to_string(value) + "]";
}

Node::Node(uint32_t bucketSize) : buckets(bucketSize, Block(randomInt(INT_MIN, INT_MAX), randomInt(INT_MIN, INT_MAX), true)), occupied(0), size(bucketSize) {}

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

void Node::remove(uint32_t index) {
    if (index < occupied) {
        buckets[index] = Block();
        occupied--;
    } else {
        throw std::out_of_range("Invalid index");
    }
}

std::string Node::toString() const {
    std::string result = "Node(occupied:" + std::to_string(occupied) + "/" + std::to_string(size) + ") [";
    for (uint32_t i = 0; i < buckets.size(); i++) {
        if (i > 0) result += ", ";
        result += buckets[i].toString();
    }
    result += "]";
    return result;
}

Tree::Tree(uint32_t nodeCount, uint32_t bucketSize) {
    uint32_t size = 0;
    treeLevel = 0;
    while (nodeCount > size) {
        size = (size << 1) | 1;
        treeLevel++;
    }
    for (uint32_t i = 0; i < size; i ++) {
        nodes.emplace_back(Node(bucketSize));
    }
    leafStartIndex = size/2;
}

uint32_t Tree::getParent(uint32_t children) const {
    if (children == 0) {
        return 0;
    }
    return (children - 1) / 2;
}

// read all nodes only，for cut the path
std::vector<uint32_t> Tree::getPath(uint32_t leafID) const {
    std::vector<uint32_t> path;
    for (uint32_t node = leafID; ; node = getParent(node)) {
        path.push_back(node);
        if (node == 0) break;
    }
    return path;
}

void Tree::readFromPath(uint32_t pathID) {
    uint32_t curNode = pathID;
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

// only read until half (nodeID would be half)
void Tree::readFromNodes(const std::vector<uint32_t>& nodeIDs) {
    for (auto node : nodeIDs) {
        for (auto& blk : nodes[node].buckets) {
            if (!blk.isDummy) stash.push_back(std::move(blk));
        }
        nodes[node].clear();
    }
}

bool Tree::isSamePath(uint32_t curNode, uint32_t leafNode) const {
    while (leafNode > curNode) {
        leafNode >>= 1;
    }
    return leafNode == curNode;
}

std::optional<int> Tree::access(Operation op, uint32_t position, int value, bool debugMode) {
    uint32_t prevPath;
    if (positionMap.count(position)) {
        prevPath = positionMap[position];
        readFromPath(prevPath);
    } else {
        prevPath = randomInt((int)leafStartIndex, (int)nodes.size() - 1);
        readFromPath(prevPath);
        stash.push_back(Block(value, position, false));
    }

    // read the first half part
    auto fullPath = getPath(prevPath);
    size_t half = fullPath.size() / 2;
    readFromNodes(std::vector<uint32_t>(fullPath.begin(), fullPath.begin() + half));

    // check if it is in the first half part
    bool found = false;
    for (auto& b : stash) {
        if (b.originalPosition == position) { found = true; break; }
    }

    if (!found) {
        readFromNodes(std::vector<uint32_t>(fullPath.begin() + half, fullPath.end()));
    }

    size_t newPath = randomInt((int)leafStartIndex, (int)nodes.size() - 1);
    if (debugMode) std::cout<<"newPath: "<<newPath<<std::endl;
    positionMap[position] = newPath;
    int returnValue = 0;
    for (auto& block : stash) {
        if (block.originalPosition == position) {
            if (op == READ) returnValue = block.value;
            else block.value = value;
            break;
        }
    }
    size_t evictID = randomInt((int)leafStartIndex, (int)nodes.size() - 1);
    while (evictID > 0) {
        Node& node = nodes[evictID];
        size_t stashSize = stash.size();
        for (size_t i = 0; i < stashSize && node.occupied < node.buckets.size(); ++i) {
            Block blk = std::move(stash.front()); stash.pop_front();
            if (isSamePath(evictID, positionMap[blk.originalPosition])) node.put(blk);
            else stash.push_back(std::move(blk));
        }
        evictID = getParent(evictID);
    }
    Node& lastNode = nodes[evictID];
    size_t stashSize = stash.size();
    for (size_t i = 0; i < stashSize && lastNode.occupied < lastNode.buckets.size(); ++i) {
        Block blk = std::move(stash.front()); stash.pop_front();
        if (isSamePath(evictID, positionMap[blk.originalPosition])) lastNode.put(blk);
        else stash.push_back(std::move(blk));
    }
    if (op == READ) {
        return returnValue;
    } else {
        return std::nullopt;
    }

}


std::string Tree::toString() const {
    std::string result = "Tree(levels:" + std::to_string(treeLevel) + 
                        ", leafStart:" + std::to_string(leafStartIndex) + ")\n";

    result += "Nodes:\n";
    for (uint32_t i = 0; i < nodes.size(); i++) {
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
