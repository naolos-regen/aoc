#include "day06.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <limits>
#include <cctype>
#include <algorithm>

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
                        if (num == 0)
                                break;
                        this->result = this->result * num;
                        break;
                }
        }
        friend ostream & operator << (ostream &os, const Eval& obj)
        {
                os << "Operator: " << static_cast<int>(obj.op) << ", Result: " << obj.result;
                return os;
        }
        Eval * clone() const
        {
                return new Eval(this->op, this->result);
        }
        Eval * clone_reset() const
        {
                return new Eval(this->op);
        }
};

static istream &
ignore_line(ifstream & in, ifstream::pos_type &pos)
{
        pos = in.tellg();
        return (in.ignore(numeric_limits<streamsize>::max(), '\n'));
}

static string
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
        return (line);
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

static pair<vector<Eval>, vector<Eval>> 
parse_day06_p01(const string &fp)
{
        vector<Eval *> * evaluators = new vector<Eval *>();

        ifstream in(fp);
        if (!in)
                exit(1);

        string last = get_last_line(in);
        for (const char c : last)
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
                if (line == last)
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
                                                evaluators->at(index)->operate(res);
                                        }
                                        res = 0;
                                        index++;
                                }
                        }
                }
                index = 0;
        }

        vector<Eval> evaluators_p1;
        for (Eval *ev : *evaluators)
        {
                Eval e(ev->op, ev->result);
                evaluators_p1.push_back(e);
        }

        for (Eval *ev : *evaluators)
        {
                ev->result = ev->op == MUL ? 1 : 0;
        }

        in.clear();
        in.seekg(0);
        vector<string> lines;

        while (getline(in, line))
        {
                if (line == last)
                        continue;
                lines.push_back(line);
        }

        size_t max_len = 0;
        for (const auto & line : lines)
        {
                max_len = max(max_len, (line.length()));
        }

        int index = 0;
        for (int col = 0; col < max_len; ++col)
        {
                uint_fast64_t res = 0;
                for (const auto &line : lines)
                {
                        if (col < line.length())
                        {
                                char c = line[col];
                                if (isdigit(c))
                                        res = res * 10 + (c - '0');
                        }
                }
                evaluators->at(index)->operate(res);
                if (res == 0)
                        index++;
        }

        vector<Eval> evaluators_p2;
        for (Eval *ev : *evaluators)
        {
                Eval e(ev->op, ev->result);
                evaluators_p2.push_back(e);
                delete ev;
        }
        
        evaluators->clear();
        delete evaluators;
        pair<vector<Eval>, vector<Eval>> p{evaluators_p1, evaluators_p2};
        in.close();
        return (p);
}

void
day06(const char* fp)
{
        auto evaluators = parse_day06_p01(fp);
        uint_fast64_t result_p01 = 0;
        for (const Eval ev : evaluators.first)
        {
                result_p01 = result_p01 + ev.result;
        }

        uint_fast64_t result_p02 = 0;
        for (const Eval ev : evaluators.second)
        {
                result_p02 = result_p02 + ev.result;
        }

        cout << "Day 06 Part 01: " << result_p01 << endl;
        cout << "Day 06 Part 02: " << result_p02 << endl;
}
