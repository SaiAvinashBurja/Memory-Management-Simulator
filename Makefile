# Compiler and Flags
CC = g++
CFLAGS = -Wall -std=c++17

# Source and Output Files
SRC = main.cpp
OUTFILE = outfile1

# Executable Names
FIFO = fifo
LRU = lru

# Phony Targets
.PHONY: all clean

# Default Target: Build both FIFO and LRU executables
all: $(FIFO) $(LRU)

# Build FIFO Mode Executable
$(FIFO): $(SRC)
	$(CC) $(CFLAGS) -o $(FIFO) $(SRC) -D MODE_FIFO

# Build LRU Mode Executable
$(LRU): $(SRC)
	$(CC) $(CFLAGS) -o $(LRU) $(SRC) -D MODE_LRU

# Clean Target to Remove All Outputs
clean:
	rm -f $(FIFO) $(LRU) $(OUTFILE)
