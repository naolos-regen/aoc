#include "day08.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <set>
#include <queue>
#include <assert.h>

using pos = std::array<uint64_t, 3>;

struct Node
{
        pos p;
        uint64_t id;
};

struct Pair
{
        Node first;
        Node second;
        uint64_t d2;
};

using circuit = std::set<size_t>;
using circuits = std::vector<circuit>;

struct cmpare
{
        bool operator()(const Pair & a, const Pair & b)
        {
                return a.d2 > b.d2;
        };
};
using Que_Pasa = std::priority_queue<Pair, std::vector<Pair>, cmpare>;


bool
read_file(const std::string & fp, std::vector<std::string> & lines)
{
        std::ifstream in{fp};
        if (!in)
                return (false);

        std::string line;
        while (std::getline(in, line))
        {
                lines.push_back(line);
        };

        in.close();
        return (true);
};

std::vector<pos>
parse(const std::vector<std::string> & lines)
{
        std::vector<pos> poses;
        for (const auto & line : lines)
        {
                std::istringstream iss(line);
                uint64_t x = 0, y = 0, z = 0;
                iss >> x;
                iss.ignore(1);
                iss >> y;
                iss.ignore(1);
                iss >> z;
                poses.push_back({x, y, z});
        };
        return (poses);
};

Que_Pasa
pairs(const std::vector<pos> & poses)
{
        auto dist = [](const pos & a, const pos & b) -> uint64_t
        {
                const uint64_t d0 = a[0] - b[0], d1 = a[1] - b[1], d2 = a[2] - b[2];
                return (d0 * d0 + d1 * d1 + d2 * d2);
        };

        Que_Pasa pairs;
        for (size_t x = 0; x < poses.size(); ++x)
        {
                for (size_t y = x + 1; y < poses.size(); ++y)
                {
                        pairs.push({{poses[x], x}, {poses[y], y}, dist(poses[x], poses[y])});
                }
        }
        return (pairs);
};

void
connect(circuits & circuits, const Pair & pair)
{
        const auto & [a, b, _] = pair;
        std::vector<size_t> found{};

        for (size_t y = 0; y < circuits.size(); ++y)
        {
                const auto & circuit = circuits[y];
                if (circuit.find(a.id) != circuit.end() || circuit.find(b.id) != circuit.end())
                {
                        found.push_back(y);
                }
        }
        if (found.empty())
        {
                circuits.push_back(circuit{a.id, b.id});
        }
        else if (found.size() == 1)
        {
                auto & circuit = circuits[found.front()];
                circuit.insert(a.id);
                circuit.insert(b.id);
        }
        else
        {
                assert(2 == found.size());
                auto& circuit = circuits[found.front()];
                const auto& other = circuits[found.back()];
                circuit.insert(other.begin(), other.end());
                circuit.insert(a.id);
                circuit.insert(b.id);
                circuits.erase(circuits.begin() + found.back());
        }
};

void day08(const char* fp)
{
        std::vector<std::string> lines;
        if (!read_file(fp, lines))
                exit(1);

        auto positions = parse(lines);

        Que_Pasa all_pairs = pairs(positions);

        circuits part1_circuits;

        Que_Pasa pq1 = all_pairs; 

        for (size_t i = 0; i < 1000; ++i)
        {
                connect(part1_circuits, pq1.top());
                pq1.pop();
        }

        std::nth_element(
                part1_circuits.begin(),
                part1_circuits.begin() + 2,
                part1_circuits.end(),
                [](auto &a, auto &b)
        {
                return a.size() > b.size();
        }
        );

        std::cout << "Day 08 Part 01: "
                  << part1_circuits[0].size() *
                  part1_circuits[1].size() *
                  part1_circuits[2].size()
                  << "\n";


        circuits part2_circuits;
        Que_Pasa pq2 = all_pairs;   
        Pair last;

        auto fully_connected = [&]()
        {
                uint64_t max_size = 0;
                for (auto &c : part2_circuits)
                        max_size = std::max(max_size, c.size());
                return max_size == lines.size();
        };

        while (!fully_connected())
        {
                last = pq2.top();
                pq2.pop();
                connect(part2_circuits, last);
        }

        std::cout << "Day 08 Part 02: "
                  << last.first.p[0] * last.second.p[0]
                  << "\n";
};
