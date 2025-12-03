#include "../inc/day03.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

struct pair
{
        uint64_t left, right;
};

static uint_fast64_t
find_max_joltage(const char* bank_selection, unsigned int digit_amount)
{
        size_t n = strlen(bank_selection);
        if (digit_amount == 0 || digit_amount >= n)
                return (uint_fast64_t)-1;

        char* st = (char*) malloc(n + 1);
        if (!st) return (uint_fast64_t)-1;

        size_t top = 0;
        size_t to_remove = n - digit_amount;

        for (size_t i = 0; i < n; i++)
        {
                char c = bank_selection[i];
                while (top > 0 && to_remove > 0 && st[top - 1] < c)
                {
                        top--;
                        to_remove--;
                }
                st[top++] = c;
        }

        if (top > digit_amount)
                top = digit_amount;

        uint_fast64_t r = 0;
        for (size_t i = 0; i < top; i++)
                r = r * 10 + (st[i] - '0');

        free(st);
        return r;
}

static struct pair
calculate_total_joltage(const char** bank_selections)
{
        struct pair result = { .left = 0, .right = 0 };

        for (size_t i = 0; bank_selections[i] != NULL; i++)
        {
                const char* line = bank_selections[i];
                result.left = result.left + find_max_joltage(line, 2);
                result.right = result.right + find_max_joltage(line, 12);
        }

        return (result);
}

static const char**
read_file_to_single_line(const char* fp)
{
        FILE *file = fopen(fp, "r");

        if (!file)
                return (NULL);

        char** lines = NULL;
        size_t count = 0;
        char* line = NULL;
        size_t n = 0;

        while (getline(&line, &n, file) != -1)
        {
                size_t len = strlen(line);
                if (len > 0 && line[len - 1] == '\n')
                        line[len - 1] = '\0';

                char** tmp = (char**) realloc(lines, sizeof(char*) * (count + 2));
                if (!tmp)
                {
                        return (NULL); 
                }

                lines = tmp;
                lines[count++] = line;
                line = NULL;
                lines[count] = NULL;
        }

        free(line);
        fclose(file);
        return (const char**)lines;
}


void day03(const char* fp)
{
        const char** strings = read_file_to_single_line(fp);

        if (!strings)
                return;

        struct pair result = calculate_total_joltage (strings);

        printf("Day 03 Part 01: %ld\nDay 03 Part 02: %ld\n", result.left, result.right);

        for (size_t i = 0; strings[i]; i++)
                free((char*)strings[i]);
        free(strings);
}
