CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = path_oram
SOURCES = Tree.cpp path_oram.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) $(TARGET).exe

test: $(TARGET)
	./$(TARGET)

.PHONY: all clean test
