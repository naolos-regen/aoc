#include "fileHelpers.h"

using namespace std;

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
read_file_to_single_line(const string& filename)
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
