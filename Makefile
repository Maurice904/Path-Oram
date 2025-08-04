CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = path_oram
SOURCES = src/Tree.cpp src/Forest.cpp src/rgen.cpp main/path_oram.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
    rm -f $(TARGET) $(TARGET).exe

test: $(TARGET)
    ./$(TARGET)

.PHONY: all clean test