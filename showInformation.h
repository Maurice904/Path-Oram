#ifndef SHOW_INFORMATION_H
#define SHOW_INFORMATION_H

#include "Forest.h"
#include "StandardPathORAM.h"
#include <chrono>
#include <string>
#include <memory>


class ShowInformation {
public:

    static void showStatistics(bool useStandardOram,
                             std::unique_ptr<std_poram::StandardPathORAM>& standardOram,
                             const Forest& oramTrees,
                             const std::chrono::high_resolution_clock::time_point& startTime,
                             const std::chrono::high_resolution_clock::time_point& endTime,
                             size_t dataSize,
                             int opCount = 0);


    static void showDebugInfo(bool useStandardOram,
                            std::unique_ptr<std_poram::StandardPathORAM>& standardOram,
                            const Forest& oramTrees,
                            const std::string& prefix = "");


    static std::string getTreeInfo(bool useStandardOram,
                                  std::unique_ptr<std_poram::StandardPathORAM>& standardOram,
                                  const Forest& oramTrees);


    static std::string getSizeInfo(bool useStandardOram,
                                  std::unique_ptr<std_poram::StandardPathORAM>& standardOram,
                                  const Forest& oramTrees);


    static std::string getPositionRangeInfo(bool useStandardOram,
                                           std::unique_ptr<std_poram::StandardPathORAM>& standardOram,
                                           const Forest& oramTrees);

private:

    static size_t calculateStandardOramMemory(std::unique_ptr<std_poram::StandardPathORAM>& standardOram);


    static size_t calculateForestMemory(const Forest& oramTrees, size_t dataSize);
};

#endif