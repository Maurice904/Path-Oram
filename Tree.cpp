#include "Tree.h"

int randomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}


struct Block {
    int value;
    size_t originalPosition;
    bool isDummy;
    Block(): value(randomInt(INT_MIN, INT_MAX)), originalPosition(randomInt(INT_MIN, INT_MAX)), isDummy(true) {}
    Block(int value, size_t originalPosition, bool isDummy = false) 
        : value(value), originalPosition(originalPosition), isDummy(isDummy) {}
};

class Node {
    public:
        std::vector<Block> buckets;
        size_t occupied = 0;
        size_t size;
        bool isData;

        Node(size_t bucketSize = 4) : buckets(bucketSize, Block(0, 0)) {
            size = bucketSize;
            for (size_t i = 0; i < size; i++) {
                buckets[i] = Block(randomInt(INT_MIN, INT_MAX),randomInt(INT_MIN, INT_MAX), true);
            }
        }

        void clear() {
            for (auto& block : buckets) {
                block = Block(randomInt(INT_MIN, INT_MAX),randomInt(INT_MIN, INT_MAX), true);
            }
            occupied = 0;
        }

        void put(Block& block) {
            if (occupied >= size) {
                throw std::runtime_error("buckets is full");
            } else {
                buckets[occupied] = std::move(block);
                occupied++;
            }
        }

        void remove(size_t index) {
            if (index < occupied) {
                buckets[index] = Block();
                occupied--;
            } else {
                throw std::out_of_range("Invalid index");
            }
        }

};

class Tree {
    public:
        std::vector<Node> nodes;
        std::unordered_map<size_t, size_t> positionMap;
        std::deque<Block> stash;
        size_t leafStartIndex;
        size_t treeLevel;

        Tree(size_t nodeCount, size_t bucketSize = 4) {
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

        size_t getParent(size_t children) {
            if (children == 0) {
                return 0;
            }
            return (children - 1) / 2;
        }

        void readFromPath(size_t pathID) {
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
            curNode = getParent(curNode);
        }

        bool isSamePath(size_t curNode, size_t leafNode) {
            while (leafNode > curNode) {
                leafNode >>= 1;
            }
            return leafNode == curNode;
        }

        std::optional<int> access(Operation op, size_t position, int value = 0) {
            if (positionMap.find(position) != positionMap.end()) {
                size_t prevPath = positionMap[position];
                readFromPath(prevPath);
            } else {
                readFromPath(randomInt(leafStartIndex, nodes.size() - 1));
                stash.push_back(Block(value, position, false));
            }

            size_t newPath = randomInt(leafStartIndex, nodes.size() - 1);
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
            size_t evictPathID = randomInt(leafStartIndex, nodes.size() - 1);
            while (evictPathID > 0) {
                Node& parentNode = nodes[getParent(evictPathID)];
                Node& curNode = nodes[evictPathID];
                // propagate the block towards the leaf
                for (size_t i = 0; i < parentNode.size; i++) {
                    if (parentNode.occupied == parentNode.size) {
                        break;
                    }
                    if (!parentNode.buckets[i].isDummy && isSamePath(evictPathID, parentNode.buckets[i].originalPosition)) {
                        curNode.put(parentNode.buckets[i]);
                        parentNode.remove(i);
                    }
                }
                evictPathID = getParent(evictPathID);
            }
            for (size_t i = 0; i < stash.size(); i ++) {
                if (nodes[0].occupied == nodes[0].size) {
                    break;
                }
                nodes[0].put(stash.front());
                stash.pop_front();
            }


            if (op == Operation::READ) {
                return returnValue;
            } else {
                return std::nullopt;
            }
        }
};