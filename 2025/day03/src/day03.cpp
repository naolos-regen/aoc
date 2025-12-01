#include "day03.h"
#include <fstream>
#include <iostream>
#include <string>

void day03(const char *fp) {
    std::ifstream in(fp);
    if (!in) {
        std::cerr << "Failed to open " << fp << std::endl;
        return;
    }

    std::string line;
    while (std::getline(in, line)) {
        std::cout << line << std::endl;
    }
}
