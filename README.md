# Memory Management Simulator (Paging with FIFO & LRU)

## 📌 Overview
This project is an **Operating Systems simulator** implemented in C++ that demonstrates how **paging and swap memory management** work. It allows processes to be loaded, executed, and terminated while managing their memory pages using either **FIFO (First-In-First-Out)** or **LRU (Least Recently Used)** page replacement algorithms.

The system mimics OS-level memory management by:  
- Dividing main memory and swap memory into fixed-size pages  
- Maintaining page tables for each process  
- Handling page faults by swapping pages between main and swap memory  
- Supporting user commands to load, run, kill, and inspect processes  

---

## 🚀 Features
- **Memory Management**
  - Main memory and swap memory divided into pages  
  - Page allocation and deallocation on process load/kill  

- **Page Replacement**
  - FIFO policy (oldest page swapped out first)  
  - LRU policy (least recently accessed page swapped out first)  

- **Process Management**
  - Load executables with instructions  
  - Run, kill, and list active processes  
  - Maintain page tables per process  

- **Command Support**
  - `load <files>` → Load processes into memory  
  - `run <pid>` → Execute process instructions (`add`, `sub`, `load`, `print`)  
  - `kill <pid>` → Terminate a process and free its memory  
  - `listpr` → List all active processes  
  - `pte <pid>` → Print page table for a process  
  - `pteall` → Print all page tables  
  - `print <memloc>` → Inspect memory contents  
  - `exit` → Exit the simulator  

---

## ⚙️ Usage
Compile the program using `make` with the desired replacement algorithm:

```bash
make fifo
./fifo -M <main memory size KB> -V <swap memory size KB> -P <page size bytes> -i <input file> -o <output file>

make lru
./lru -M <main memory size KB> -V <swap memory size KB> -P <page size bytes> -i <input file> -o <output file>
```

Example:  
```bash
./fifo -M 2 -V 32 -P 512 -i infile -o outfile
```

---

## 📂 Data Structures
- `vector<vector<int>> Mainmem, SwapMem` → Represent the main memory and swap memory, both divided into pages with offset.  
- `vector<int> Mainid, Swapid` → Track which process (by its PID) owns each page in the main and swap memory.  
- `list<int> fifo` → Used to manage the FIFO page replacement queue.  
- `vector<bool> MainmemAvl, SwapMemAvl` → Track the availability (free or occupied) of pages in the main and swap memory.  
- `map<int, int> indexes` → Tracks page usage times for the Least Recently Used (LRU) page replacement algorithm.  
- `int pnum` → A counter for assigning unique process IDs (PIDs).  
- `int avlPagesMain, avlPagesSwap` → Keep track of the available pages in main and swap memory.  
- `int ind` → A global counter used to track the time of memory access (for LRU).  
- `map<int, process> allProc` → Stores all the processes, mapped by their PID.  

---

## 📦 Classes

### pagetableentry
- `pid`: The process ID  
- `logadd`: Logical address of the page  
- `phyadd`: Physical address (index) of the page in memory  
- `isinmain`: A flag to indicate if the page is currently in the main memory or swap memory  

### process
- `pid`: Process ID  
- `size`: The size of the process  
- `isactive`: Indicates if the process is active (i.e., it hasn't been killed)  
- `vector<pagetableentry> pagetable`: The page table for the process  
- `vector<string> instructions`: A list of instructions for the process to execute  
- `runprocess()` → Runs the process by interpreting and executing its instructions (`add`, `sub`, `load`, `print`)  
- `killprocess()` → Terminates the process, freeing up the pages in memory and swap space  
- `isPractive()` → Returns whether the process is currently active  

---


### Supported Commands
- `load` → Loads a process  
- `run` → Runs a specific process by PID  
- `kill` → Terminates a process  
- `listpr` → Lists active processes  
- `pte` → Prints the page table of a specific process  
- `pteall` → Prints all page tables  
- `print` → Prints memory values  
- `exit` → Terminates the program  

---

## 🔧 Functions

### swappages2
```cpp
void swappages2(int x, int pid, vector<pagetableentry> &pagetable)
```
Handles page swapping based on the selected algorithm (FIFO or LRU). If the main memory is full, this function swaps out pages to the swap memory and brings in the required page from the swap memory.  

### loadprocess
```cpp
void loadprocess(const string &tobeloaded)
```
Loads a process from a file, assigning its pages to either main memory or swap memory based on availability. Creates a page table for the process and tracks it in `allProc`.  

### swappages
```cpp
void swappages(int x, int t, int id)
```
Performs the actual swapping of pages between main memory and swap memory. Updates both the process’s page table and memory structures when pages are swapped.  
