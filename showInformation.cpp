#include "showInformation.h"
#include <iostream>
#include <iomanip>

void ShowInformation::showStatistics(bool useStandardOram,
                                   std::unique_ptr<std_poram::StandardPathORAM>& standardOram,
                                   const Forest& oramTrees,
                                   const std::chrono::high_resolution_clock::time_point& startTime,
                                   const std::chrono::high_resolution_clock::time_point& endTime,
                                   size_t dataSize,
                                   int opCount) {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "\n=== STATISTICS ===" << std::endl;
    std::cout << "Algorithm: " << (useStandardOram ? "StandardPathORAM" : "Forest/Tree") << std::endl;
    std::cout << "Total execution time: " << duration.count() << " ms" << std::endl;
    

    size_t currentStashSize = 0;
    if (useStandardOram) {
        currentStashSize = standardOram->getStashSize();
    } else {
        for (const auto& tree : oramTrees.trees) {
            currentStashSize += tree.stash.size();
        }
    }
    size_t memoryUsage = 0;
    if (useStandardOram) {
        memoryUsage = calculateStandardOramMemory(standardOram);
        std::cout << "Tree levels: " << standardOram->getNumLevels() << std::endl;
        std::cout << "Tree leaves: " << standardOram->getNumLeaves() << std::endl;
        std::cout << "Total buckets: " << standardOram->getNumBuckets() << std::endl;
        
        size_t positionMapSize = standardOram->getNumBlocks() * sizeof(int);
        std::cout << "Position map: ~" << positionMapSize / 1024 << " KB" << std::endl;
        std::cout << "Current stash size: " << currentStashSize << " blocks" << std::endl;
        std::cout << "Max stash size: " << standardOram->getMaxStashSize() << " blocks" << std::endl;
    } else {
        memoryUsage = calculateForestMemory(oramTrees, dataSize);
        
        std::cout << "Number of trees: " << oramTrees.trees.size() << std::endl;
        std::cout << "Current total stash size: " << currentStashSize << " items" << std::endl;
        std::cout << "Max total stash size: " << oramTrees.getMaxStashSize() << " items" << std::endl;
        
        size_t storedDataSize = dataSize * sizeof(int) * 2;
        std::cout << "Data storage: ~" << (storedDataSize < 1024 ? storedDataSize : storedDataSize / 1024) 
                  << (storedDataSize < 1024 ? " bytes" : " KB") << std::endl;
    }
    
    std::cout << "Estimated memory usage: " << memoryUsage / 1024 << " KB" << std::endl;
    
    if (opCount > 0) {
        std::cout << "Operations processed: " << opCount << std::endl;
        std::cout << "Average time per operation: " << (duration.count() / opCount) << " ms" << std::endl;
    } else {
        std::cout << "Data blocks stored: " << dataSize << std::endl;
    }
    
    std::cout << "=================" << std::endl;
}

void ShowInformation::showDebugInfo(bool useStandardOram,
                                  std::unique_ptr<std_poram::StandardPathORAM>& standardOram,
                                  const Forest& oramTrees,
                                  const std::string& prefix) {
    if (!prefix.empty()) {
        std::cout << prefix << " - ";
    }
    
    if (useStandardOram) {
        std::cout << "StandardPathORAM Debug Info:" << std::endl;
        std::cout << "  Current stash size: " << standardOram->getStashSize() << std::endl;
        std::cout << "  Max stash size: " << standardOram->getMaxStashSize() << std::endl;
        std::cout << "  Tree levels: " << standardOram->getNumLevels() << std::endl;
        std::cout << "  Number of leaves: " << standardOram->getNumLeaves() << std::endl;
        std::cout << "  Number of buckets: " << standardOram->getNumBuckets() << std::endl;
        std::cout << "  Number of blocks: " << standardOram->getNumBlocks() << std::endl;
    } else {
        std::cout << "Forest/Tree Debug Info:" << std::endl;
        size_t currentStashSize = 0;
        for (const auto& tree : oramTrees.trees) {
            currentStashSize += tree.stash.size();
        }
        
        std::cout << "  Number of trees: " << oramTrees.trees.size() << std::endl;
        std::cout << "  Current total stash size: " << currentStashSize << " items" << std::endl;
        std::cout << "  Max total stash size: " << oramTrees.getMaxStashSize() << " items" << std::endl;
        std::cout << "  Position range: 1-" << oramTrees.getPosRange() - 1 << std::endl;
        std::cout << oramTrees.toString() << std::endl;
    }
}

