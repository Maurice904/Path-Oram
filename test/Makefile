# Simple test Makefile for Path-ORAM
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Source files (relative to test directory)
CORE_SOURCES = ../src/Tree.cpp ../src/Forest.cpp ../src/rgen.cpp
TEST_SOURCES = test.cpp
TARGET = test_runner

# Default target - build test program
all: $(TARGET)

# Build test program
$(TARGET): $(CORE_SOURCES) $(TEST_SOURCES)
    @echo "Building Path-ORAM Tests..."
    @echo "Compiling test program..."
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(CORE_SOURCES) $(TEST_SOURCES)
    @echo "✅ Test build successful!"
    @echo "Run tests with: ./$(TARGET)"

# Build and run tests
run: $(TARGET)
    @echo "🧪 Running tests..."
    ./$(TARGET)

# Clean test artifacts
clean:
    rm -f $(TARGET) $(TARGET).exe
    @echo "🧹 Test artifacts cleaned"

# Show help
help:
    @echo "Available targets:"
    @echo "  all (default) - Build test program"
    @echo "  run           - Build and run tests"
    @echo "  clean         - Remove test artifacts"
    @echo "  help          - Show this help"

.PHONY: all run clean help