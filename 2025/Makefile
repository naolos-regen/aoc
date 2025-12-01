CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17

DAYS = day01 day02 day03 day04 day05 day06 day07 day08 day09 day10 day11 day12

DAY_OBJS = $(foreach day, $(DAYS), $(day)/$(day).o)

YEAR_OBJ = 2025.o

.PHONY: all clean

all: $(DAY_OBJS) $(YEAR_OBJ)

$(DAY_OBJS):
	@for day in $(DAYS); do \
		$(MAKE) -C $$day; \
	done

$(YEAR_OBJ): 2025.cpp 2025.h
	$(CXX) $(CXXFLAGS) -c 2025.cpp -o $(YEAR_OBJ)

clean:
	$(RM) $(YEAR_OBJ)
	@for day in $(DAYS); do \
		$(MAKE) -C $$day clean; \
	done

