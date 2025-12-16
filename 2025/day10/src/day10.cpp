#include "day10.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <queue>
#include <set>

using namespace std;

using light = bool;
using light_diagram = vector<light>;

using buttons = vector<uint32_t>;
using button_wiring = vector<buttons>;

using joltage_requirements = vector<uint32_t>;

static vector<string>
split (const string & str, char c)
{
        stringstream ss(str);
        vector<string> res;
        string token;
        while (getline(ss, token, c))
                res.emplace_back(token);
        return (res);
};

uint64_t counter = 0;

class Machine
{
private:
        light_diagram           immutable_ld;
        light_diagram           mutable_ld;

        button_wiring           bw;
        joltage_requirements    jr;

        light_diagram           parse_light_diagram             (string & line);
        button_wiring           parse_button_wiring             (vector<string> & line);
        joltage_requirements    parse_joltage_requirements      (string & line);
public:
        uint32_t                res = 0;
        Machine() = default;

        explicit Machine(const string & line)
        {
                this->parse(line);
                cout << "Parsing " << counter++ << "\n";
                this->mutable_ld = immutable_ld;
        };

        void parse(const string & line);
        void toggle(buttons & bw);
        void solve_min_presses();
        static bool is_all_off(light_diagram ld);
        button_wiring shortest();
};


class MachineCollection
{
private:
        vector<Machine> machines;
public:
        MachineCollection() = default;

        void load(const vector<string> & lines);

        uint32_t total();
};

uint32_t
MachineCollection::total()
{
        uint32_t res = 0;

        for (const Machine & m : this->machines)
        {
                res = res + m.res;
        };

        return (res);
};

void
MachineCollection::load(const vector<string> & lines)
{
        for (const auto & l : lines)
        {
                Machine t(l);
                t.solve_min_presses();
                this->machines.emplace_back(t);
        };
};

void
Machine::solve_min_presses() 
{
        this->res = static_cast<uint32_t>(this->shortest().size());
};

bool
Machine::is_all_off(light_diagram ld)
{
        return std::none_of
        (
/*Begin Iter*/ ld.begin(),
/*End   Iter*/ ld.end(),
/*Lambda Exp*/ [](const bool x) { return (x); }
        );

};

uint64_t iterating = 0;

button_wiring
Machine::shortest()
{
        queue<pair<light_diagram, button_wiring>> queue;
        set<light_diagram>                        visited;

        queue.push({this->mutable_ld, {}});

        while (!queue.empty())
        {
                auto [cl, cp] = queue.front();
                queue.pop();

                if (Machine::is_all_off(cl))
                {
                        cout << "Done " << cp.size() << "\n";
                        return {cp};
                };
                for (const auto & b : this->bw)
                {
                        light_diagram nl = cl;

                        for (size_t idx : b)
                                nl[idx] = !nl[idx];

                        if (visited.find(nl) == visited.end())
                        {
                                button_wiring np = cp;
                                np.emplace_back(b);
                                queue.push({nl, np});

                        };
                };

        };

        return {};
};

void
Machine::parse(const string & line)
{
        vector<string> splitted = split (line, ' ');
        vector<string> bws(splitted.begin() + 1, splitted.begin() + splitted.size() - 1);

        this->immutable_ld      = this->parse_light_diagram        (splitted[0]);
        this->bw                = this->parse_button_wiring        (bws);
        this->jr                = this->parse_joltage_requirements (splitted[splitted.size() - 1]);
};

joltage_requirements
Machine::parse_joltage_requirements (string & line)
{
        joltage_requirements jr;

        uint32_t num = 0;
        for (const char & c : line)
        {
                if (std::isdigit(c))
                        num = num * 10 + (c - '0');
                if (c == ',' || c == '}')
                {
                        jr.emplace_back(num);
                        num = 0;
                };
        };
        return (jr);
};

button_wiring
Machine::parse_button_wiring(vector<string> & lines)
{
        button_wiring bw;

        for (const auto & line : lines)
        {
                buttons b{};
                uint32_t curr = 0;

                for (const char & c : line)
                {
                        if (std::isdigit(c))
                                curr = curr * 10 + (c - '0');
                        else if (c == ',' || c == ')')
                        {
                                b.emplace_back(curr);
                                curr = 0;
                        }
                };
                bw.emplace_back(b);
        };
        return (bw);
};

light_diagram
Machine::parse_light_diagram(string & line)
{
        light_diagram ld;
                
        for (const char & c : line)
        {
                switch (c)
                {
                        case '.': ld.emplace_back(false); break;
                        case '#': ld.emplace_back(true);  break;
                        default : continue;
                };
        };

        return (ld);
};


void 
Machine::toggle(buttons & bw)
{
        light_diagram tmp(this->mutable_ld.size(), false);
        for (const auto & n : bw)
                tmp[n] = true;
        for (size_t i = 0; i < this->mutable_ld.size(); ++i)
                mutable_ld[i] = mutable_ld[i] ^ tmp[i];

};

static bool
read_file (const string & fp, vector<string> & lines)
{
        ifstream in{fp};
        if (!in)
                return (false);
        string line;
        while (getline(in, line))
                lines.emplace_back(line);
        return (true);
};

extern "C" void 
day10(const char* fp)
{
        vector<string> lines;
        if (!read_file(fp, lines))
                exit(1);
        
        MachineCollection collect{};

        collect.load(lines);

        uint32_t result = collect.total();

        cout << "Day 10 Part 01: " << result << endl;
};

int main(void)
{
        day10("res/input.txt");
        return (0);
}
