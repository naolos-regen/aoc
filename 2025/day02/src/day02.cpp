#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
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
                                equal(id.begin() + seq_len, id.end(), id.begin()))
                        return (true);

        }

        return (false);
}

static pair<size_t, size_t>
solve_parts(
        const vector<pair<string, string>> * in,
        bool (*is_invalid_ID_p1) (const string & id),
        bool (*is_invalid_ID_p2) (const string & id)
)
{
        size_t invalid_p1 = 0;
        size_t invalid_p2 = 0;

        for (const auto& [first_ID, last_ID] : *in)
        {
                size_t start = stoull(first_ID);
                size_t end   = stoull(last_ID);

                for (size_t id = start; id <= end; ++id)
                {
                        string id_str = to_string(id);

                        if (is_invalid_ID_p1(id_str))
                                invalid_p1 += id;       
                        if (is_invalid_ID_p2(id_str))
                                invalid_p2 += id;
                }
        }

        return (make_pair(invalid_p1, invalid_p2));
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
read_file_to_single_line(const string& filename)
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
                pair<size_t, size_t> sum = solve_parts(&first_last_ID, is_invalid_ID, is_invalid_ID_inf_repeats);
                cout << "Day 02 Part 01: " << sum.first << endl;
                cout << "Day 02 Part 02: " << sum.second << endl;
        }
        catch (const std::exception & ex)
        {
                cerr << "something went wrong\n" << ex.what() << endl;
        }
}
