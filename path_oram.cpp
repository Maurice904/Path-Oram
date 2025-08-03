#include "Forest.h"
#include "StandardPathORAM.h"
#include "showInformation.h"
#include "Plot.h"

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
#include <memory>
#include <iomanip>


//===============================
// COMMAND REFERENCE & USAGE:
//===============================
// INITIALIZATION COMMANDS:
//   store <file_name> [bucket_size] [-d] [-s] [-p]
//     - Load data from file and initialize ORAM structure
//     - bucket_size: optional bucket size (default: 4)
//     - flags: -d (debug output), -s (performance statistics), -p (use StandardPathORAM)
//
//   operate <file_name> [-d] [-s]
//     - Execute batch operations from file (requires prior 'store' command)
//     - flags: -d (debug output), -s (performance statistics)
//
// INTERACTIVE COMMANDS:
//   print <option> [output_file]
//     - trees: display complete ORAM tree structure
//     - sizes: show bucket and stash size information
//     - posRange: display valid position range for operations
//     - output_file: optional file to save output (only for trees)
//
//   exit
//     - Terminate the program
//
//===============================
// FILE FORMATS:
//===============================
// Store file format: <position> <value>
// Example: 1 100
//          2 200
//
// Operate file format: <operation> <position> [value]
// R <position>        - Read operation
// W <position> <value> - Write operation
// Example: R 1
//          W 2 150
//
//===============================
// BENCHMARK COMMANDS:
//===============================
//   plot store [-p]
//     - Generate performance benchmarks for store operations
//     - Tests: 100kstore, 200kstore, 300kstore, 400kstore, 500kstore, 1mstore
//     - Metrics: max stash size, execution time, memory usage
//     - flags: -p (use StandardPathORAM implementation)
//
//   plot op [-p]
//     - Generate performance benchmarks for operation files
//     - Tests corresponding operation files for each store dataset
//     - Metrics: operation throughput, stash behavior, timing analysis
//     - flags: -p (use StandardPathORAM implementation)
//===============================


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
    
    std::unique_ptr<std_poram::ServerStorage> storage;
    std::unique_ptr<std_poram::StandardRandomGen> rng;
    std::unique_ptr<std_poram::StandardPathORAM> standardOram;
    bool useStandardOram = false;
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

        if (args.empty()) {
            continue;
        }

        if (args[0] == "exit") {
            break;
        } else if (args[0] == "store") {
            if (args.size() < 2) {
                std::cerr << "Usage: store <file_name> [bucket_size] [-d] [-s] [-p]" << std::endl;
                continue;
            }
            
            bool debugMode = false;
            bool statsMode = false;
            bool useStandardFlag = false;
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
            auto standardIt = std::find(args.begin(), args.end(), "-p");
            if (standardIt != args.end()) {
                useStandardFlag = true;
                args.erase(standardIt); 
            }
            
            std::string fileName = args[1];
            int bucketSize = 4;
            if (args.size() > 2 && args[2][0] != '-') {
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

            auto startTime = std::chrono::high_resolution_clock::now();
            
            if (useStandardFlag) {
                useStandardOram = true;
                
                standardOram.reset();
                storage.reset();
                rng.reset();
                
                std_poram::ServerStorage::resetState();
                std_poram::Bucket::resetState();
                
                storage = std::make_unique<std_poram::ServerStorage>();
                rng = std::make_unique<std_poram::StandardRandomGen>();
                standardOram = std::make_unique<std_poram::StandardPathORAM>(storage.get(), rng.get(), bucketSize, data.size());
                
                for (const auto& entry : data) {
                    int newData[std_poram::Block::BLOCK_SIZE] = { entry.second, 0 };
                    int* result = standardOram->access(std_poram::StandardPathORAM::WRITE, entry.first - 1, newData);
                    delete[] result;
                    std::cout<<"position:"<<entry.first<<" stored completed"<<std::endl;
                }
            } else {
                useStandardOram = false;
                oramTrees = Forest(data.size(), bucketSize);
                
                for (const auto& entry : data) {
                    oramTrees.put(entry.first, entry.second);
                    std::cout<<"position:"<<entry.first<<" stored completed"<<std::endl;
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();

            loaded = true;
            inputFile.close();
            std::cout<<args[1]<< " loaded successfully with " << data.size() << " entries." << std::endl;
            
            if (debugMode) {
                if (useStandardOram) {
                    std::cout << "StandardPathORAM loaded - Stash size: " << standardOram->getStashSize() 
                              << ", Levels: " << standardOram->getNumLevels() 
                              << ", Leaves: " << standardOram->getNumLeaves() << std::endl;
                } else {
                    std::cout<<oramTrees.toString() << std::endl;
                }
            }
            if (statsMode) {
                ShowInformation::showStatistics(useStandardOram, standardOram, oramTrees, startTime, endTime, data.size());
            }
        } else if (args[0] == "operate") {
            if (!loaded) {
                std::cerr << "No data loaded. Please use the 'store' command first." << std::endl;
                continue;
            }
            if (args.size() < 2) {
                std::cerr << "Usage: operate <file_name> [-d] [-s]" << std::endl;
                continue;
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
            
            std::string fileName = args[1];
            std::ifstream inputFile(fileName);
            if (!inputFile) {
                std::cerr << "Error opening file: " << fileName << std::endl;
                continue;
            }
            std::string line;
            int opCount = 0;
            std::vector<int*> readBuffers;
            auto startTime = std::chrono::high_resolution_clock::now();
            
            while (std::getline(inputFile, line)) {
                std::istringstream lineStream(line);
                std::string operation;
                if (lineStream >> operation) {
                    if (operation == "R") {
                        size_t position;
                        if (lineStream >> position) {
                            if (useStandardOram) {
                                int* data = standardOram->access(std_poram::StandardPathORAM::READ, position - 1, nullptr);
                                if (data) {
                                    std::cout << "READ pos " << position << ": " << data[0] << std::endl;
                                    readBuffers.push_back(data);
                                } else {
                                    std::cout << "READ pos " << position << ": NOT FOUND" << std::endl;
                                }
                            } else {
                                auto result = oramTrees.get(position, debugMode);
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
                            if (useStandardOram) {
                                int newData[std_poram::Block::BLOCK_SIZE] = { value, 0 };
                                int* result = standardOram->access(std_poram::StandardPathORAM::WRITE, position - 1, newData);
                                delete[] result;
                            } else {
                                oramTrees.put(position, value, debugMode);
                            }
                            std::cout << "WRITE pos " << position << " val " << value << ": DONE" << std::endl;
                            opCount++;
                        } else {
                            std::cerr << "Invalid write operation format: " << line << std::endl;
                        }
                    } else {
                        std::cerr << "Unknown operation: " << operation << " in line: " << line << std::endl;
                    }
                    if (debugMode) {
                        if (useStandardOram) {
                            std::cout << "StandardPathORAM - Current stash size: " << standardOram->getStashSize() << std::endl;
                        } else {
                            std::cout << oramTrees.toString() << std::endl;
                        }
                    }
                }
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            inputFile.close();
            std::cout << "Processed " << opCount << " operations from " << fileName << std::endl;

            if (statsMode) {
                ShowInformation::showStatistics(useStandardOram, standardOram, oramTrees, startTime, endTime, oramTrees.dataCount, opCount);
            }
            
            for (int* ptr : readBuffers) {
                delete[] ptr;
            }

        } else if (args[0] == "get") {
            if (args.size() < 2) {
                std::cerr << "Usage: get <position> [-d]" << std::endl;
                continue;
            }
            
            bool debugMode = false;
            auto debugIt = std::find(args.begin(), args.end(), "-d");
            if (debugIt != args.end()) {
                debugMode = true;
                args.erase(debugIt); 
            }
            
            size_t position = std::stoul(args[1]);
            
            if (useStandardOram) {
                int* data = standardOram->access(std_poram::StandardPathORAM::READ, position - 1, nullptr);
                if (data) {
                    std::cout << "GET pos " << position << ": " << data[0] << std::endl;
                    delete[] data;
                } else {
                    std::cout << "GET pos " << position << ": NOT FOUND" << std::endl;
                }
            } else {
                if (position >= oramTrees.getPosRange()) {
                    std::cerr << "Position out of range: " << position << std::endl;
                    continue;
                } else {
                    auto result = oramTrees.get(position, debugMode);
                    if (result.has_value()) {
                        std::cout << "GET pos " << position << ": " << result.value() << std::endl;
                    } else {
                        std::cout << "GET pos " << position << ": NOT FOUND" << std::endl;
                    }
                }
            }
            std::cout<<"==========================="<<std::endl;
        } else if (args[0] == "put") { 
            if (args.size() < 3) {
                std::cerr << "Usage: put <position> <value> [-d]" << std::endl;
                continue;
            }
            
            bool debugMode = false;
            auto debugIt = std::find(args.begin(), args.end(), "-d");
            if (debugIt != args.end()) {
                debugMode = true;
                args.erase(debugIt); 
            }
            
            size_t position = std::stoul(args[1]);
            int value = std::stoi(args[2]);
            
            if (useStandardOram) {
                int newData[std_poram::Block::BLOCK_SIZE] = { value, 0 };
                int* result = standardOram->access(std_poram::StandardPathORAM::WRITE, position - 1, newData);
                delete[] result;
            } else {
                if (position >= oramTrees.getPosRange()) {
                    std::cerr << "Position out of range: " << position << std::endl;
                    continue;
                }
                oramTrees.put(position, value, debugMode);
            }
            std::cout << "PUT pos " << position << " val " << value << ": DONE" << std::endl;
            std::cout<<"==========================="<<std::endl;
        } else if (args[0] == "print") {
            if (args.size() < 2) {
                std::cerr << "Usage: print <ITEMS>"<<std::endl;
            } else if (args[1] == "trees") {
                std::string info = ShowInformation::getTreeInfo(useStandardOram, standardOram, oramTrees);
                if (args.size() < 3) {
                    std::cout << info << std::endl;
                } else {
                    writeFileTo(args[2], info);
                }
            } else if (args[1] == "sizes") {
                std::cout << ShowInformation::getSizeInfo(useStandardOram, standardOram, oramTrees) << std::endl;
            } else if (args[1] == "posRange" ){
                std::cout << ShowInformation::getPositionRangeInfo(useStandardOram, standardOram, oramTrees) << std::endl;
            } else {
                std::cerr << "Unknown print command: " << args[1] << std::endl;
            }

        } else if (args[0] == "plot") {
            if (args.size() < 2) {
                std::cerr << "Usage: plot <store|op> [-p]" << std::endl;
                continue;
            }
            
            if (args[1] == "store") {
                bool useStandardFlag = false;
                auto standardIt = std::find(args.begin(), args.end(), "-p");
                if (standardIt != args.end()) {
                    useStandardFlag = true;
                }
                
                PathORAMPlot::plotStoreBenchmark(useStandardFlag);
            } else if (args[1] == "op") {
                bool useStandardFlag = false;
                auto standardIt = std::find(args.begin(), args.end(), "-p");
                if (standardIt != args.end()) {
                    useStandardFlag = true;
                }
                
                PathORAMPlot::plotOpBenchmark(useStandardFlag);
            } else {
                std::cerr << "Unknown plot command: " << args[1] << ". Use 'store' or 'op'." << std::endl;
            }

        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

}

