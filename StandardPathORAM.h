#ifndef STANDARD_PATH_ORAM_H
#define STANDARD_PATH_ORAM_H

#include <vector>
#include <random>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace std_poram {

class Block {
public:
    static const int BLOCK_SIZE = 2;  // Original project uses 2 integers per block
    int leaf_id;                      // Leaf assignment in binary tree
    int index;                        // Logical block index (-1 for dummy)
    int data[BLOCK_SIZE];             // Actual data payload

    Block();
    Block(int leaf_id, int index, int data[]);
    void printBlock();
    virtual ~Block();
};

class Bucket {
public:
    Bucket();
    Bucket(Bucket* other);
    Block getBlockByIndex(int index);
    void addBlock(Block new_blk);
    bool removeBlock(Block rm_blk);
    std::vector<Block> getBlocks();
    void printBlocks();
    
    static void setMaxSize(int maximumSize);
    static void resetState();
    static int getMaxSize();

private:
    static bool is_init;
    static int max_size;
    std::vector<Block> blocks;
};

class UntrustedStorageInterface {
public:
    virtual ~UntrustedStorageInterface() = default;
    
    virtual void setCapacity(int totalNumOfBuckets) = 0;
    virtual Bucket ReadBucket(int position) = 0;
    virtual void WriteBucket(int position, const Bucket& bucket_to_write) = 0;
};

class ServerStorage : public UntrustedStorageInterface {
public:
    static bool is_initialized;
    static bool is_capacity_set;
    std::vector<Bucket> buckets;
    
    ServerStorage();
    ~ServerStorage();
    void setCapacity(int totalNumOfBuckets) override;
    Bucket ReadBucket(int position) override;
    void WriteBucket(int position, const Bucket& bucket_to_write) override;
    
    static void resetState();

private:
    int capacity;
};

class RandForOramInterface {
public:
    virtual ~RandForOramInterface() = default;
    virtual void setBound(int exclusiveMax) = 0;
    virtual int getRandomLeaf() = 0;
};

class StandardRandomGen : public RandForOramInterface {
public:
    StandardRandomGen() : rng_(std::random_device{}()) {}
    
    void setBound(int exclusiveMax) override { 
        bound_ = exclusiveMax; 
    }
    
    int getRandomLeaf() override {
        if (bound_ <= 0) return 0;
        std::uniform_int_distribution<int> dist(0, bound_ - 1);
        return dist(rng_);
    }

private:
    std::mt19937 rng_;
    int bound_ = 1;
};

class StandardPathORAM {
public:
    enum Operation { READ, WRITE };
    
    StandardPathORAM(UntrustedStorageInterface* storage, 
                     RandForOramInterface* rand_gen,
                     int bucket_size, 
                     int num_blocks);
    
    int* access(Operation op, int blockIndex, int newdata[]);
    
    int P(int leaf, int level);
    
    int* getPositionMap();
    std::vector<Block> getStash();
    int getStashSize();
    int getMaxStashSize();
    int getNumLeaves();
    int getNumLevels();
    int getNumBlocks();
    int getNumBuckets();

private:
    UntrustedStorageInterface* storage_;
    RandForOramInterface* rand_gen_;
    
    int bucket_size_;
    int num_levels_;
    int num_leaves_;
    int num_blocks_;
    int num_buckets_;
    
    int* position_map_;
    std::vector<Block> stash_;
    int max_stash_size_;
};

}

#endif