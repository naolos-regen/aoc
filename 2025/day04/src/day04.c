#include "../inc/day04.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

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
        if (pos2.row < 0 || pos2.row >= pattern->row_length||
                        pos2.col < 0 || pos2.col >= pattern->col_length)
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
        if (pos2.row < 0 || pos2.row >= pattern->row_length ||
                        pos2.col < 0 || pos2.col >= pattern->col_length)
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

#define FOR(i, imax)\
        for (i = 0; i < (imax); i++)

static int32_t
get_all_matching_neighbours(t_pos2 index, char match)
{
        int32_t result = 0;
        char index_char = get_data_from(index);
        if (index_char == '.')
                return (0);

        int i;
        FOR(i, ADJACEMENTS_LEN)
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

static void
step(t_pos2 index, int32_t* result)
{
        char current_char = get_data_from(index);
        if (current_char == '@')
        {
                int32_t neighbours = get_all_matching_neighbours(index, '@');
                if (neighbours < 4)
                {
                        (*result)++;
                }
        }
}

static void
x_die(t_pos2 index)
{
        char current_char = get_data_from(index);
        if (current_char == 'x')
                set_data_at(index, '.');
}

static void
die(t_pos2 index)
{
        char current_char = get_data_from(index);
        if (current_char == '@')
        {
                int32_t neighbours = get_all_matching_neighbours(index, '@');
                if (neighbours < 4)
                {
                        set_data_at(index, 'x');
                }
        }
}

#define DOUBLE_FOR(i, imax, j, jmax) \
                for (i = 0; i < (imax); i++) \
                        for (int j = 0; j < (jmax); j++)


static void
print_pattern(int32_t frame)
{
        int scale = 15;
        char buf[256];
        snprintf(buf, sizeof(buf), "vid/output-%09d.ppm", frame);
        FILE *f = fopen(buf,"wb");
        if (!f)
        {
                perror("fopen");
                return;
        }

        int width  = pattern->col_length * scale;
        int height = pattern->row_length * scale;

        fprintf(f, "P6\n%d %d\n255\n", width, height);

        for (int row = 0; row < pattern->row_length; row++)
        {
                for (int srow = 0; srow < scale; srow++)
                {
                        for (int col = 0; col < pattern->col_length; col++)
                        {
                                t_pos2 pos = { .row = row, .col = col };
                                char c = get_data_from(pos);
                                unsigned char rgb[3];

                                if (c == '@')
                                {
                                        rgb[0]=0xd0;
                                        rgb[1]=0xb8;
                                        rgb[2]=0x92;
                                }
                                else if (c == '.')
                                {
                                        rgb[0]=0x06;
                                        rgb[1]=0x26;
                                        rgb[2]=0x25;
                                }
                                else
                                {
                                        rgb[0]=0x54;
                                        rgb[1]=0x49;
                                        rgb[2]=0x3a;
                                }

                                for (int scol = 0; scol < scale; scol++)
                                        fwrite(rgb, 1, 3, f);
                        }
                }
        }

        fclose(f);
        printf("generated %s\n", buf);
}

#define SLEEP_MS(milliseconds)\
                usleep(milliseconds * 1000);

static int32_t
step_round()
{
        int row, col;
        int32_t res = 0;
        static uint32_t counter = 0;

        DOUBLE_FOR(row, pattern->row_length, col, pattern->col_length)
                die((t_pos2) { row, col });

        DOUBLE_FOR(row, pattern->row_length, col, pattern->col_length)
        {
                t_pos2 pos = {row, col};
                char c = get_data_from(pos);
                if (c == 'x')
                {
                        set_data_at(pos, '.');
                        res++;
                }
        }
        return (res);
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
        int32_t result_part_02 = 0;

        int32_t result;
        while (1)
        {
                result = step_round();
                if (result == 0)
                        break;
                result_part_02 = result_part_02 + result;
        }

        free(pattern);

        pattern = read_pattern_file(fp);
        int32_t result_part_01 = step_round();

        free(pattern);
        printf("Day 04 Part 01: %d\n", result_part_01);
        printf("Day 04 Part 02: %d\n", result_part_02);
}
