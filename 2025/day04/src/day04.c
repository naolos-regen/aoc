#include "../inc/day04.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ADJACEMENTS_LEN 8

typedef struct s_pos2
{
        int_fast32_t row;
        int_fast32_t col;
} t_pos2;

typedef struct s_pattern
{
        char* data;
        uint32_t total_length;
        uint32_t row_length;
        uint32_t col_length;
} t_pattern;

t_pos2* adjacements;
t_pattern *pattern;

/*
static int32_t
wrap_index(int32_t index, uint32_t length)
{
        return (int32_t)fmod(index, (int32_t)length + (index < 0 ? (int32_t)length : 0));
}
*/

static int32_t
get_pos_at_a(int32_t row, int32_t col)
{
        return (row) * pattern->col_length + (col);
}

static int32_t
get_pos_at_b(t_pos2 pos2)
{
        return  (pos2.row) * pattern->col_length + (pos2.col);
}

static t_pos2
get_pos2_at(int32_t index)
{
        return ((t_pos2)
        {
                .row = index / pattern->col_length,
                .col = index % pattern->col_length
        });
}

static char
get_data_from(t_pos2 pos2)
{
        if (pos2.row < 0 || pos2.row >= pattern->col_length ||
                        pos2.col < 0 || pos2.col >= pattern->row_length)
        {
                return '.';
        }

        uint32_t index = get_pos_at_b(pos2);
        if (index < pattern->total_length && pattern->data[index])
                return pattern->data[index];
        else
                return '.';
}

static void
set_data_at(t_pos2 pos2, char c)
{
        if (pos2.row < 0 || pos2.row >= pattern->col_length ||
                        pos2.col < 0 || pos2.col >= pattern->row_length)
        {
                return;
        }

        uint32_t index = get_pos_at_b(pos2);
        if (index < pattern->total_length && pattern->data[index])
                pattern->data[index] = c;
}

static void
set_all_adjacements()
{
        adjacements = (t_pos2 *) malloc(sizeof(t_pos2) * ADJACEMENTS_LEN);

// *INDENT-OFF*
        adjacements[0] = (t_pos2) { .row = -1, .col = -1 };
        adjacements[1] = (t_pos2) { .row = -1, .col =  0 };
        adjacements[2] = (t_pos2) { .row = -1, .col =  1 };
        adjacements[3] = (t_pos2) { .row =  0, .col = -1 };
        adjacements[4] = (t_pos2) { .row =  0, .col =  1 };
        adjacements[5] = (t_pos2) { .row =  1, .col = -1 };
        adjacements[6] = (t_pos2) { .row =  1, .col =  0 };
        adjacements[7] = (t_pos2) { .row =  1, .col =  1 };
// *INDENT-ON*
}

static int32_t
get_all_matching_neighbours(t_pos2 index, char match)
{
        int32_t result = 0;
        char index_char = get_data_from(index);
        if (index_char == '.')
                return (0);

        for (int i = 0; i < ADJACEMENTS_LEN; i++)
        {
                t_pos2 position =
                {
                        .row = (adjacements[i].row + index.row),
                        .col = (adjacements[i].col + index.col)
                };
                char current_char = get_data_from(position);

                if (current_char == match)
                {
                        result++;
                }
        }
        return (result);
}


static struct s_pattern*
read_pattern_file(const char* filename)
{
        FILE* file = fopen(filename, "r");
        if (!file)
        {
                perror("Error opening file");
                return NULL;
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        rewind(file);

        t_pattern* pattern = (t_pattern*) malloc(sizeof(struct s_pattern));
        if (!pattern)
        {
                perror("Memory allocation failed for pattern struct");
                fclose(file);
                return NULL;
        }

        pattern->data = (char*) malloc(file_size + 1);
        if (!pattern->data)
        {
                perror("Memory allocation failed for pattern data");
                free(pattern);
                fclose(file);
                return NULL;
        }

        pattern->total_length = 0;
        pattern->row_length = 0;
        pattern->col_length = 0;

        int c;
        int current_row_length = 0;
        while ((c = fgetc(file)) != EOF)
        {
                if (c == '\n')
                {
                        if (pattern->row_length == 0)
                        {
                                pattern->row_length = current_row_length;
                        }
                        pattern->col_length++;
                        current_row_length = 0;
                }
                else
                {
                        pattern->data[pattern->total_length++] = c;
                        current_row_length++;
                }
        }

        if (current_row_length > 0)
        {
                pattern->col_length++;
        }

        pattern->data[pattern->total_length] = '\0';

        fclose(file);
        return pattern;
}


void
day04 (const char* fp)
{
        pattern = read_pattern_file(fp);
        set_all_adjacements();
        for (int x = 0; x < pattern->col_length; x++)
        {
                for (int y = 0; y < pattern->row_length; y++)
                {
                        printf("%c", get_data_from((t_pos2)
                        {
                                .row = x, .col=y
                        }));
                }
                printf("\n");
        }

        uint32_t final_result = 0;
        for (int x = 0; x < pattern->col_length; x++)
        {
                for (int y = 0; y < pattern->row_length; y++)
                {
                        t_pos2 current_pos = (t_pos2)
                        {
                                .row = x, .col = y
                        };

                        if (get_data_from(current_pos) == '@')
                        {
                                int32_t adjacent_rolls = get_all_matching_neighbours(current_pos, '@');
                                printf("%d", adjacent_rolls);

                                if (adjacent_rolls < 4)
                                {
                                        final_result++;
                                }
                        }
                        else
                        {
                                printf(".");
                        }
                }
                printf("\n");
        }


        printf("\nTotal_Length :: %d\nRow_Length :: %d\nCol_Length %d\n", pattern->total_length, pattern->row_length, pattern->col_length);

        printf("Day 01 : Part 01: %d\n", final_result);
        free(adjacements);
        free(pattern);
}

int
main (void)
{
        day04("res/input.txt");
}
