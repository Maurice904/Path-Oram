#include "Forest.h"

#include <fstream>
#include <iostream>
#include <string>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <chrono>

// use -d to see debug information and use -s to see statistics related with run time and stash sizes
//command format:  store|operate <file_name> [--r <random read ratio>] [-d] [--max-size <max_tree_size>] [-rp] ,
//                 print sizes|trees [output_file] ,
//                 get <position> [--r <random read ratio>] [-d] [-rp],
//                 put <position> <value> [--r <random read ratio>] [-d] [-rp] ,
//                 newTree <data_size> <bucket_size> <max_tree_size> [-d] ,
//                 exit ,
//==================================================================================
// How to use different optimizations:
//1. use --max-size to set the maximum size of each tree, default is 65535,
// the data will be distributed across multiple trees if the data size exceeds the maximum size.
// this optimization is used by default, so set a large number larger than the data you want to store to disable it.
//==================================================================================
//2. use -rp flag to enable ring oram features, default is false
//==================================================================================
//3. use --r <random read ratio> to enable random read algorithm, default is disabled,
// the random read ratio should be between 0 and 1 and represent the probability a read block along the path is put into the stash




// test files format:
// store: <position> <value>
//===============================
// operate: R <position> , or W <position> <value>
// where R is read operation and W is write operation


void writeFileTo(const std::string& fileName, const std::string& content) {
    std::ofstream outputFile(fileName);
    if (outputFile.is_open()) {
        outputFile << content;
        outputFile.close();
        std::cout << "Content written to " << fileName << std::endl;
    } else {
        std::cerr << "Error opening file: " << fileName << std::endl;
    }
}

size_t parseMaxTreeSize(std::vector<std::string>& args) {
    auto it = std::find(args.begin(), args.end(), "--max-size");
    if (it != args.end()) {
        if (it + 1 != args.end()) {
            try {
                size_t maxSize = std::stoul(*(it + 1));
                args.erase(it + 1);
                args.erase(it);
                return maxSize;
            } catch (const std::exception& e) {
                std::cerr << "Invalid max tree size format." << std::endl;
                args.erase(it + 1);
                args.erase(it);
                return MAX_TREE_SIZE; 
            }
        } else {
            std::cerr << "Missing value for --max-tree-size parameter." << std::endl;
            args.erase(it);
            return MAX_TREE_SIZE;
        }
    }
    return MAX_TREE_SIZE;
}


std::optional<double> parseRandomReadRatio(std::vector<std::string>& args) {
    auto it = std::find(args.begin(), args.end(), "--r");
    if (it != args.end()) {
        if (it + 1 != args.end()) {
            try {
                double ratio = std::stod(*(it + 1));
                if (ratio >= 0 && ratio <= 1) {
                    args.erase(it + 1);
                    args.erase(it);
                    return ratio;
                } else {
                    std::cerr << "Random read ratio must be between 0 and 1." << std::endl;
                    args.erase(it + 1);
                    args.erase(it);
                    return std::nullopt;
                }
            } catch (const std::exception& e) {
                std::cerr << "Invalid random read ratio format." << std::endl;
                args.erase(it + 1);
                args.erase(it);
                return std::nullopt;
            }
        } else {
            std::cerr << "Missing value for --r parameter." << std::endl;
            args.erase(it);
            return std::nullopt;
        }
    }
    return std::nullopt;
}

