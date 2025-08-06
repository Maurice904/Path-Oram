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