std::string ShowInformation::getTreeInfo(bool useStandardOram,
                                        std::unique_ptr<std_poram::StandardPathORAM>& standardOram,
                                        const Forest& oramTrees) {
    if (useStandardOram) {
        std::string info = "StandardPathORAM Info:\n";
        info += "Stash size: " + std::to_string(standardOram->getStashSize()) + "\n";
        info += "Max stash size: " + std::to_string(standardOram->getMaxStashSize()) + "\n";
        info += "Number of leaves: " + std::to_string(standardOram->getNumLeaves()) + "\n";
        info += "Number of levels: " + std::to_string(standardOram->getNumLevels()) + "\n";
        info += "Number of buckets: " + std::to_string(standardOram->getNumBuckets()) + "\n";
        info += "Number of blocks: " + std::to_string(standardOram->getNumBlocks()) + "\n";
        return info;
    } else {
        size_t currentStashSize = 0;
        for (const auto& tree : oramTrees.trees) {
            currentStashSize += tree.stash.size();
        }
        
        std::string info = "Forest/Tree Info:\n";
        info += "Number of trees: " + std::to_string(oramTrees.trees.size()) + "\n";
        info += "Current total stash size: " + std::to_string(currentStashSize) + " items\n";
        info += "Max total stash size: " + std::to_string(oramTrees.getMaxStashSize()) + " items\n";
        info += "Position range: 1-" + std::to_string(oramTrees.getPosRange() - 1) + "\n\n";
        info += oramTrees.toString();
        return info;
    }
}

std::string ShowInformation::getSizeInfo(bool useStandardOram,
                                       std::unique_ptr<std_poram::StandardPathORAM>& standardOram,
                                       const Forest& oramTrees) {
    if (useStandardOram) {
        std::string info = "StandardPathORAM has " + std::to_string(standardOram->getNumBuckets()) + " buckets.\n";
        info += "Stash size: " + std::to_string(standardOram->getStashSize()) + " blocks\n";
        info += "Max stash size: " + std::to_string(standardOram->getMaxStashSize()) + " blocks";
        return info;
    } else {
        size_t currentStashSize = 0;
        for (const auto& tree : oramTrees.trees) {
            currentStashSize += tree.stash.size();
        }
        
        std::string info = "Forest has " + std::to_string(oramTrees.trees.size()) + " trees.\n";
        info += "Current total stash size: " + std::to_string(currentStashSize) + " items\n";
        info += "Max total stash size: " + std::to_string(oramTrees.getMaxStashSize()) + " items\n";
        info += oramTrees.getSizes();
        return info;
    }
}

std::string ShowInformation::getPositionRangeInfo(bool useStandardOram,
                                                std::unique_ptr<std_poram::StandardPathORAM>& standardOram,
                                                const Forest& oramTrees) {
    if (useStandardOram) {
        return "Position range: 0-" + std::to_string(standardOram->getNumBlocks() - 1);
    } else {
        return "Position range: 1-" + std::to_string(oramTrees.getPosRange() - 1);
    }
}

size_t ShowInformation::calculateStandardOramMemory(std::unique_ptr<std_poram::StandardPathORAM>& standardOram) {
    size_t positionMapSize = standardOram->getNumBlocks() * sizeof(int);
    size_t currentStashSize = standardOram->getStashSize();
    size_t stashMemorySize = currentStashSize * 64;
    size_t treeSize = standardOram->getNumBuckets() * 64;
    
    return positionMapSize + stashMemorySize + treeSize;
}

size_t ShowInformation::calculateForestMemory(const Forest& oramTrees, size_t dataSize) {
    size_t treeStructureSize = oramTrees.trees.size() * 1024;
    size_t storedDataSize = dataSize * sizeof(int) * 2;
    size_t positionMapSize = dataSize * sizeof(size_t) * 2;
    
    size_t totalStashSize = 0;
    for (const auto& tree : oramTrees.trees) {
        totalStashSize += tree.stash.size();
    }
    size_t stashDataSize = totalStashSize * sizeof(int) * 4;
    
    return treeStructureSize + storedDataSize + positionMapSize + stashDataSize;
}