#ifndef HELPERS_H
#define HELPERS_H

#include <vector>
#include <string>
#include <algorithm>
#include <fstream>


std::vector<std::string> parse_args              (int argc, char* argv[]);
std::vector<std::string> read_file_to_single_line(const std::string& filename);


#endif
