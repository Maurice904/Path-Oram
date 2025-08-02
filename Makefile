CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = path_oram
SOURCES = Tree.cpp Forest.cpp path_oram.cpp rgen.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
    rm -f $(TARGET) $(TARGET).exe

test: $(TARGET)
    ./$(TARGET)

.PHONY: all clean test