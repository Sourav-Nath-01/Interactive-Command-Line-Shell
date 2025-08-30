# Compiler settings
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN = shell

# Files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Default target
all: $(BIN)

# Link object files
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lreadline

# Compile source files into objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure obj directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build
clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean

