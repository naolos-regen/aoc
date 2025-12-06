#include "../inc/day06.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <limits>
#include <cctype>

using namespace std;

enum operators
{
        ADD,
        MUL
};

class Eval
{
public:
        operators       op;
        uint_fast64_t   result;

        Eval(operators op) : op(op), result(op == MUL ? 1 : 0) {}
        Eval(operators op, uint_fast64_t res) : op(op), result(res) {}
        ~Eval()=default;
        void operate(uint_fast64_t num)
        {
                switch (this->op)
                {
                case ADD:
                        this->result = this->result + num;
                        break;
                case MUL:
                        this->result = this->result * num;
                        break;
                }
        }
        friend ostream & operator << (ostream &os, const Eval& obj)
        {
                os << "Operator: " << static_cast<int>(obj.op) << ", Result: " << obj.result;
                return os;
        }
};

static istream &
ignore_line(ifstream & in, ifstream::pos_type &pos)
{
        pos = in.tellg();
        return (in.ignore(numeric_limits<streamsize>::max(), '\n'));
}

static pair<string, ifstream::pos_type>
get_last_line(ifstream &in)
{
        ifstream::pos_type pos = in.tellg();

        ifstream::pos_type last_pos;
        while (in >> ws && ignore_line(in, last_pos))
                pos = last_pos;

        in.clear();
        in.seekg(pos);

        string line;
        getline(in, line);
        return make_pair(line, pos);
}

static string
trim_spaces(const string &in)
{
        string result;
        bool lastWasSpace = false;

        for (char c : in)
        {
                if (isspace(c))
                {
                        if (!lastWasSpace)
                        {
                                result += ' ';
                                lastWasSpace = true;
                        }
                }
                else
                {
                        result += c;
                        lastWasSpace = false;
                }
        }

        if (!result.empty() && result.back() == ' ')
                result.pop_back();

        return result;
}

static vector<Eval *> *
parse_day06_p01(const string &fp)
{
        vector<Eval *> * evaluators = new vector<Eval *>();

        ifstream in(fp);
        if (!in)
                return (nullptr);

        pair<string, ifstream::pos_type> last = get_last_line(in);
        for (const char c : last.first)
        {
                if (isspace(c))
                        continue;
                if (c == '*')
                {
                        evaluators->push_back(new Eval(MUL));
                }
                if (c == '+')
                {
                        evaluators->push_back(new Eval(ADD));
                }
        }

        in.clear();
        in.seekg(0);
        string line;
        while (getline(in, line))
        {
                if (line == last.first)
                        continue;
                line = trim_spaces(line);
                uint_fast64_t res = 0;
                size_t index = 0;
                for (size_t i = 0; i < line.length(); i++)
                {
                        char c = line[i];
                        if (isdigit(c))
                        {
                                res = res * 10 + (c - '0');
                        }
                        if (isspace(c) || i == line.length() - 1)
                        {
                                if (res > 0)
                                {
                                        if (index < evaluators->size())
                                        {
                                                Eval *ev = evaluators->at(index);
                                                ev->operate(res);
                                        }
                                        res = 0;
                                        index++;
                                }
                        }
                }
                index = 0;
        }
        return (evaluators);
}

void day06(const char* fp)
{
        vector<Eval *> * evaluators = parse_day06_p01(fp);
        uint_fast64_t result_p01 = 0;
        for (const Eval * ev : *evaluators)
        {
                result_p01 = result_p01 + ev->result;
        }
        for (const Eval * ev : *evaluators)
        {
                delete ev;
        }
        delete evaluators;
        cout << "Day 06: Part 01: " << result_p01 << endl;
}

int main(void)
{
        day06("res/input.txt");
}
