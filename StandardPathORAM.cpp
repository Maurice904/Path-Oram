#include "StandardPathORAM.h"
#include <iostream>
#include <string>
#include <sstream>

namespace std_poram {


Block::Block() {
    this->leaf_id = -1;
    this->index = -1;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        this->data[i] = 0;
    }
}

Block::Block(int leaf_id, int index, int data[]) : leaf_id(leaf_id), index(index) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        this->data[i] = data[i];
    }
}

Block::~Block() {
}

void Block::printBlock() {
    std::string data_holder = "";
    for (int i = 0; i < BLOCK_SIZE; i++) {
        data_holder += std::to_string(this->data[i]);
        data_holder += " ";
    }
    std::cout << "index: " << std::to_string(this->index) 
              << " leaf id: " << std::to_string(this->leaf_id) 
              << " data: " << data_holder << std::endl;
}


bool Bucket::is_init = false;
int Bucket::max_size = -1;

Bucket::Bucket() {
    if (!is_init) {
        throw std::runtime_error("Please set bucket size before creating a bucket");
    }
    blocks = std::vector<Block>();
}

Bucket::Bucket(Bucket* other) {
    if (other == nullptr) {
        throw std::runtime_error("the other bucket is not allocated.");
    }
    blocks = std::vector<Block>(max_size);
    for (int i = 0; i < max_size; i++) {
        blocks[i] = Block(other->blocks[i]);
    }
}

Block Bucket::getBlockByIndex(int index) {
    Block* copy_block = nullptr;
    for (Block b : blocks) {
        if (b.index == index) {
            copy_block = new Block(b);
            break;
        }
    }
    if (copy_block == nullptr) {
        throw std::runtime_error("Block with index " + std::to_string(index) + " not found");
    }
    Block result = *copy_block;
    delete copy_block;
    return result;
}

void Bucket::addBlock(Block new_blk) {
    if (static_cast<int>(blocks.size()) < max_size) {
        Block toAdd = Block(new_blk);
        blocks.push_back(toAdd);
    }
}

bool Bucket::removeBlock(Block rm_blk) {
    bool removed = false;
    for (int i = 0; i < static_cast<int>(blocks.size()); i++) {
        if (blocks[i].index == rm_blk.index) {
            blocks.erase(blocks.begin() + i);
            removed = true;
            break;
        }
    }
    return removed;
}

std::vector<Block> Bucket::getBlocks() {
    return this->blocks;
}

void Bucket::setMaxSize(int maximumSize) {
    if (is_init == true) {
        throw std::runtime_error("Max Bucket Size was already set");
    }
    max_size = maximumSize;
    is_init = true;
}

int Bucket::getMaxSize() {
    return max_size;
}

void Bucket::resetState() {
    is_init = false;
}

void Bucket::printBlocks() {
    for (Block b : blocks) {
        b.printBlock();
    }
}


bool ServerStorage::is_initialized = false;
bool ServerStorage::is_capacity_set = false;

ServerStorage::ServerStorage() {
    if (this->is_initialized) {
        throw std::runtime_error("ONLY ONE ServerStorage CAN BE USED AT A TIME IN THIS IMPLEMENTATION");
    }
    this->is_initialized = true;
}

ServerStorage::~ServerStorage() {
    is_initialized = false;
    is_capacity_set = false;
}

void ServerStorage::resetState() {
    is_initialized = false;
    is_capacity_set = false;
}

void ServerStorage::setCapacity(int totalNumOfBuckets) {
    if (this->is_capacity_set) {
        throw std::runtime_error("Capacity of ServerStorage cannot be changed");
    }
    this->is_capacity_set = true;
    this->capacity = totalNumOfBuckets;
    this->buckets.assign(totalNumOfBuckets, Bucket());
}

Bucket ServerStorage::ReadBucket(int position) {
    if (!this->is_capacity_set) {
        throw std::runtime_error("Please call setCapacity before reading or writing any block");
    }
    
    if (position >= this->capacity || position < 0) {
        throw std::runtime_error("You are trying to access Bucket " + std::to_string(position) + 
                                ", but this Server contains only " + std::to_string(this->capacity) + " buckets.");
    }
    return this->buckets.at(position);
}

void ServerStorage::WriteBucket(int position, const Bucket& bucket_to_write) {
    if (!this->is_capacity_set) {
        throw std::runtime_error("Please call setCapacity before reading or writing any block");
    }
    
    if (position >= this->capacity || position < 0) {
        throw std::runtime_error("You are trying to access Bucket " + std::to_string(position) + 
                                ", but this Server contains only " + std::to_string(this->capacity) + " buckets.");
    }
    
    this->buckets.at(position) = bucket_to_write;
}


