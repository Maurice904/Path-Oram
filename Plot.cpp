#include "Plot.h"
#include "Forest.h"
#include "StandardPathORAM.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <memory>
#include <iomanip>

namespace PathORAMPlot {
    
    const int DEFAULT_BUCKET_SIZE = 4;

    void plotStoreBenchmark(bool useStandardFlag) {
        std::vector<std::string> storeFiles = {
            "100kstore", "200kstore", "300kstore", 
            "400kstore", "500kstore", "1mstore"
        };
        
        std::vector<BenchmarkResult> results;
        
        std::cout << "\n=== PLOT STORE BENCHMARK ===" << std::endl;
        std::cout << "Algorithm: " << (useStandardFlag ? "StandardPathORAM" : "Forest/Tree") << std::endl;
        std::cout << "Testing files: ";
        for (size_t i = 0; i < storeFiles.size(); ++i) {
            std::cout << storeFiles[i];
            if (i < storeFiles.size() - 1) std::cout << ", ";
        }
        std::cout << "\n" << std::endl;
        
        for (const auto& fileName : storeFiles) {
            std::cout << "Processing " << fileName << "..." << std::endl;
            
            std::ifstream inputFile(fileName);
            if (!inputFile) {
                std::cerr << "Warning: Could not open file " << fileName << ", skipping..." << std::endl;
                continue;
            }
            
            std::vector<std::pair<int, int>> data;
            std::string line;
            while (std::getline(inputFile, line)) {
                std::istringstream lineStream(line);
                int position, value;
                if (lineStream >> position >> value) {
                    data.push_back({position, value});
                }
            }
            inputFile.close();
            
            if (data.empty()) {
                std::cerr << "Warning: No data found in " << fileName << ", skipping..." << std::endl;
                continue;
            }
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            std::unique_ptr<std_poram::ServerStorage> storage;
            std::unique_ptr<std_poram::StandardRandomGen> rng;
            std::unique_ptr<std_poram::StandardPathORAM> standardOram;
            Forest oramTrees(0);
            
            size_t maxStashSize = 0;
            size_t memoryUsage = 0;
            
            if (useStandardFlag) {
                size_t numBlocks = data.size();
                
                std_poram::ServerStorage::resetState();
                std_poram::Bucket::resetState();
                
                storage = std::make_unique<std_poram::ServerStorage>();
                rng = std::make_unique<std_poram::StandardRandomGen>();
                standardOram = std::make_unique<std_poram::StandardPathORAM>(storage.get(), rng.get(), DEFAULT_BUCKET_SIZE, numBlocks);
                
                for (const auto& item : data) {
                    int newData[std_poram::Block::BLOCK_SIZE] = { item.second, 0 };
                    int* result = standardOram->access(std_poram::StandardPathORAM::WRITE, item.first - 1, newData);
                    delete[] result;
                }
                
                maxStashSize = standardOram->getMaxStashSize();
                
                size_t positionMapSize = standardOram->getNumBlocks() * sizeof(int);
                size_t currentStashSize = standardOram->getStashSize();
                size_t stashMemorySize = currentStashSize * 64;
                size_t treeSize = standardOram->getNumBuckets() * 64;
                memoryUsage = (positionMapSize + stashMemorySize + treeSize) / 1024;
                
            } else {
                oramTrees = Forest(data.size(), BUCKET_SIZE);
                
                for (const auto& item : data) {
                    oramTrees.put(item.first, item.second, false);
                }
                
                maxStashSize = oramTrees.getMaxStashSize();
                
                size_t treeStructureSize = oramTrees.trees.size() * 1024;
                size_t storedDataSize = data.size() * sizeof(int) * 2;
                size_t positionMapSize = data.size() * sizeof(size_t) * 2;
                
                size_t totalStashSize = 0;
                for (const auto& tree : oramTrees.trees) {
                    totalStashSize += tree.stash.size();
                }
                size_t stashDataSize = totalStashSize * sizeof(int) * 4;
                
                memoryUsage = (treeStructureSize + storedDataSize + positionMapSize + stashDataSize) / 1024;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            BenchmarkResult result;
            result.fileName = fileName;
            result.dataSize = data.size();
            result.maxStashSize = maxStashSize;
            result.executionTime = duration.count();
            result.memoryUsage = memoryUsage;
            
            results.push_back(result);
            
            std::cout << "  Data size: " << data.size() << " blocks" << std::endl;
            std::cout << "  Max stash size: " << maxStashSize << std::endl;
            std::cout << "  Execution time: " << duration.count() << " ms" << std::endl;
            std::cout << "  Memory usage: " << memoryUsage << " KB" << std::endl;
            std::cout << std::endl;
        }
        
        std::string algorithmName = useStandardFlag ? "StandardPathORAM" : "Forest/Tree";
        printBenchmarkSummary(results, algorithmName);
    }

    void plotOpBenchmark(bool useStandardFlag) {
        std::vector<std::string> testSizes = {"100k", "200k", "300k", "400k", "500k", "1m"};
        std::vector<BenchmarkResult> results;
        
        std::cout << "\n=== PLOT OP BENCHMARK ===" << std::endl;
        std::cout << "Algorithm: " << (useStandardFlag ? "StandardPathORAM" : "Forest/Tree") << std::endl;
        std::cout << "Testing operation files: ";
        for (size_t i = 0; i < testSizes.size(); ++i) {
            std::cout << testSizes[i] << "op";
            if (i < testSizes.size() - 1) std::cout << ", ";
        }
        std::cout << "\n" << std::endl;
        
        for (const auto& size : testSizes) {
            std::string storeFile = size + "store";
            std::string opFile = size + "op";
            
            std::cout << "Processing " << opFile << " (requires " << storeFile << ")..." << std::endl;
            
            std::ifstream storeFileStream(storeFile);
            std::ifstream opFileStream(opFile);
            
            if (!storeFileStream) {
                std::cerr << "Warning: Could not open store file " << storeFile << ", skipping..." << std::endl;
                continue;
            }
            if (!opFileStream) {
                std::cerr << "Warning: Could not open operation file " << opFile << ", skipping..." << std::endl;
                continue;
            }
            
            std::vector<std::pair<int, int>> storeData;
            std::string line;
            while (std::getline(storeFileStream, line)) {
                std::istringstream lineStream(line);
                int position, value;
                if (lineStream >> position >> value) {
                    storeData.push_back({position, value});
                }
            }
            storeFileStream.close();
            
            if (storeData.empty()) {
                std::cerr << "Warning: No data found in " << storeFile << ", skipping..." << std::endl;
                continue;
            }
            
            std::vector<std::tuple<char, int, int>> operations;
            while (std::getline(opFileStream, line)) {
                std::istringstream lineStream(line);
                char op;
                int position;
                if (lineStream >> op >> position) {
                    if (op == 'W') {
                        int value;
                        if (lineStream >> value) {
                            operations.push_back(std::make_tuple(op, position, value));
                        }
                    } else if (op == 'R') {
                        operations.push_back(std::make_tuple(op, position, 0));
                    }
                }
            }
            opFileStream.close();
            
            if (operations.empty()) {
                std::cerr << "Warning: No operations found in " << opFile << ", skipping..." << std::endl;
                continue;
            }
            
            std::unique_ptr<std_poram::ServerStorage> storage;
            std::unique_ptr<std_poram::StandardRandomGen> rng;
            std::unique_ptr<std_poram::StandardPathORAM> standardOram;
            Forest oramTrees(0);
            
            size_t maxStashSize = 0;
            size_t memoryUsage = 0;
            

            if (useStandardFlag) {
                size_t numBlocks = storeData.size();
                
                std_poram::ServerStorage::resetState();
                std_poram::Bucket::resetState();
                
                storage = std::make_unique<std_poram::ServerStorage>();
                rng = std::make_unique<std_poram::StandardRandomGen>();
                standardOram = std::make_unique<std_poram::StandardPathORAM>(storage.get(), rng.get(), DEFAULT_BUCKET_SIZE, numBlocks);
                
                for (const auto& item : storeData) {
                    int newData[std_poram::Block::BLOCK_SIZE] = { item.second, 0 };
                    int* result = standardOram->access(std_poram::StandardPathORAM::WRITE, item.first - 1, newData);
                    delete[] result;
                }
            } else {
                oramTrees = Forest(storeData.size(), BUCKET_SIZE);
                
                for (const auto& item : storeData) {
                    oramTrees.put(item.first, item.second, false);
                }
            }
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (const auto& operation : operations) {
                char op = std::get<0>(operation);
                int position = std::get<1>(operation);
                int value = std::get<2>(operation);
                
                if (useStandardFlag) {
                    if (op == 'R') {
                        int* result = standardOram->access(std_poram::StandardPathORAM::READ, position - 1, nullptr);
                        delete[] result;
                    } else if (op == 'W') {
                        int newData[std_poram::Block::BLOCK_SIZE] = { value, 0 };
                        int* result = standardOram->access(std_poram::StandardPathORAM::WRITE, position - 1, newData);
                        delete[] result;
                    }
                } else {
                    if (op == 'R') {
                        oramTrees.get(position, false);
                    } else if (op == 'W') {
                        oramTrees.put(position, value, false);
                    }
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            if (useStandardFlag) {
                maxStashSize = standardOram->getMaxStashSize();
                
                size_t positionMapSize = standardOram->getNumBlocks() * sizeof(int);
                size_t currentStashSize = standardOram->getStashSize();
                size_t stashMemorySize = currentStashSize * 64;
                size_t treeSize = standardOram->getNumBuckets() * 64;
                memoryUsage = (positionMapSize + stashMemorySize + treeSize) / 1024;
            } else {
                maxStashSize = oramTrees.getMaxStashSize();
                
                size_t treeStructureSize = oramTrees.trees.size() * 1024;
                size_t storedDataSize = storeData.size() * sizeof(int) * 2;
                size_t positionMapSize = storeData.size() * sizeof(size_t) * 2;
                
                size_t totalStashSize = 0;
                for (const auto& tree : oramTrees.trees) {
                    totalStashSize += tree.stash.size();
                }
                size_t stashDataSize = totalStashSize * sizeof(int) * 4;
                
                memoryUsage = (treeStructureSize + storedDataSize + positionMapSize + stashDataSize) / 1024;
            }
            
            BenchmarkResult result;
            result.fileName = opFile;
            result.dataSize = operations.size();
            result.maxStashSize = maxStashSize;
            result.executionTime = duration.count();
            result.memoryUsage = memoryUsage;
            
            results.push_back(result);
            
            std::cout << "  Operations performed: " << operations.size() << std::endl;
            std::cout << "  Max stash size: " << maxStashSize << std::endl;
            std::cout << "  Execution time: " << duration.count() << " ms" << std::endl;
            std::cout << "  Memory usage: " << memoryUsage << " KB" << std::endl;
            std::cout << std::endl;
        }
        
        std::string algorithmName = useStandardFlag ? "StandardPathORAM" : "Forest/Tree";
        printOpBenchmarkSummary(results, algorithmName);
    }

    void printOpBenchmarkSummary(const std::vector<BenchmarkResult>& results, 
                                const std::string& algorithmName) {
        if (!results.empty()) {
            std::cout << "=== OPERATION BENCHMARK SUMMARY TABLE ===" << std::endl;
            std::cout << "Algorithm: " << algorithmName << std::endl;
            std::cout << std::left << std::setw(12) << "Op File" 
                      << std::setw(12) << "Operations" 
                      << std::setw(15) << "Max Stash" 
                      << std::setw(15) << "Time (ms)" 
                      << std::setw(15) << "Memory (KB)" << std::endl;
            std::cout << std::string(70, '-') << std::endl;
            
            for (const auto& result : results) {
                std::cout << std::left << std::setw(12) << result.fileName
                          << std::setw(12) << result.dataSize
                          << std::setw(15) << result.maxStashSize
                          << std::setw(15) << result.executionTime
                          << std::setw(15) << result.memoryUsage << std::endl;
            }
            std::cout << std::string(70, '=') << std::endl;
        } else {
            std::cout << "No operation benchmark data collected." << std::endl;
        }
    }

    void printBenchmarkSummary(const std::vector<BenchmarkResult>& results, 
                              const std::string& /* algorithmName */) {
        if (!results.empty()) {
            std::cout << "=== BENCHMARK SUMMARY TABLE ===" << std::endl;
            std::cout << std::left << std::setw(12) << "File" 
                      << std::setw(12) << "Data Size" 
                      << std::setw(15) << "Max Stash" 
                      << std::setw(15) << "Time (ms)" 
                      << std::setw(15) << "Memory (KB)" << std::endl;
            std::cout << std::string(70, '-') << std::endl;
            
            for (const auto& result : results) {
                std::cout << std::left << std::setw(12) << result.fileName
                          << std::setw(12) << result.dataSize
                          << std::setw(15) << result.maxStashSize
                          << std::setw(15) << result.executionTime
                          << std::setw(15) << result.memoryUsage << std::endl;
            }
            std::cout << std::string(70, '=') << std::endl;
        } else {
            std::cout << "No benchmark data collected." << std::endl;
        }
    }

}