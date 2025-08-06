#include "../src/Forest.h"
#include "../src/rgen.h"

#include <cassert>
#include <iostream>



void accessTest(size_t input_size, size_t bucket_size, size_t max_tree_size, std::optional<double> rratio = std::nullopt, bool rp_flag = false) {
    Forest forest(input_size, bucket_size, max_tree_size);
    std::vector<int> data_map(input_size);
    for (size_t i = 0; i < input_size; i ++) {
        int val = randomSizeT(0, INT_MAX);
        data_map[i] = val;
        forest.put(i, val, false, rratio, rp_flag);
    }

    for (size_t i = 0; i < input_size; i ++) {
        std::optional<int> retrieved_val = forest.get(i, false, rratio, rp_flag);
        if (retrieved_val.has_value()) {
            assert(retrieved_val.value() == data_map[i]);
        } else {
            std::cerr << "Value not found for key: " << i << std::endl;
        }
    }
    std::cout<<"stash size:"<<forest.getSizes() << std::endl;
}

int main() {
    std::cout << "Running access test with input size 10000, bucket size 4, max tree size 65535, no random read ratio, and ring path flag set to false." << std::endl;
    accessTest(10000, 4, MAX_TREE_SIZE, std::nullopt, false);
    std::cout << "Access test completed successfully." << std::endl;

    std::cout << "Running access test with input size 200000, bucket size 4, max tree size 65535, no random read ratio, and ring path flag set to false." << std::endl;
    accessTest(200000, 4, MAX_TREE_SIZE, std::nullopt, false);
    std::cout << "Access test completed successfully." << std::endl;

    std::cout << "Running access test with input size 200000, bucket size 4, max tree size 65535, random read ratio 0.5, and ring path flag set to false." << std::endl;
    accessTest(200000, 4, MAX_TREE_SIZE, 0.5, false);
    std::cout << "Access test completed successfully." << std::endl;

    std::cout<< "Running access test with input size 1000000, bucket size 4, max tree size 65535, no random read ratio, and ring path flag set to false." << std::endl;
    accessTest(1000000, 4, MAX_TREE_SIZE, std::nullopt, false);
    std::cout << "Access test completed successfully." << std::endl;


    std::cout<< "Running access test with input size 1000000, bucket size 4, max tree size 65535, no random read ratio, and ring path flag set to true." << std::endl;
    accessTest(1000000, 4, MAX_TREE_SIZE, std::nullopt, true);
    std::cout << "Access test completed successfully." << std::endl;

    return 0;
}