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
//===============================
// test files format:
// store: <position> <value>
//===============================
// operate: R <position> , or W <position> <value>
// where R is read operation and W is write operation

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
        auto debugIt = std::find(args.begin(), args.end(), "-d");
        if (debugIt != args.end()) {
            debugMode = true;
            args.erase(debugIt); 
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
            for (const auto& entry : data) {
                oramTrees.put(entry.first, entry.second);
                position++;
                std::cout<<"position:"<<entry.first<<" stored completed"<<std::endl;
            }
            loaded = true;
            inputFile.close();
            std::cout<<args[1]<< " loaded successfully with " << data.size() << " entries." << std::endl;
            if (debugMode) {
                std::cout<<oramTrees.toString() << std::endl;
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
            inputFile.close();
            std::cout << "Processed " << opCount << " operations from " << fileName << std::endl;


        } else if (args[0] == "size") {
            std::cout << oramTrees.getSizes() << std::endl;
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

}