StandardPathORAM::StandardPathORAM(UntrustedStorageInterface* storage, 
                                   RandForOramInterface* rand_gen,
                                   int bucket_size, 
                                   int num_blocks) 
    : storage_(storage), rand_gen_(rand_gen), bucket_size_(bucket_size), num_blocks_(num_blocks) {
    
    // Calculate tree parameters
    this->num_levels_ = static_cast<int>(std::ceil(std::log10(num_blocks_) / std::log10(2))) + 1;
    this->num_buckets_ = static_cast<int>(std::pow(2, num_levels_)) - 1;
    this->num_leaves_ = static_cast<int>(std::pow(2, num_levels_ - 1));
    
    // Verify capacity
    if (this->num_buckets_ * this->bucket_size_ < this->num_blocks_) {
        throw std::runtime_error("Not enough space for the actual number of blocks.");
    }
    
    // Initialize bucket system
    Bucket::resetState();
    Bucket::setMaxSize(bucket_size_);
    this->rand_gen_->setBound(num_leaves_);
    this->storage_->setCapacity(num_buckets_);
    
    // Initialize position map
    this->position_map_ = new int[this->num_blocks_];
    this->stash_ = std::vector<Block>();
    this->max_stash_size_ = 0;  // Initialize maximum stash size tracker
    
    for (int i = 0; i < this->num_blocks_; i++) {
        position_map_[i] = rand_gen_->getRandomLeaf();
    }
    
    // Initialize all buckets in storage with empty blocks
    for (int i = 0; i < num_buckets_; i++) {
        Bucket init_bkt = Bucket();
        for (int j = 0; j < bucket_size_; j++) {
            init_bkt.addBlock(Block());
        }
        storage_->WriteBucket(i, init_bkt);
    }
}

int* StandardPathORAM::access(Operation op, int blockIndex, int newdata[]) {
    int* data = new int[Block::BLOCK_SIZE];
    
    // Step 1: Read the path from root to leaf
    int oldLeaf = position_map_[blockIndex];
    position_map_[blockIndex] = rand_gen_->getRandomLeaf();
    
    Block* targetBlock = nullptr;
    
    // Read entire path and move all real blocks to stash
    for (int i = 0; i < num_levels_; i++) {
        std::vector<Block> blocks = storage_->ReadBucket(P(oldLeaf, i)).getBlocks();
        Bucket write_back = Bucket();
        
        for (Block b : blocks) {
            if (b.index != -1) {
                if (b.index == blockIndex) {
                    targetBlock = new Block(b);
                }
                stash_.push_back(Block(b));
            }
            write_back.addBlock(Block());
        }
        storage_->WriteBucket(P(oldLeaf, i), write_back);
    }
    
    if (op == Operation::WRITE) {
        if (targetBlock == nullptr) {
            Block newBlock = Block(position_map_[blockIndex], blockIndex, newdata);
            stash_.push_back(newBlock);
        } else {
            for (int i = 0; i < Block::BLOCK_SIZE; i++) {
                targetBlock->data[i] = newdata[i];
            }
            targetBlock->leaf_id = position_map_[blockIndex];
        }
    } else {
        if (targetBlock == nullptr) {
            data = nullptr;
        } else {
            for (int i = 0; i < Block::BLOCK_SIZE; i++) {
                data[i] = targetBlock->data[i];
            }
        }
    }
    
    int current_stash_size = static_cast<int>(stash_.size());
    if (current_stash_size > max_stash_size_) {
        max_stash_size_ = current_stash_size;
    }
    
    for (int l = num_levels_ - 1; l >= 0; l--) {
        std::vector<int> bid_evicted = std::vector<int>();
        Bucket bucket = Bucket();
        int Pxl = P(oldLeaf, l);
        int counter = 0;
        
        for (Block b_instash : stash_) {
            if (counter >= bucket_size_) {
                break;
            }
            Block be_evicted = Block(b_instash);
            if (Pxl == P(position_map_[be_evicted.index], l)) {
                bucket.addBlock(be_evicted);
                bid_evicted.push_back(be_evicted.index);
                counter++;
            }
        }
        
        for (int i = 0; i < static_cast<int>(bid_evicted.size()); i++) {
            for (int j = 0; j < static_cast<int>(stash_.size()); j++) {
                Block b_instash = stash_.at(j);
                if (b_instash.index == bid_evicted.at(i)) {
                    this->stash_.erase(this->stash_.begin() + j);
                    break;
                }
            }
        }
        
        while (counter < bucket_size_) {
            bucket.addBlock(Block());
            counter++;
        }
        storage_->WriteBucket(Pxl, bucket);
    }
    
    if (targetBlock) {
        delete targetBlock;
    }
    
    return data;
}

int StandardPathORAM::P(int leaf, int level) {
    return (1 << level) - 1 + (leaf >> (this->num_levels_ - level - 1));
}

int* StandardPathORAM::getPositionMap() {
    return this->position_map_;
}

std::vector<Block> StandardPathORAM::getStash() {
    return this->stash_;
}

int StandardPathORAM::getStashSize() {
    return static_cast<int>(this->stash_.size());
}

int StandardPathORAM::getMaxStashSize() {
    return this->max_stash_size_;
}

int StandardPathORAM::getNumLeaves() {
    return this->num_leaves_;
}

int StandardPathORAM::getNumLevels() {
    return this->num_levels_;
}

int StandardPathORAM::getNumBlocks() {
    return this->num_blocks_;
}

int StandardPathORAM::getNumBuckets() {
    return this->num_buckets_;
}

}