int main() {
    Forest oramTrees(0);
    bool loaded = false;
    while (true) {
        std::string command;
        std::cout<< "Enter command: ";
        std::getline(std::cin, command);
        if (command.empty()) {
            continue;
        }
        std::vector<std::string> args;
        std::istringstream iss(command);
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }

        std::optional<double> randomReadRatio = parseRandomReadRatio(args);
        size_t maxTreeSize = parseMaxTreeSize(args);
        bool debugMode = false;
        bool statsMode = false;
        bool ringFlag = false;
        auto debugIt = std::find(args.begin(), args.end(), "-d");
        if (debugIt != args.end()) {
            debugMode = true;
            args.erase(debugIt); 
        }
        auto statsIt = std::find(args.begin(), args.end(), "-s");
        if (statsIt != args.end()) {
            statsMode = true;
            args.erase(statsIt); 
        }
        auto ringIt = std::find(args.begin(), args.end(), "-rp");
        if (ringIt != args.end()) {
            ringFlag = true;
            args.erase(ringIt);
        }
        if (args.empty()) {
            continue;
        }

        if (args[0] == "exit") {
            break;
        } else if (args[0] == "newTree") {
            if (args.size() < 4) {
                std::cerr << "Usage: newTree <data_size> <bucket_size> <max_tree_size>" << std::endl;
                continue;
            }
            size_t dataSize;
            size_t bucketSize;
            size_t maxSize;
            try {
                dataSize = std::stoul(args[1]);
                bucketSize = std::stoul(args[2]);
                maxSize = std::stoul(args[3]);
            } catch (const std::exception& e) {
                std::cerr << "Invalid data size, bucket size or max tree size format." << std::endl;
                continue;
            }
            oramTrees = Forest(dataSize, bucketSize, maxSize);
            loaded = true;
            std::cout<< "New forest created with " << oramTrees.trees.size() << " trees." << std::endl;
        } else if (args[0] == "store") {
            if (args.size() < 2) {
                std::cerr << "Usage: store <file_name>" << std::endl;
                continue;
            }
            std::string fileName = args[1];
            int bucketSize = 4;
            if (args.size() > 2) {
                try {
                    bucketSize = std::stoi(args[2]);
                } catch (const std::exception& e) {
                    std::cerr << "Invalid bucket size format: " << args[2] << std::endl;
                    continue;
                }
            }
            std::ifstream inputFile(fileName);
            if (!inputFile) {
                std::cerr << "Error opening file: " << fileName << std::endl;
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
            oramTrees = Forest(data.size(), bucketSize, maxTreeSize);
            size_t position = 0;

            auto startTime = std::chrono::high_resolution_clock::now();
            for (const auto& entry : data) {
                oramTrees.put(entry.first, entry.second, debugMode, randomReadRatio, ringFlag);
                position++;
                if (!statsMode) {
                    std::cout<<"position:"<<entry.first<<" stored completed"<<std::endl; 
                }
            }
            auto endTime = std::chrono::high_resolution_clock::now();

            loaded = true;
            inputFile.close();
            if (!statsMode) {
                std::cout<<args[1]<< " loaded successfully with " << data.size() << " entries." << std::endl;
            }
            if (debugMode) {
                std::cout<<oramTrees.toString() << std::endl;
            }
            if (statsMode) {
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                size_t totalStashSize = 0;
                for (const auto& tree : oramTrees.trees) {
                    totalStashSize += tree.stash.size();
                }
                std::cout << "\n=== STATISTICS ===" << std::endl;
                std::cout << "Total execution time: " << duration.count() << " ms" << std::endl;
                std::cout << "Total stash size: " << totalStashSize << " blocks" << std::endl;
                std::cout << "=================" << std::endl;
            }
        } else if (args[0] == "operate") {
            if (!loaded) {
                std::cerr << "No data loaded. Please use the 'store' or 'newTree' command first." << std::endl;
                continue;
            }
            if (args.size() < 2) {
                std::cerr << "Usage: operate <file_name>" << std::endl;
                continue;
            }

            std::string fileName = args[1];
            std::ifstream inputFile(fileName);
            if (!inputFile) {
                std::cerr << "Error opening file: " << fileName << std::endl;
                continue;
            }
            std::string line;
            int opCount = 0;
            auto startTime = std::chrono::high_resolution_clock::now();
            while (std::getline(inputFile, line)) {
                std::istringstream lineStream(line);
                std::string operation;
                if (lineStream >> operation) {
                    if (operation == "R") {
                        size_t position;
                        if (lineStream >> position) {
                            auto result = oramTrees.get(position, debugMode, randomReadRatio, ringFlag);
                            if (!statsMode) {
                                if (result.has_value()) {
                                    std::cout << "READ pos " << position << ": " << result.value() << std::endl;
                                } else {
                                    std::cout << "READ pos " << position << ": NOT FOUND" << std::endl;
                                }
                            }
                            opCount++;
                        } else {
                            std::cerr << "Invalid read operation format: " << line << std::endl;
                        }
                    } else if (operation == "W") {
                        size_t position;
                        int value;
                        if (lineStream >> position >> value) {
                            oramTrees.put(position, value, debugMode, randomReadRatio);
                            if (!statsMode) {
                                std::cout << "WRITE pos " << position << " val " << value << ": DONE" << std::endl;
                            }
                            opCount++;
                        } else {
                            std::cerr << "Invalid write operation format: " << line << std::endl;
                        }
                    } else {
                        std::cerr << "Unknown operation: " << operation << " in line: " << line << std::endl;
                    }
                    if (debugMode) {
                        std::cout << oramTrees.toString() << std::endl;
                    }
                }
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            inputFile.close();
            std::cout << "Processed " << opCount << " operations from " << fileName << std::endl;

            if (statsMode) {
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                size_t totalStashSize = 0;
                for (const auto& tree : oramTrees.trees) {
                    totalStashSize += tree.stash.size();
                }
                std::cout << "\n=== STATISTICS ===" << std::endl;
                std::cout << "Total execution time: " << duration.count() << " ms" << std::endl;
                std::cout << "Total stash size: " << totalStashSize << " blocks" << std::endl;
                std::cout << "Operations processed: " << opCount << std::endl;
                std::cout << "Average time per operation: " << (opCount > 0 ? duration.count() / opCount : 0) << " ms" << std::endl;
                std::cout << "=================" << std::endl;
            }

        } else if (args[0] == "get") {
            // you should call the print posRange command before using get to avoid accessing out of range positions
            if (!loaded) {
                std::cerr << "No data loaded. Please use the 'store' or 'newTree' command first." << std::endl;
                continue;
            }
            if (args.size() < 2) {
                std::cerr << "Usage: get <position>" << std::endl;
                continue;
            }
            size_t position;
            try {
                position = std::stoul(args[1]);
            } catch (const std::exception& e) {
                std::cerr << "Invalid position format: " << args[1] << std::endl;
                continue;
            }
            if (position >= oramTrees.getPosRange()) {
                std::cerr << "Position out of range: " << position << std::endl;
                continue;
            } else {
                auto result = oramTrees.get(position, debugMode, randomReadRatio, ringFlag);
                if (result.has_value()) {
                    std::cout << "GET pos " << position << ": " << result.value() << std::endl;
                } else {
                    std::cout << "GET pos " << position << ": NOT FOUND" << std::endl;
                }
            }
            std::cout<<"==========================="<<std::endl;
        } else if (args[0] == "put") { 
            // you should call the print posRange command before using put to avoid accessing out of range positions
            if (!loaded) {
                std::cerr << "No data loaded. Please use the 'store' or 'newTree' command first." << std::endl;
                continue;
            }
            if (args.size() < 3) {
                std::cerr << "Usage: put <position> <value>" << std::endl;
                continue;
            }
            size_t position;
            try {
                position = std::stoul(args[1]);
            } catch (const std::exception& e) {
                std::cerr << "Invalid position format: " << args[1] << std::endl;
                continue;
            }
            if (position >= oramTrees.getPosRange()) {
                std::cerr << "Position out of range: " << position << std::endl;
                continue;
            }
            int value;
            try {
                value = std::stoi(args[2]);
            } catch (const std::exception& e) {
                std::cerr << "Invalid value format: " << args[2] << std::endl;
                continue;
            }
            oramTrees.put(position, value, debugMode, randomReadRatio, ringFlag);
            std::cout << "PUT pos " << position << " val " << value << ": DONE" << std::endl;
            std::cout<<"==========================="<<std::endl;
        } else if (args[0] == "print") {
            if (args.size() < 2) {
                std::cerr << "Usage: print <ITEMS>"<<std::endl;
            } else if (args[1] == "trees") {
                if (args.size() < 3) {
                    std::cout<< oramTrees.toString() << std::endl;
                } else {
                    writeFileTo(args[2], oramTrees.toString());
                }
            } else if (args[1] == "sizes") {
                std::cout << oramTrees.getSizes() << std::endl;
            } else if (args[1] == "posRange" ){
                std::cout<< "Position range 1-" <<oramTrees.getPosRange()  - 1<< std::endl;
            } else {
                std::cerr << "Unknown print command: " << args[1] << std::endl;
            }

        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

}

