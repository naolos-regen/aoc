CXX             := g++
CXXFLAGS        := -Wall -Wextra -Iinc -fPIC

SRC             := src/day03.cpp
OBJ_DIR         := obj
OBJ             := $(OBJ_DIR)/day03.o


.PHONY: all clean

all: $(OBJ)

$(OBJ): $(SRC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC) -o $(OBJ)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)
