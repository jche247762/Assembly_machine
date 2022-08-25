#ifndef ASSEMBLY
#define ASSEMBLY

#include <stdlib.h>
#define LEN 50
#define my_len 4

struct node
{
    u_int8_t* arr;
    int data_num;
    struct node* next;
};

struct node* node_new(int n);

int find_next_bit(FILE* file);

int find_opt(int n, FILE* file);

void check_type(FILE* file, struct node* node, int type, int n);

struct node* add_five_val(FILE* file, int opt);

struct node* add_three_val(FILE* file, int opt);

struct node* check_opt(int opt, FILE* file);

int get_idx(int* arr, int len, int target);

int check_arr_null(int* array);

void print_val_type(struct node* cur, int idx, int* stk_arr);

void print_line(struct node* head, int* stk_arr);

void add_to_end(struct node* head, struct node* new_node);

struct node* reverse_list(struct node* head);

void operate_line(FILE* file, struct node* head);

struct node* get_func(FILE* myfile);

void myfree(struct node* head);

#endif
