#include "day05.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

struct interval
{
        uint64_t        low;
        uint64_t        high;
};

struct i_tree_node
{
        struct interval         *interval;
        uint64_t                max;
        struct i_tree_node      *left;
        struct i_tree_node      *right;
};

struct pattern
{
        struct i_tree_node *root;
        uint64_t*           ids;
        size_t              ids_len;
};

static void*
x_malloc (size_t size)
{
        void* allocated = malloc (size);
        if (!allocated)
        {
                exit(1);
        }
        return (allocated);
}

static struct interval *
new_interval (struct interval i)
{
        struct interval *ret = (struct interval *) x_malloc (sizeof(struct interval));
        ret->high = i.high;
        ret->low  = i.low;
        return (ret);
}

static struct i_tree_node *
new_tree_node (struct interval ivl)
{
        struct i_tree_node *tmp = (struct i_tree_node *) x_malloc (sizeof(struct i_tree_node));
        struct interval *i = new_interval(ivl);

        tmp->interval = i;
        tmp->max = ivl.high;
        tmp->left = tmp->right = NULL;
        return (tmp);
}

static struct i_tree_node *
insert (struct i_tree_node *root, struct interval ivl)
{
        if (root == NULL)
                return new_tree_node (ivl);

        int low = root->interval->low;

        if (ivl.low < low)
        {
                root->left = insert (root->left, ivl);
        }

        else
        {
                root->right = insert (root->right, ivl);
        }

        if (root->max < ivl.high)
                root->max = ivl.high;

        return (root);
}

static int
is_overlapping (struct interval ivl1, struct interval ivl2)
{
        if (ivl1.low <= ivl2.high && ivl2.low <= ivl1.high)
                return (1);
        return (0);
}

static struct interval *
search (struct i_tree_node *root, struct interval ivl)
{
        if (root == NULL)
                return (NULL);

        if (is_overlapping (*(root->interval), ivl))
                return (root->interval);

        if (root->left != NULL && root->left->max >= ivl.low)
                return search (root->left, ivl);

        return search (root->right, ivl);
}

static struct interval *
search_id (struct i_tree_node *root, uint64_t id)
{
        if (root == NULL)
        {
                return NULL;
        }
        if (root->interval->low <= id && id <= root->interval->high)
        {
                return root->interval;
        }
        if (root->left != NULL && root->left->max >= id)
        {
                struct interval *left_result = search_id(root->left, id);
                if (left_result != NULL)
                {
                        return left_result;
                }
        }
        return search_id(root->right, id);
}

static int
compare_intervals(const void* a, const void* b)
{
        const struct interval *x = (const struct interval *)a;
        const struct interval *y = (const struct interval *)b;

        return (x->low > y->low) - (x->low < y->low);
}

static void
collect_intervals (struct i_tree_node *root, struct interval *intervals, size_t* n)
{
        if (!root) return;

        intervals[*n].low  = root->interval->low;
        intervals[*n].high = root->interval->high;
        (*n)++;

        collect_intervals(root->left,  intervals, n);
        collect_intervals(root->right, intervals, n);
}


static uint64_t
count_unique_ids (struct interval *arr, size_t n)
{
        if (n == 0)
                return 0;

        qsort(arr, n, sizeof(arr[0]), compare_intervals);

        uint64_t total = 0;
        uint64_t low = arr[0].low;
        uint64_t high = arr[0].high;

        for (size_t i = 1; i < n; i++)
        {
                if (arr[i].low <= high + 1)
                {
                        if (arr[i].high > high)
                                high = arr[i].high;
                }
                else
                {
                        total += (high - low + 1);
                        low = arr[i].low;
                        high = arr[i].high;
                }
        }

        total += (high - low + 1);

        return total;
}


static uint64_t
interval_count_unique(struct i_tree_node *root)
{
        struct interval intervals[BUFSIZ];
        size_t n = 0;

        collect_intervals(root, intervals, &n);

        return count_unique_ids(intervals, n);
}


static void
free_tree(struct i_tree_node *root)
{
        if (root == NULL)
                return;
        free_tree(root->left);
        free_tree(root->right);

        if (root->interval)
                free(root->interval);
        free(root);
}

static void
print_order (struct i_tree_node *root)
{
        if (root == NULL)
                return;

        print_order (root->left);

        printf("I:[ %015lu , %015lu ]\n", root->interval->low, root->interval->high);

        print_order (root->right);
}

static struct pattern *
parse_to_tree (const char* fp)
{
        struct i_tree_node *i_tree = NULL;

        FILE *file = fopen (fp, "r");
        if (!file)
        {
                return (NULL);
        }

        char* line = NULL;
        size_t   n = 0;

        while (getline (&line, &n, file) != -1)
        {
                if (line == NULL || line[0] == '\n' || line[0] == '\0')
                {
                        break;
                }
                size_t len = strlen(line);
                if (len > 0 && line[len - 1] == '\n')
                        line[len - 1] = '\0';

                struct interval ivl;
                uint64_t min = 0;
                uint64_t max = 0;
                size_t index = 0;
                for (; line[index] && line[index] != '-' ; index++)
                        if (isdigit(line[index]))
                                min = min * 10 + (line[index] - '0');
                index++;
                for (; line[index] && isdigit(line[index]) ; index++)
                        max = max * 10 + (line[index] - '0');
                ivl.low = min;
                ivl.high = max;
                i_tree = insert(i_tree, ivl);
        }
        uint64_t buf[1000];

        size_t bufindex = 0;
        while (getline(&line, &n, file) != -1)
        {
                size_t len = strlen(line);
                if (len > 0 && line[len - 1] == '\n')
                        line[len - 1] = '\0';

                uint64_t value = 0;
                size_t index = 0;
                for (; line[index] && isdigit(line[index]) ; index++)
                        value = value * 10 + (line[index] - '0');

                buf[bufindex] = value;
                bufindex = bufindex + 1;

        }

        uint64_t* ids = (uint64_t *) x_malloc(sizeof(uint64_t) * bufindex);
        for (size_t i = 0; i < bufindex; i++)
                ids[i] = buf[i];

        struct pattern * pattern = (struct pattern*) x_malloc(sizeof(struct pattern));
        pattern->root = i_tree;
        pattern->ids  = ids;
        pattern->ids_len = bufindex;


        free(line);
        fclose(file);
        return (pattern);
}

void
free_pattern(struct pattern *p)
{
        if (p == NULL)
                return;
        free(p->ids);
        free_tree(p->root);
        free(p);
}

void
day05 (const char* fp)
{
        struct pattern *pattern = parse_to_tree(fp);

        uint32_t part_01 = 0;
        for (size_t i = 0; i < pattern->ids_len; i++)
        {
                uint64_t n = pattern->ids[i];
                struct interval *ivl = search_id(pattern->root, n);
                if (ivl)
                {
                        part_01++;
                }
        }

        printf("Day 05 Part 01: %d\n", part_01);
        printf("Day 05 Part 02: %lu\n", interval_count_unique(pattern->root));
        free_pattern(pattern);
}
