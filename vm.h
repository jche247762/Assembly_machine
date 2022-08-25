#ifndef VM
#define VM

#include <stdlib.h>
#define LEN 50
#define my_len 4

struct node
{
    u_int8_t* arr;
    int data_num;
    struct node* next;
};

struct mynode
{
    int stack_ptr;
    int idx;
    int program_ctr;
    int func_num;

    u_int8_t stack[my_len];
    u_int8_t ptr[my_len];
    struct mynode* next;
};

struct mynode* new_mynode(int stack_ptr, int program_ctr, int idx, int func_num);

struct mynode* push_node(struct mynode** head, int stack_ptr, int program_ctr, int idx, int func_num);

int pop_node(struct mynode** head);

struct node* node_new(int n);

int find_next_bit(FILE* file);

int find_opt(int n, FILE* file);

void check_type(FILE* file, struct node* node, int type, int n);

struct node* add_five_val(FILE* file, int opt);

struct node* add_three_val(FILE* file, int opt);

struct node* check_opt(int opt, FILE* file);

void add_to_end(struct node* head, struct node* new_node);

struct node* reverse_list(struct node* head);

void operate_line(FILE* file, struct node* head);

struct node* get_func(FILE* myfile);

void myfree(struct node* head);

int get_func_idx(struct node** func, int n, int target);

int get_idx(int* arr, int target);

int stack_idx(int* array, int val, int index);

int stk_len(struct mynode* head);

int get_val_with_idx(struct mynode* head, int index);

void func_operate(struct node* func_node, struct mynode* head, u_int8_t* reg, struct node** func, int n);

#endif
