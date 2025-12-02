#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include "../inc/day02.h"

using namespace std;

static bool
is_invalid_ID(const string& id)
{
        const size_t len = id.length();
        if (len % 2 != 0) return false;

        size_t half = len / 2;

        if (id[0] == '0') return false;

        return id.substr(0, half) == id.substr(half);
}

static bool
is_invalid_ID_inf_repeats(const string & id)
{
        if (id[0] == '0') return (false);

        const size_t len = id.length();
        for (size_t seq_len = 1; seq_len <= len / 2; ++seq_len)
        {
                if (len % seq_len == 0 &&
                                std::equal(id.begin() + seq_len, id.end(), id.begin()))
                        return (true);

        }

        return (false);
}

static vector<size_t>
solve_part_01(const vector<pair<string, string>> * in)
{
        vector<size_t> invalid_IDs;

        for (const auto& [first_ID, last_ID] : *in)
        {
                size_t start = stoull(first_ID);
                size_t end   = stoull(last_ID);

                for (size_t id = start; id <= end; ++id)
                {
                        string id_str = to_string(id);

                        if (is_invalid_ID(id_str))
                                invalid_IDs.push_back(id);
                }
        }

        return (invalid_IDs);
}

static vector<size_t>
solve_part_02(const vector<pair<string, string>> * in)
{
        vector<size_t> invalid_IDs;

        for (const auto& [first_ID, last_ID] : *in)
        {
                size_t start = stoull(first_ID);
                size_t end   = stoull(last_ID);

                for (size_t id = start; id <= end; ++id)
                {
                        string id_str = to_string(id);

                        if (is_invalid_ID_inf_repeats(id_str))
                                invalid_IDs.push_back(id);
                }

        }

        return (invalid_IDs);
}

static vector<pair<string, string>>
parse_to_pair(const vector<string>& in)
{
        vector<pair<string, string>> out;

        transform(in.begin(), in.end(),
                  back_inserter(out),
                  [](const string & str)
        {
                size_t dash_pos = str.find('-');
                if (dash_pos == string::npos)
                        throw invalid_argument("Invalid range format, no dash seperator");

                return std::make_pair
                       (
                               str.substr(0, dash_pos),
                               str.substr(dash_pos + 1)
                       );
        });
        return (out);
}

static vector<string>
split_ranges(const string& input)
{
        vector<string> ranges;
        stringstream ss(input);
        string range;

        while (getline(ss, range, ','))
        {
                range.erase(0, range.find_first_not_of(" "));
                range.erase(range.find_last_not_of(" ") + 1);

                if (!range.empty())
                {
                        ranges.push_back(range);
                }
        }

        return (ranges);
}

static vector<string>
read_file_to_single_line(const std::string& filename)
{
        ifstream file(filename);
        if (!file.is_open())
                throw std::runtime_error("Could not open file: " + filename);

        string line;
        vector<string> result;

        getline(file, line);

        return (split_ranges(line));
}

extern "C" void
day02(const char* fp)
{
        try
        {
                vector<string> fileString = read_file_to_single_line(fp);
                vector<pair<string, string>> first_last_ID = parse_to_pair(fileString);
                vector<size_t> invalid_IDs_p1 = solve_part_01(&first_last_ID);
                vector<size_t> invalid_IDs_p2 = solve_part_02(&first_last_ID);
                size_t sum_p1 = std::accumulate(invalid_IDs_p1.begin(), invalid_IDs_p1.end(), (size_t)0);
                size_t sum_p2 = std::accumulate(invalid_IDs_p2.begin(), invalid_IDs_p2.end(), (size_t) 0);
                cout << "Day 02 Part 01: " << sum_p1 << endl;
                cout << "Day 02 Part 02: " << sum_p2 << endl;
        }
        catch (const std::exception & ex)
        {
                cerr << "something went wrong\n" << ex.what() << endl;
        }
}
