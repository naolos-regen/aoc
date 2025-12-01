#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;
#define MAX   100
#define MIN   0
#define START 50

vector<pair<int, char>>
part01_parse(vector<string> in)
{
        vector<pair<int, char>> out;

        transform(in.begin(), in.end(),
                  back_inserter(out),
                  [](const string &line)
        {
                return make_pair(stoi(line.substr(1)), line[0]);
        });

        return (out);
}

int
part01_solve(const vector<pair<int, char>>& in)
{
        int dial = START;
        int result = 0;

        for (const auto& [rotation, direction] : in)
        {
                if (direction == 'L')
                        dial = (dial - rotation + MAX) % MAX;
                else if (direction == 'R')
                        dial = (dial + rotation) % MAX;
                if (dial == 0)
                        result = result + 1;
        }
        return (result);
}

int
part02_solve(const vector<pair<int, char>>& in)
{
        int dial = START;
        int result = 0;

        for (auto [rotation, direction] : in)
        {
                int start = dial;
                int crosses = rotation / MAX;
                int rem = rotation % MAX;

                if (direction == 'R')
                {
                        int k = (MAX - start) % MAX;
                        if (k != 0 && k < rem)
                                crosses = crosses + 1;
                        dial = (start + rotation) % MAX;
                }
                else if (direction == 'L')
                {
                        int k = start;
                        if (k != 0 && k < rem)
                                crosses = crosses + 1;
                        dial = (start - rem + MAX) % MAX;
                }

                result = result + crosses;

                if (dial == 0)
                        result = result + 1;
        }

        return result;
}

vector<string>
parse_args(int argc, char* argv[])
{
        vector<string> args(argc);
        transform(argv, argv + argc, args.begin(), [](char* arg)
        {
                return string(arg);
        });
        return (args);
}

vector<string>
read_file_to_single_line(const std::string& filename)
{
        ifstream file(filename);
        if (!file.is_open())
                throw std::runtime_error("Could not open file: " + filename);

        string line;
        vector<string> result;
        bool first_line = true;

        while (getline(file, line))
        {
                if (!line.empty())
                        result.push_back(line);
        }
        return (result);
}

int
main(int argc, char* argv[])
{
        if (argc == 1)
                return (1);
        vector<string> args = parse_args(argc, argv);
        try
        {
                vector<string> file_contents = read_file_to_single_line(args[1]);
                vector<pair<int, char>> mapped = part01_parse(file_contents);
                int result_part1 = part01_solve(mapped);
                int result_part2 = part02_solve(mapped);
                cout << result_part1 << endl;
                cout << result_part2 << endl;
        }
        catch (const std::exception& e)
        {
                cerr << "Error: " << e.what() << endl;
                return (1);
        }
        return (0);
}
