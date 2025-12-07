CXX 		:= g++
CXXFLAGS 	:= -Wall -Wextra -Isrc -fPIC

SRC 		:= src/day01.cpp
OBJ_DIR 	:= obj
OBJ 		:= $(OBJ_DIR)/day01.o

.PHONY: all clean

all: $(OBJ)

$(OBJ): $(SRC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC) -o $(OBJ)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)
