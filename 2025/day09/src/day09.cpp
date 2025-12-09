#include "day09.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <cassert>

using pos2 = std::array<uint32_t, 2>;
using poly = std::vector<pos2>;
using solve = std::pair<uint64_t, uint64_t>;

[[nodiscard]] static bool 
point_on_edge(const pos2& p, const poly& poly)
{
        const auto n = poly.size();
        for (size_t i = 0; i < n; ++i)
        {
                const auto& a = poly[i];
                const auto& b = poly[(i + 1) % n];
                if (a[0] == b[0] && p[0] == a[0] && p[1] >= std::min(a[1], b[1]) && p[1] <= std::max(a[1], b[1]))
                                        return (true);
                else if (p[1] == a[1] && p[0] >= std::min(a[0], b[0]) && p[0] <= std::max(a[0], b[0]))
                                        return (true);
        }
        return false;
}

[[nodiscard]] static bool 
point_in_poly(const pos2& p, const poly& poly)
{
        const auto n = poly.size();
        uint64_t crossings = 0;
        for (size_t i = 0; i < n; ++i)
        {
                const auto& p1 = poly[i];
                const auto& p2 = poly[(i + 1) % n];
                if (p1[0] == p2[0] && p[1] >= std::min(p1[1], p2[1]) && p[1] < std::max(p1[1], p2[1]))
                                crossings += p1[0] > p[0] ? 1 : 0;
        }

        return (1 == crossings % 2);
}


[[nodiscard]] static bool
point_in_on(const pos2& p, const poly& poly)
{
        return (point_on_edge(p, poly) || point_in_poly(p, poly));
}

[[nodiscard]] static bool
segment_inside_poly(const pos2& a, const pos2& b, const poly& poly)
{
        assert(point_in_on(a, poly));
        assert(point_in_on(b, poly));

        if (a[1] == b[1])
        {
                const auto x1 = std::min(a[0], b[0]);
                const auto x2 = std::max(a[0], b[0]);
                for (size_t i = 0; i < poly.size(); ++i)
                {
                        const auto& p1 = poly[i];
                        const auto& p2 = poly[(i + 1) % poly.size()];
                        if (p1[0] == p2[0])
                        {
                                if (a[1] >= std::min(p1[1], p2[1]) && a[1] <= std::max(p1[1], p2[1]))
                                {
                                        if (p1[0] > x1 && p1[0] < x2)
                                        {
                                                return false;
                                        }
                                }
                        }
                }
                return true;
        }

        const auto y1 = std::min(a[1], b[1]);
        const auto y2 = std::max(a[1], b[1]);
        for (size_t i = 0; i < poly.size(); ++i)
        {
                const auto& p1 = poly[i];
                const auto& p2 = poly[(i + 1) % poly.size()];
                if (p1[1] == p2[1])
                {
                        if (a[0] >= std::min(p1[0], p2[0]) && a[0] <= std::max(p1[0], p2[0]))
                        {
                                if (p1[1] > y1 && p1[1] < y2)
                                {
                                        return false;
                                }
                        }
                }
        }
        return true;
}

[[nodiscard]] static bool 
rectangle_inside(const pos2& p1, const pos2& p3, const poly& poly)
{
        assert(point_on_edge(p1, poly));
        assert(point_on_edge(p3, poly));

        const auto& [s1, e1] = p1;
        const auto& [s3, e3] = p3;
        const auto p2 = pos2{s1, e3};
        if (!point_in_on(p2, poly))
                return false;
        const auto p4 = pos2{s3, e1};
        if (!point_in_on(p4, poly))
                return false;
        if (!segment_inside_poly(p1, p2, poly))
                return false;
        if (!segment_inside_poly(p2, p3, poly))
                return false;
        if (!segment_inside_poly(p3, p4, poly))
                return false;
        if (!segment_inside_poly(p4, p1, poly))
                return false;
        return true;
}

[[nodiscard]] static poly
parse (const std::vector<std::string> & lines)
{
        poly p{};
        for (const auto & line : lines)
        {
                pos2 pos{0,0};
                size_t idx = 0;
                for (const auto & c : line)
                {
                        if (c == ',')
                                idx++;
                        if (std::isdigit(c))
                                pos[idx] = pos[idx] * 10 + (c - '0');
                }
                p.push_back(pos);
        }
        return (p);
}

[[nodiscard]] static bool
read (const std::string & fp, std::vector<std::string> & lines)
{
        std::ifstream in{fp};
        if (!in)
                return (false);

        std::string line;
        while (std::getline(in, line))
                lines.push_back(line);
        return (true);
}

[[nodiscard]] static solve
solution (const poly & p)
{
        uint64_t maxArena1 = 0;
        uint64_t maxArena2 = 0;
        
        std::vector<uint64_t> arena_cache(p.size() * p.size(), 0);

        for (size_t row = 0; row < p.size(); ++row)
        {
                const auto & [row_s, row_e] = p[row];
                for (size_t col = 0; col < p.size(); ++col)
                {
                        if (arena_cache[row * p.size() + col] != 0)
                                continue;
                        const auto & [col_s, col_e] = p[col];
                        const uint64_t dist1 = row_s >= col_s ? row_s - col_s : col_s - row_s;
                        const uint64_t dist2 = row_e >= col_e ? row_e - col_e : col_e - row_e;
                        arena_cache[row * p.size() + col] = (dist1 + 1) * (dist2 + 1);
                        maxArena1 = std::max(maxArena1, arena_cache[row * p.size() + col]);
                        if (!rectangle_inside(p[row], p[col], p))
                                continue;
                        maxArena2 = std::max(maxArena2, arena_cache[row * p.size() + col]);
                }
        }
        return (std::make_pair(maxArena1, maxArena2));
}

void
day09(const char* fp)
{
        std::vector<std::string> lines{};
        if (!read(fp, lines))
                return;

        poly p = parse(lines);

        solve s = solution(p);
        std::cout << "Day 09 Part 01: " << s.first << std::endl;
        std::cout << "Day 09 Part 02: " << s.second << std::endl;
}
