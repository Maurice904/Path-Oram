#ifndef PLOT_H
#define PLOT_H

#include <string>
#include <vector>



namespace PathORAMPlot {


    struct BenchmarkResult {
        std::string fileName;
        size_t dataSize;
        size_t maxStashSize;     
        long long executionTime;
        size_t memoryUsage;
    };


    void plotStoreBenchmark(bool useStandardFlag = false);


    void plotOpBenchmark(bool useStandardFlag = false);


    void printBenchmarkSummary(const std::vector<BenchmarkResult>& results, 
                              const std::string& algorithmName);


    void printOpBenchmarkSummary(const std::vector<BenchmarkResult>& results, 
                                const std::string& algorithmName);

}

#endif