# 🕶 Project Title

Path-Oram implemented in C++

---

## 🧰 Environment Setup

This project was developed and tested in the following environment:
1.
- **Operating System**: Ubuntu 22.04 LTS  
- **Kernel Version**: 5.15.0-75-generic  
- **Python Version**: 3.10.12  
- **Dependencies**: See [requirements.txt](requirements.txt)

---

## 🚀 Installation

Clone the repository and install dependencies:

```bash
git clone git@github.com:Maurice904/Path-Oram.git
cd Path-Oram
```

For Linux: 
```bash
g++ -std=c++17 -Wall -Wextra -g -o path_oram src/Tree.cpp src/Forest.cpp src/rgen.cpp main/path_oram.cpp
```

---

## 🖥 Generate operations and data files

* For data file named storage.txt with 1000000 lines:
  * ```bash
    python3 scripts/fileGen.py storage 1000000 --output "storage.txt"
  
* For operation named operation.txt file with 1000000 lines:
  * ```bash
    python3 scripts/fileGen.py operate 1000000 --output "operation.txt"


## Main Commands
| Command                                                       | Description                                                                                               |                                                                                                                        |
| ------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------- |
| `store <file> [options]`                                      | Loads a data file into the ORAM.                                                                          |                                                                                                                        |
| `operate <file> [options]`                                    | Runs read/write operations from a file.                                                                   |                                                                                                                        |
| `get <position> [--r <random read ratio>] [-d] [-rp]`         | Reads the value at a specified position. Valid only if position is in range. <br> *Example:* `get 42 -rp` |                                                                                                                        |
| `put <position> <value> [--r <random read ratio>] [-d] [-rp]` | Writes a value to a specified position in the ORAM. <br> *Example:* `put 42 123 -rp`                      |                                                                                                                        |
| \`print sizes                                                 | trees \[output\_file]\`                                                                                   | Prints internal stats, tree structure, or position range. Can output to file. <br> *Example:* `print trees output.txt` |
| `newTree <data_size> <bucket_size> <max_tree_size> [-d]`      | Manually creates a forest with custom size parameters. <br> *Example:* `newTree 1000 4 100000`            |                                                                                                                        |
| `exit`                                                        | Terminates the program.                                                                                   |                                                                                                                        |

## 🔩 Supported Flags
Our path_oram interface accepts several command-line flags to customize execution behavior and enable optional optimizations:

| Flag               | Description                                                                                             | Example                                            |
| ------------------ | ------------------------------------------------------------------------------------------------------- | -------------------------------------------------- |
| `-s`               | **Silent mode**: Suppresses detailed logs and speeds up batch runs.                                     | `operate operation.txt -s`                         |
| `-rp`              | **Ring ORAM mode**: Enables ring-based pathing.                                      | `store storage.txt -s -rp`                         |
| `--r <float>`      | **Random hybrid mode**: Uses Ring ORAM with given probability; Path ORAM otherwise.                     | `--r 0.5` (50% Ring, 50% Path)                     |
| `--max-size <int>` | **Max tree size**: Forces single or multiple trees. If dataset exceeds this value, Forest mode is used. | `--max-size 100001` (single tree if data ≤ 100000) |

---
## 💡Run "Path-Oram" now!
* No Optimization - Original Path-Oram:
  ```bash
  $ ./path_oram
  $ Enter command: store storage.txt --max-size 1000001 -s # the number should always be greater than the data size, so it is 1000000 + 1
  $ Enter command: operate operation.txt -s # -s： Run in silent mode. Suppresses detailed logs and command outputs during execution.
  $ Enter command: print sizes
  $ Enter command: exit  ## exit the command
* Forest optimization：
  ```bash
  $ ./path_oram
  $ Enter command: store storage.txt -s
  $ Enter command: operate operation.txt -s
* Ring oram Optimization：
  ```bash
  $ ./path_oram
  $ Enter command: store storage.txt -s -rp --max-size 1000001 
  $ Enter command: operate operation.txt -s -rp 
 * Random read ratio + Forest Optimization：
   ```bash
   $ ./path_oram
   $ Enter command: store storage.txt -s --r 0.5 # r is used for the probability of using ring-oram
   $ Enter command: operate operation.txt -s --r 0.5
* Ring oram + Forest Optimization：
  ```bash
  $ ./path_oram
  $ Enter command: store storage.txt -s -rp
  $ Enter command: operate operation.txt -s -rp
---
## ⏰ Test it by yourself!
Simply run the follow command:
```bash
chmod +x run_all.sh
./run_all.sh
```
Then all the output result would be in files named ```no_opt_result.txt```, ```forest_opt_result.txt```, ```ring_oram_result.txt```, ```ring_oram_forest_result.txt```, ```random_rr_forest_result.txt```，and they also output csv files named  ```result_random_rr_forest.csv```, ```result_no_opt.csv```, ```result_forest_opt.csv```, ```result_ring_oram.csv```, ```result_ring_oram_forest.csv```
