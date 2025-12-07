#include "day07.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <map>

using namespace std;

using pos   = pair<uint16_t, uint16_t>;
using Beams = map<pos, uint64_t>;
using Grid  = vector<string>;

static bool
file_to_vector_of_strings(const string & file_name, vector<string> & lines)
{
        ifstream in(file_name);
        if (!in)
                return false;

        string line;
        while (getline(in, line))
                lines.push_back(line);

        in.close();
        return true;
}

static uint32_t
simulate(const Grid &grid, map<pos, uint64_t> & beams)
{
        Beams next{};
        uint32_t part_01 = 0;

        for (const auto &[pos, count] : beams)
        {
                auto [r, c] = pos;

                if ('^' == grid[r + 1][c])
                {
                        ++part_01;
                        next[ {(r + 1), (c - 1)}] += count;
                        next[ {(r + 1), (c + 1)}] += count;
                }
                else
                {
                        next[ {(r + 1), c}] += count;
                }

        }
        beams.swap(next);
        return part_01;
}

void day07(const char* fp)
{
        Grid grid{};
        if (!file_to_vector_of_strings(fp, grid))
                return;

        Beams beams{};
        beams[{1, grid.at(0).find('S')}] = 1;

        uint32_t num_splits = 0;
        uint16_t r = 1;
        while (r++ < grid.size() - 2)
        {
                num_splits += simulate(grid, beams);
        }

        cout << "Day 07 Part 01: " << num_splits << endl;

        uint64_t num_beams = 0;
        for (const auto &kvp : beams)
        {
                num_beams += kvp.second;
        }
        cout << "Day 07 Part 02: " << num_beams << endl;
}
