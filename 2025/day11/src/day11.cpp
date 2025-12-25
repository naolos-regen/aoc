#include "day11.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <cstdint>
#include <array>

using namespace std;

static bool
parse (const ::string & file_name, ::vector<::string> & lines)
{
        ifstream in{file_name};
        if (!in)
                return (false);

        string line;
        while (getline(in, line))
                lines.emplace_back(line);
        in.close();
        return (true);
}

static unordered_map<string,vector<string>>
get (const vector<string> & lines)
{
        unordered_map<string,vector<string>> devs;

        for (const string & line : lines)
        {
                istringstream ss(line);
                string name;

                ss >> name;
                name.pop_back();

                vector<string> vals;
                string out;
                while (ss >> out)
                        vals.emplace_back(out);

                devs.emplace(name, vals);
        };

        return (devs);
}

static bool
contains (unordered_map<string, uint64_t> & memo, string search)
{
        for (const auto & [l, _] : memo)
        {
                if (l == search)
                        return (true);
        };
        return (false);
};

static uint64_t 
eval_p1 (const unordered_map<string,vector<string>> & devs, const string & dev_name, const string & dest, unordered_map<string, uint64_t> & memo)
{
        const auto &outputs = devs.at(dev_name);
        uint64_t result{0};
        
        for (const auto & o : outputs)
        {
                if (contains(memo, o))
                        result += memo.at(o);
                else if (o == dest)
                        result++;
                else if (o != "out")
                        result += eval_p1(devs, o, dest, memo);
        };

        memo.emplace(dev_name, result);
        return (result);
};

static uint64_t
eval_p2 (const unordered_map<string,vector<string>> & devs)
{
        static array<array<string, 4>, 2> paths{"svr", "fft", "dac", "out", "svr", "dac", "fft", "out"};
        uint64_t p2_res {0};
        for (const auto & path : paths)
        {
                uint64_t p_res {0};
                unordered_map<string, uint64_t> m;
                for (auto i = path.rbegin(); i < path.rend() - 1; ++i)
                {
                        p_res = eval_p1(devs, *(i + 1), *i, m);
                        m = unordered_map<string, uint64_t>();
                        m.emplace(*(i + 1), p_res);
                };
                p2_res += p_res;
        };

        return (p2_res);
};

void
day11 (const char* fp)
{
        vector<string> lines;
        if (!parse(fp, lines))
                exit (1);

        auto e = get(lines);
        unordered_map<string, uint64_t> memo;

        auto eval_p1u = eval_p1(e, "you", "out", memo);
        cout << "Day 11 Part 01: " << eval_p1u << "\n";
        
        auto eval_p2u = eval_p2(e);
        cout << "Day 11 Part 02: " << eval_p2u << "\n";
}
