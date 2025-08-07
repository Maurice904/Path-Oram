# 🕶 Path-ORAM Implementation in C++

A high-performance Path-ORAM implementation with multiple optimizations including Forest, Ring ORAM pathing, and Random Read Ratio.

**Repository**: [github.com/Maurice904/Path-Oram](https://github.com/Maurice904/Path-Oram)

---

## 🧰 Environment Setup

This project was developed and tested in the following environment:
- **Operating System**: Ubuntu 22.04 LTS  
- **Kernel Version**: 5.15.0-75-generic  
- **Python Version**: 3.10.12  
- **C++ Standard**: C++17
- **Build Tools**: g++, make

---

## 🚀 Installation

### Clone from GitHub Repository

```bash
git clone https://github.com/Maurice904/Path-Oram.git
cd Path-Oram
```

### Compile the Executable

**Linux:**
```bash
make
```

**Windows:**
```bash
.\build.ps1
```

---

## Generating Simulation Files

We provide a script `fileGen.py` in the `scripts` folder to generate both storage files (containing original data) and operation files.

### For Data File Generation:
```bash
python3 scripts/fileGen.py storage <data_size> --output <filename> [--max-value <max_value>]
```

### For Operations File Generation:
```bash
python3 scripts/fileGen.py operate <instruction_amount> --output <filename> [--max-value <max_value>] [--max-position <max_position>] [--read-ratio <read_ratio>]
```

### Examples:
```bash
# Generate storage file with 100,000 entries
python3 scripts/fileGen.py storage 100000 --output storage.txt

# Generate operation file with 100,000 operations
python3 scripts/fileGen.py operate 100000 --output operation.txt
```

### File Formats

**Storage File Format:**
```
<position> <value>
1 42
2 17
3 99
...
```

**Operations File Format:**
```
R <position>          # Read operation
W <position> <value>  # Write operation
```

---
## 📋 Command-Line Interface

Run the program with `./path_oram` (Linux) or `.\path_oram.exe` (Windows) after compilation.

### Main Commands
| Command | Description |
|---------|-------------|
| `store <file_name> [-s] [--r <ratio>] [-d] [--max-size <size>] [-rp]` | Loads a data file into the ORAM |
| `operate <file_name> [-s] [--r <ratio>] [-d] [--max-size <size>] [-rp]` | Runs read/write operations from a file |
| `get <position> [--r <ratio>] [-d] [-rp]` | Reads value at specified position<br>*Example:* `get 42 -rp` |
| `put <position> <value> [--r <ratio>] [-d] [-rp]` | Writes value to specified position<br>*Example:* `put 42 123 -rp` |
| `print sizes\|trees [output_file]` | Prints internal stats or tree structure<br>*Example:* `print trees output.txt` |
| `newTree <data_size> <bucket_size> <max_tree_size> [-d]` | Manually creates forest with custom parameters<br>*Example:* `newTree 1000 4 100000` |
| `exit` | Terminates the program |

### 🔩 Supported Flags

| Flag | Description | Example |
|------|-------------|---------|
| `-s` | **Statistic Mode**: Suppresses detailed logs for faster batch runs | `operate operation.txt -s` |
| `-rp` | **Ring ORAM Mode**: Enables "opposite" path eviction strategy | `store storage.txt -s -rp` |
| `--r <float>` | **Random Read Ratio**: Probability of using Ring ORAM (0.0-1.0) | `--r 0.5` (50% Ring, 50% Path) |
| `--max-size <int>` | **Max Tree Size**: Forces single tree if data ≤ max-size, otherwise uses Forest | `--max-size 100001` |
| `-d` | **Debug Mode**: Enables detailed debug output | `get 42 -d` |

**Note:** All flags can be combined to test layered optimizations.

---

## 🧪 Correctness Testing

To verify that read/write operations return and modify correct values:

1. **Navigate to test directory:**
   ```bash
   cd test
   ```

2. **Compile test program:**
   ```bash
   # Linux
   make
   
   # Windows
   .\build_test.ps1
   ```

3. **Run tests:**
   ```bash
   # Linux
   ./test_runner
   
   # Windows
   .\test_runner.exe
   ```

The test suite builds large ORAM trees and performs extensive get/put operations to verify correctness.

---

## 💡 Demo Examples

### 1. Standard Path ORAM (No Optimization)
```bash
./path_oram
Enter command: store storage.txt --max-size 100001 -s
Enter command: operate operation.txt -s
Enter command: print sizes
Enter command: exit
```

### 2. Forest Optimization
```bash
./path_oram
Enter command: store storage.txt -s
Enter command: operate operation.txt -s
```

### 3. "Opposite" Path Optimization
```bash
./path_oram
Enter command: store storage.txt -s -rp --max-size 100001
Enter command: operate operation.txt -s -rp
```

### 4. Random Read Ratio + Forest Optimization
```bash
./path_oram
Enter command: store storage.txt -s --r 0.5
Enter command: operate operation.txt -s --r 0.5
```

### 5. "Opposite" Path + Forest Optimization
```bash
./path_oram
Enter command: store storage.txt -s -rp
Enter command: operate operation.txt -s -rp
```
---

## ⏰ Automated Benchmarking

To simplify testing and comparison of all optimization configurations:

```bash
chmod +x run_all.sh
./run_all.sh
```

This script automatically:
- Runs all five optimization configurations
- Tests different operation and data sizes
- Collects performance metrics (stash size, execution time)
- Generates comprehensive output files

### Output Files

**Raw Log Files:**
- `no_opt_result.txt` - Standard Path ORAM results
- `forest_opt_result.txt` - Forest optimization results  
- `ring_oram_result.txt` - Ring ORAM optimization results
- `ring_oram_forest_result.txt` - Combined Ring ORAM + Forest results
- `random_rr_forest_result.txt` - Random Read Ratio + Forest results

**CSV Files (for plotting/analysis):**
- `result_no_opt.csv`
- `result_forest_opt.csv` 
- `result_ring_oram.csv`
- `result_ring_oram_forest.csv`
- `result_random_rr_forest.csv`

---

## 🏗️ Project Structure

```
Path-Oram/
├── src/                    # Core implementation
│   ├── Tree.h/.cpp        # Single ORAM tree
│   ├── Forest.h/.cpp      # Multi-tree forest optimization  
│   └── rgen.h/.cpp        # Random number generation
├── main/
│   └── path_oram.cpp      # Main application entry point
├── test/
│   ├── test.cpp           # Correctness test suite
│   ├── Makefile           # Test build configuration
│   └── build_test.ps1     # Windows test build script
├── scripts/
│   ├── fileGen.py         # Data/operation file generator
│   └── test_script.py     # Additional testing utilities
├── testFiles/             # Generated test data
├── plot/                  # Performance analysis and plotting
├── shell/                 # Benchmark automation scripts
├── Makefile               # Main build configuration
├── build.ps1              # Windows build script
├── run_all.sh             # Automated benchmarking script
└── README.md              # This file
```

---

## 🔬 Technical Features

### Core Optimizations

1. **Forest Mode**: Splits large datasets across multiple smaller trees for better performance
2. **"Opposite" Path Eviction**: Ring ORAM-inspired eviction strategy for improved load balancing  
3. **Random Read Ratio**: Selective path reading based on probability thresholds
4. **Efficient Eviction**: O(n) eviction algorithm vs. O(n²) reference implementations

### Performance Characteristics

- **Access Complexity**: O(log n) for standard, O(n/k*log(k)) for Forest mode
- **Memory Efficiency**: Optimized stash management with vector-based storage
- **Scalability**: Handles datasets from thousands to millions of entries
- **Cross-Platform**: Builds and runs on Linux, Windows, and macOS

---

## 📊 Performance Analysis

For detailed performance analysis and comparisons, run the automated benchmarking suite and analyze the generated CSV files.
