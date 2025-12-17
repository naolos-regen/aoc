#include "day10.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>
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
        light_diagram           target_ld;
        button_wiring           bw;
        joltage_requirements    target_joltage;

        uint32_t solve_min_presses_recursive(light_diagram current, button_wiring & best_path, size_t depth);
        uint32_t solve_min_presses_bfs();
        uint32_t solve_min_joltage_presses();
        light_diagram parse_light_diagram(string & line);
        button_wiring parse_button_wiring(vector<string> & lines);
        joltage_requirements parse_joltage_requirements(const string & line);
public:
        uint32_t light_res = 0;
        uint32_t joltage_res = 0;

        explicit Machine(const string & line)
        {
                this->parse(line);
        };

        void parse(const string & line);
        void min_light_presses();
        void min_joltage_presses();
};

joltage_requirements
Machine::parse_joltage_requirements(const string & line)
{
        joltage_requirements jr;
        
        string cleaned_line = line;

        if (!cleaned_line.empty() && cleaned_line[0] == '{')
                cleaned_line = cleaned_line.substr(1);

        if (!cleaned_line.empty() && cleaned_line.back() == '}')
                cleaned_line.pop_back();

        stringstream ss(cleaned_line);
        string token;
        while (getline(ss, token, ','))
        {
                try
                {
                        token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
                        if (!token.empty())
                                jr.push_back(std::stoul(token));
                }
                catch (const exception & e)
                {
                        cerr << "Error parsing joltage_requirements: " << token << endl;
                };
        };

        return (jr);
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
Machine::solve_min_joltage_presses()
{
        joltage_requirements initial(target_joltage.size(), 0);

        queue<pair<joltage_requirements, uint32_t>> states;
        set<joltage_requirements> visited;
        
        states.push({initial, 0});
        visited.insert(initial);

        constexpr int MAX_ITERATIONS = 100000;
        int iterations = 0;

        while (!states.empty() && iterations++ < MAX_ITERATIONS)
        {
                auto [cj, cp] = states.front();
                states.pop();

                if (cj == this->target_joltage)
                        return cp;

                for (const auto & b : bw)
                {
                        joltage_requirements next = cj;
                        
                        for (size_t idx : b)
                                if (idx < target_joltage.size())
                                        next[idx]++;

                        bool valid = true;
                        for (size_t i = 0; i < target_joltage.size(); ++i)
                        {
                                if (next[i] > target_joltage[i])
                                {
                                        valid = false;
                                        break;
                                };
                        };

                        if (!valid)
                                continue;

                        if (visited.insert(next).second)
                                states.push({next, cp + 1});
                };
        };

        return UINT32_MAX;
};

uint32_t
Machine::solve_min_presses_bfs()
{
        light_diagram initial(target_ld.size(), false);

        queue<pair<light_diagram, uint32_t>> states;

        set<light_diagram> visited;

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

        this->target_ld = parse_light_diagram(splitted[0]);

        vector<string> bws(splitted.begin() + 1, splitted.end() - 1);
        this->bw = parse_button_wiring(bws);

        this->target_joltage = parse_joltage_requirements(splitted[splitted.size() - 1]);
};

void
Machine::min_light_presses()
{
        this->light_res = solve_min_presses_bfs();
};

void
Machine::min_joltage_presses()
{
        this->joltage_res = solve_min_joltage_presses();
};


class MachineCollection
{
private:
        vector<Machine> machines;
        atomic<uint32_t> total_light_result{0};
        atomic<uint32_t> total_joltage_result{0};
        mutex output_mutex;
public:
        void load(const vector<string> & lines);
        pair<uint32_t, uint32_t> total_threaded(int num_threads);

};



pair<uint32_t, uint32_t>
MachineCollection::total_threaded(int num_threads = thread::hardware_concurrency())
{
        num_threads = num_threads > 0 ? num_threads : 4;

        total_light_result = 0;
        total_joltage_result = 0;

        {
                vector<thread> light_threads;

                for (Machine & machine : this->machines)
                {
                        light_threads.emplace_back([this, &machine]() 
                                        {
                                                machine.min_light_presses();
                                                total_light_result.fetch_add(machine.light_res, std::memory_order_relaxed);
                                        });
                };

                for (auto & thread : light_threads)
                        thread.join();
        }
        {
                vector<thread> joltage_threads;

                for (Machine & machine : this->machines)
                {
                        joltage_threads.emplace_back([this, &machine]()
                                        {
                                                machine.min_joltage_presses();
                                                total_joltage_result.fetch_add(machine.joltage_res, std::memory_order_relaxed);
                                        });
                };

                for (auto & thread : joltage_threads)
                        thread.join();
        }
        return { total_light_result.load(memory_order_relaxed), total_joltage_result.load(memory_order_relaxed) };
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

        pair<uint32_t, uint32_t> result = collect.total_threaded();

        cout << "Day 10 Part 01: " << result.first << endl;
        cout << "Day 10 Part 02: " << result.second << endl;
};

int main(void)
{
        day10("res/input.txt");
        return (0);
}
