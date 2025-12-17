#include "day10.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <set>
#include <algorithm>

using namespace std;

using light = bool;
using light_diagram = vector<light>;

using buttons = vector<uint32_t>;
using button_wiring = vector<buttons>;

using joltage_requirements = vector<uint32_t>;

static vector<string>
split (const string & str, char c)
{
        vector<string> tokens;
        string token;
        istringstream tokenStream(str);

        while (getline(tokenStream, token, c))
        {
                if (!token.empty())
                {
                        tokens.push_back(token);
                };
        };
        return (tokens);
};

uint64_t counter = 0;

class Machine
{
private:
        light_diagram   target_ld;
        button_wiring   bw;

        uint32_t solve_min_presses_recursive(light_diagram current, button_wiring & best_path, size_t depth);
        uint32_t solve_min_presses_bfs();
        light_diagram parse_light_diagram(string & line);
        button_wiring parse_button_wiring(vector<string> & lines);
public:
        uint32_t res = 0;

        explicit Machine(const string & line)
        {
                this->parse(line);
        };

        void parse(const string & line);
        void solve_min_presses();
};

light_diagram
Machine::parse_light_diagram(string & line)
{
        light_diagram ld;

        if (!line.empty() && line[0] == '[')
                line = line.substr(1);
        if (!line.empty() && line.back() == ']')
                line.pop_back();

        for (char c : line)
        {
                switch (c)
                {
                case '.' :
                        ld.emplace_back(false);
                        break;
                case '#' :
                        ld.emplace_back(true);
                        break;
                default:
                        break;
                };
        };

        return (ld);
};

button_wiring
Machine::parse_button_wiring(vector<string> & lines)
{
        button_wiring bw;

        for (string & line : lines)
        {
                if (!line.empty() && line[0] == '(')
                        line = line.substr(1);
                if (!line.empty() && line.back() == ')')
                        line.pop_back();

                buttons b;
                string curr_num;

                for (char c : line)
                {
                        if (std::isdigit(c))
                                curr_num += c;
                        else if (c == ',' || curr_num.empty() == false)
                        {
                                if (!curr_num.empty())
                                {
                                        try
                                        {
                                                b.emplace_back(std::stoul(curr_num));
                                                curr_num.clear();
                                        }
                                        catch (const exception & e)
                                        {
                                                cerr << "Error parsing button: " << curr_num << endl;
                                        };
                                };
                        };
                };
                if (!curr_num.empty())
                {
                        try
                        {
                                b.emplace_back(std::stoul(curr_num));
                        }
                        catch (const exception & e)
                        {
                                cerr << "Error parsing button: " << curr_num << endl;
                        };
                };
                if (!b.empty())
                {
                        bw.emplace_back(b);
                };
        };

        return (bw);
};

uint32_t
Machine::solve_min_presses_bfs()
{
        light_diagram initial(target_ld.size(), false);

        std::queue<pair<light_diagram, uint32_t>> states;

        std::set<light_diagram> visited;

        states.push({initial, 0});
        visited.insert(initial);

        const int MAX_ITERATIONS = 100000;
        int iterations = 0;

        while (!states.empty() && iterations++ < MAX_ITERATIONS)
        {
                auto [cl, cp] = states.front();
                states.pop();

                if (cl == target_ld)
                {
                        return cp;
                }

                for (const auto & b : bw)
                {
                        light_diagram nl = cl;

                        for (size_t idx : b)
                        {
                                if (idx < nl.size())
                                {
                                        nl[idx] = !nl[idx];
                                }
                        }

                        if (visited.find(nl) == visited.end())
                        {
                                states.push({nl, cp + 1});
                                visited.insert(nl);
                        }
                }
        }
        return UINT32_MAX;
};

uint32_t
Machine::solve_min_presses_recursive(light_diagram current, button_wiring & best_path, size_t depth)
{
        if (current == this->target_ld)
                return (0);

        if (depth > 10)
                return UINT32_MAX;

        uint32_t min_presses = UINT32_MAX;
        button_wiring best_local_path;

        for (const auto & button : bw)
        {
                light_diagram next = current;

                for (size_t idx : button)
                {
                        if (idx < next.size())
                                next[idx] = !next[idx];
                };

                uint32_t sub_process = solve_min_presses_recursive(next, best_local_path, depth + 1);

                if (sub_process != UINT32_MAX)
                {
                        uint32_t total_presses = sub_process + 1;
                        if (total_presses < min_presses)
                        {
                                min_presses = total_presses;
                                best_path = best_local_path;
                                best_path.insert(best_path.begin(), button);
                        };
                };
        };

        return min_presses;
};

void
Machine::parse(const string & line)
{
        vector<string> splitted = split(line, ' ');

        if (splitted.size() < 3)
                return;

        target_ld = parse_light_diagram(splitted[0]);
        vector<string> bws(splitted.begin() + 1, splitted.end() - 1);
        bw = parse_button_wiring(bws);
};

void
Machine::solve_min_presses()
{
        res = solve_min_presses_bfs();
};

class MachineCollection
{
private:
        vector<Machine> machines;
        atomic<uint32_t> total_result{0};
        mutex output_mutex;
public:
        void load(const vector<string> & lines);
        uint32_t total_threaded();

};



uint32_t
MachineCollection::total_threaded()
{
        vector<thread> threads;

        total_result = 0;

        for (Machine & machine : machines)
        {
                threads.emplace_back([this, &machine]()
                {
                        machine.solve_min_presses();
                        total_result.fetch_add(machine.res, std::memory_order_relaxed);

                        {
                                lock_guard<mutex> lock(this->output_mutex);
                        }

                });

        };

        for (auto & thread : threads)
        {
                thread.join();
        };

        return total_result.load(std::memory_order_relaxed);
};

void
MachineCollection::load(const vector<string> & lines)
{
        this->machines.reserve(lines.size());

        for (const auto & l : lines)
        {
                this->machines.emplace_back(Machine(l));
        };
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
        in.close();
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

        uint32_t result = collect.total_threaded();

        cout << "Day 10 Part 01: " << result << endl;
};

int main(void)
{
        day10("res/input.txt");
        return (0);
}
