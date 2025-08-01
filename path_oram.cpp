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


//command format: [store|operate] <file_name> [-d]
//                 print [sizes|trees] [output_file]
//                 exit
//===============================
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

        bool debugMode = false;
        bool statsMode = false;
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

        if (args.empty()) {
            continue;
        }

        if (args[0] == "exit") {
            break;
        } else if (args[0] == "store") {
            if (args.size() < 2) {
                std::cerr << "Usage: store <file_name>" << std::endl;
                continue;
            }
            std::string fileName = args[1];
            int bucketSize = 4;
            if (args.size() > 2) {
                bucketSize = std::stoi(args[2]);
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
            oramTrees = Forest(data.size(), bucketSize);
            size_t position = 0;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            for (const auto& entry : data) {
                oramTrees.put(entry.first, entry.second);
                position++;
                std::cout<<"position:"<<entry.first<<" stored completed"<<std::endl;
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            
            loaded = true;
            inputFile.close();
            std::cout<<args[1]<< " loaded successfully with " << data.size() << " entries." << std::endl;
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
                std::cerr << "No data loaded. Please use the 'store' command first." << std::endl;
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
                            auto result = oramTrees.get(position, debugMode);
                            if (result.has_value()) {
                                std::cout << "READ pos " << position << ": " << result.value() << std::endl;
                            } else {
                                std::cout << "READ pos " << position << ": NOT FOUND" << std::endl;
                            }
                            opCount++;
                        } else {
                            std::cerr << "Invalid read operation format: " << line << std::endl;
                        }
                    } else if (operation == "W") {
                        size_t position;
                        int value;
                        if (lineStream >> position >> value) {
                            oramTrees.put(position, value, debugMode);
                            std::cout << "WRITE pos " << position << " val " << value << ": DONE" << std::endl;
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
            } else {
                std::cerr << "Unknown print command: " << args[1] << std::endl;
            }

        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

}

