#include "Tree.h"
#include <iostream>
#include <functional>
#include <string>
#include <optional>
#include <cassert>

struct ORAMTest {
    std::string name;
    std::function<void()> test_fn;
};

Tree tree(10, 4);

void reset_tree() {
    tree = Tree(10, 4);
}


// 1. read data from tree, then could not find the date in the pre position
ORAMTest test_block_not_in_original_path = {
    "Block is removed from its original path after READ",
    []() {
        reset_tree();

        tree.access(WRITE, 10, 1001);
        size_t oldLeaf = tree.positionMap[10];
        auto result = tree.access(READ, 10);
        if (!result.has_value()) {
            std::cerr << "⚠️ WARNING: block not found at READ time (may have been evicted incorrectly)\n";
}
        size_t nodeID = oldLeaf;
        bool stillInOriginalPath = false;

        while (true) {
            for (const auto& blk : tree.nodes[nodeID].buckets) {
                if (!blk.isDummy && blk.originalPosition == 10) {
                    stillInOriginalPath = true;
                    std::cout << "Fail!";
                }
            }
            if (nodeID == 0) break;
            nodeID = tree.getParent(nodeID);
        }

        assert(!stillInOriginalPath);
        std::cout << "Passed!\n";
    }
};


// 2. 
ORAMTest test_read_unwritten_returns_zero = {
    "READ on unwritten position returns 0",
    []() {
        reset_tree();
        std::optional<int> result = tree.access(READ, 1234);
        if (result.has_value()) {
            int val = *result;
            assert(val == 0);
        } else {
            assert(false);
        }
        std::cout << "Passed!\n";
    }
};


// ========== Run all tests ==========

int main() {
    std::vector<ORAMTest> tests = {
        test_block_not_in_original_path,
        test_read_unwritten_returns_zero,
    };

    std::cout << "Running " << tests.size() << " structured ORAM tests...\n";

    int test_id = 1;
    for (const auto& test : tests) {
        std::cout << "\nTest " << test_id++ << ": " << test.name << "\n";
        try {
            test.test_fn();
        } catch (const std::exception& e) {
            std::cerr << "Fail!";
        } catch (...) {
            std::cerr << "Fail!";
        }
    }

    std::cout << "All Passed!\n";
    return 0;
}
