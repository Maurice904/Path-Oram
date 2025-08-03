CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = path_oram
SOURCES = Tree.cpp Forest.cpp StandardPathORAM.cpp path_oram.cpp showInformation.cpp Plot.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
    rm -f $(TARGET) $(TARGET).exe

test: $(TARGET)
    ./$(TARGET)

.PHONY: all clean test