#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#define LEN 50
#define my_len 4

struct mynode* new_mynode(int stack_ptr, int program_ctr, int idx, int func_num)
{
    struct mynode* new = malloc(sizeof(struct mynode));
    new->program_ctr = program_ctr; // 0
    new->stack_ptr = stack_ptr; // 4
    new->func_num = func_num; // 7

    for (int i = 0; i < my_len; i++)
    {
        new->stack[i] = -1;
        new->ptr[i] = -1;
    }
    new->next = NULL;
    return new;
}

struct mynode* push_node(struct mynode** head, int stack_ptr, int program_ctr, int idx, int func_num)
{
    struct mynode* new = new_mynode(stack_ptr, program_ctr, idx, func_num);
    // printf("%d ", new->stack_frame);
    new->next = (*head);
    (*head) = new;
    // printf("%d ", head->stack_frame);
    return (*head);
}

int pop_node(struct mynode** head)
{
    if (head == NULL) return -1;
    int ret = (*head)->func_num;
    struct mynode* temp = (*head);
    (*head) = (*head)->next;
    return ret;
}

// int main(int argc, char const* argv[])
// {
//     struct mynode* head = new_mynode(4);
//     push_node(&head, 8);
//     push_node(&head, 23);
//     // printf("%d ", head->stack_frame);

//     int t = pop_node(head);
//     // printf("%d\n", t);
//     return 0;
// }

struct node* node_new(int n)
{
    struct node* new_node = malloc(sizeof(struct node));
    new_node->arr = malloc(sizeof(u_int8_t)*n);
    for (int i = 0; i < n; i++)
    {
        new_node->arr[i] = 0;
    }
    new_node->data_num = n;
    new_node->next = NULL;
    return new_node;
}

int find_next_bit(FILE* file)
{
    static int cursor = 8;
    static u_int8_t buf; // unsigned
    // static int end = 0;
    if (cursor == 8)
    {
        fread(&buf, 1, 1, file); // read b into 01000101
        // printf("b is %x\n", b);
        cursor = 0; // reset cursor
        fseek(file, -2, SEEK_CUR); // set cursor
    }
    int res = (buf >> cursor) & 1;
    // printf("%d ", r);
    // b = 01000101
    //     00000001
    cursor++;
    return res;
}

int find_opt(int n, FILE* file)
{
    int opt = 0;

    for (int j = 0; j < n; j++)
    {
        int b = find_next_bit(file);
        // printf("%x ", b);
        opt = opt | (b << j); // write into opt
        // printf("opt is %d\n", opt);

    }
    return opt;
}

void check_type(FILE* file, struct node* node, int type, int n)
{
    static int i = 1;
    int mv = 0;
    if (type == 0b01) // REG
    {
        if (i > 3) {i = 1; }
        node->arr[i] = type; // REG - 3 bits
        // print_ll(node);
        mv = 3;
        u_int8_t val = find_opt(mv, file);
        node->arr[i + 1] = val;
        if(n == 5) { i = i + 2;}
        if(n == 3) { i = 1;}
    }
    if (type == 0b00) // VAL - 8 bits
    {
        if (i > 3) {i = 1; }
        node->arr[i] = type;
        mv = 8;
        u_int8_t val = find_opt(mv, file);
        node->arr[i + 1] = val;
        if(n == 5) { i = i + 2;}
        if(n == 3) { i = 1;}
    }
    if (type == 0b10) // STK - 5 bits
    {
        if (i > 3) {i = 1; }
        node->arr[i] = type;
        // print_ll(node);
        mv = 5;
        u_int8_t val = find_opt(mv, file);
        node->arr[i + 1] = val;
        // print_ll(node);
        if(n == 5) { i = i + 2;}
        if(n == 3) { i = 1;}
    }
    if (type == 0b11) // PTR - 5 bits
    {
        if (i > 3) {i = 1; }
        node->arr[i] = type;
        mv = 5;
        u_int8_t val = find_opt(mv, file);
        node->arr[i + 1] = val;
        if(n == 5) { i = i + 2;}
        if(n == 3) { i = 1;}
    }
}

struct node* add_five_val(FILE* file, int opt) // node with 5 added value
{
    int n = 5;
    int mv = 2;
    struct node* node = node_new(n);
    node->arr[0] = opt;
    int type = find_opt(mv, file);
    check_type(file, node, type, n);

    int type2 = find_opt(mv, file);
    check_type(file, node, type2, n);
    return node;
}

struct node* add_three_val(FILE* file, int opt) // node with 3 added value
{
    int n = 3;
    int mv = 2;
    struct node* node = node_new(n);
    node->arr[0] = opt;

    int type = find_opt(mv, file);
    check_type(file, node, type, n);
    return node;
}

struct node* check_opt(int opt, FILE* file)
{
    struct node* new;
    if (opt == 0b000) // MOV
    {
        new = add_five_val(file, opt); // create node and add 5 value in arr
    }
    if (opt == 0b001) // CAL
    {
        new = add_three_val(file, opt);// create node and add 3 value in arr
    }
    if (opt == 0b011) // REF
    {
        new = add_five_val(file, opt);
    }
    if (opt == 0b100) // ADD
    {
        new = add_five_val(file, opt);
    }
    if (opt == 0b110) // NOT
    {
        new = add_three_val(file, opt);
    }
    if (opt == 0b111) // EQU
    {
        new = add_three_val(file, opt);
    }
    if (opt == 0b101) // PRINT
    {
        new = add_three_val(file, opt);
    }
    if (opt == 0b010) // RET
    {
        new = node_new(1);
        new->arr[0] = opt;
    }
    return new;
}

void add_to_end(struct node* head, struct node* new_node){
    struct node* cur = head;
    while(cur->next != NULL){
        cur=cur->next;
    }
    cur->next=new_node;
}

struct node* reverse_list(struct node* head) {
    struct node* cur = head;
    struct node* prev = NULL;
    while(cur != NULL) {
        struct node* temp = cur->next;
        cur->next = prev;
        prev = cur;
        cur = temp;
    }
    return prev;
}

void operate_line(FILE* file, struct node* head)
{
    int opt = find_opt(3, file);
    struct node* new = check_opt(opt, file);
    add_to_end(head, new);
}

struct node* get_func(FILE* myfile)
{
    int line_num = 0;
    for (int i = 0; i < 5; i++) // find the line number
    {
        int b = find_next_bit(myfile);
        // printf("%d ", b);
        line_num = line_num | (b << i);
    }
    // printf("line num is %d ", line_num);
    int opt = find_opt(3, myfile);
    struct node* head = check_opt(opt, myfile);

    for(int i = 0; i < line_num - 1; i++)
    {
        operate_line(myfile, head);
    }

    // check label
    int label = find_opt(3, myfile);
    struct node* label_node = node_new(1);
    label_node->arr[0] = label;
    add_to_end(head, label_node);
    struct node* new_head = reverse_list(head);
    return new_head;
}

void myfree(struct node* head)
{
    struct node* cur = head;
    while(cur != NULL)
    {
        struct node* temp = cur;
        free(temp->arr);
        free(temp);
        cur = cur->next;
    }
}

int get_func_idx(struct node** func, int n, int target)
{
    int idx = -1;
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (func[i]->arr[0] == target) // find main func with func label target in arr
        {
            idx = i;
            count++;
        }
    }
    if (count != 1) { return -1;} // no func or more than one func
    return idx;
}

int get_idx(int* arr, int target)
{
    for(int i = 0; i < my_len; i++)
    {
        if (arr[i] == target)
        {
            return i;
        }
    }
    return -1;
}

int stack_idx(int* array, int val, int index)
{
    if(get_idx(array, val) == -1) // not exists
    {
        index++;
        array[index] = val;
    }
    else
    {
        index = get_idx(array, val);
    }
    return index;
}

int stk_len(struct mynode* head)
{
    struct mynode* cur = head;
    int length = 0;
    while (cur != NULL)
    {
        cur = cur->next;
        length++;
    }
    return length;
}

int get_val_with_idx(struct mynode* head, int index)
{
    struct mynode* cur = head;
    int num = index / 4;
    int length = stk_len(cur);

    for (int i = 0; i < length - 1 - num; i++)
    {
        cur = cur->next;
    }
    int val = cur->stack[index % 4];
    return val;
}


void func_operate(struct node* func_node, struct mynode* head, u_int8_t* reg, struct node** func, int n)
{
    struct node* cur = func_node; // node without label
    int stk_arr[my_len] = {-1, -1, -1, -1};

    static int program_ctr = 0;
    static int stack_ptr = 0;
    static int index = -1;

    while (cur != NULL)
    {
        program_ctr++; // 1
        if (cur->arr[0] == 0b000) // MOV
        {
            if (cur->arr[1] == 0b10 && cur->arr[3] == 0b00) // MOV.. STACK .. VAL ..
            {
                int ret = stack_idx(stk_arr, cur->arr[2], index);
                index = ret;
                head->stack[ret] = cur->arr[4];
            }
            if (cur->arr[1] == 0b10 && cur->arr[3] == 0b01) // MOV.. STACK.. REG..
            {
                int ret = stack_idx(stk_arr, cur->arr[2], index);
                index = ret;
                head->stack[ret] = reg[cur->arr[4]];
            }
            if (cur->arr[1] == 0b01 && cur->arr[3] == 0b00) // MOV..REG .. VAL ..
            {
                reg[cur->arr[2]] = cur->arr[4];
            }
            if (cur->arr[1] == 0b10 && cur->arr[3] == 0b10) // MOV..STK .. STK ..
            {
                int ret = stack_idx(stk_arr, cur->arr[2], index);
                int ret1 = stack_idx(stk_arr, cur->arr[4], index);
                head->stack[ret] = head->stack[ret1];
            }
            if (cur->arr[1] == 0b11 && cur->arr[3] == 0b01) // MOV..PTR .. REG ..
            {
                int ret = stack_idx(stk_arr, cur->arr[2], index);
                index = ret;
                int address = head->stack[ret];
                head->ptr[ret] = get_val_with_idx(head, address);
            }
            if (cur->arr[1] == 0b01 && cur->arr[3] == 0b11) // MOV..REG .. PTR
            {

                int ret = stack_idx(stk_arr, cur->arr[2], index);
                index = ret;
                int address = head->stack[ret];
                reg[cur->arr[2]] = get_val_with_idx(head, address);
            }
        }
        if (cur->arr[0] == 0b001) // CAL
        {
            int num = get_func_idx(func, n, cur->arr[2]);
            // printf("num is %d\n", num);
            if (num == -1) { exit(1); } // error checking: none or more than one main fuc
            struct node* new_node = func[num]->next; // func 0
            // print_ll(new_node);
            stack_ptr += my_len;
            // printf("stack_ptr is %d\n", stack_ptr);
            index = 0;
            head->program_ctr = program_ctr;
            // printf("head pc is %d\n", head->program_ctr);
            program_ctr = 0;
            push_node(&head, stack_ptr, program_ctr, index, cur->arr[2]);

            func_operate(new_node, head, reg, func, n);

            int label = pop_node(&head);
            if (head == NULL) { exit(0);}

            func_operate(new_node, head, reg, func, n);
        }
        if (cur->arr[0] == 0b011) // REF
        {
            if (cur->arr[1] == 0b01 && cur->arr[3] == 0b10) // REF REG.. STK..
            {
                int ret = stack_idx(stk_arr, cur->arr[4], index);
                index = ret;
                // printf("ref ret is %d\n", ret);
                reg[cur->arr[2]] = ret + my_len;
            }
        }
        if (cur->arr[0] == 0b100) // ADD
        {
            reg[cur->arr[2]] = reg[cur->arr[2]] + reg[cur->arr[4]];
        }
        if (cur->arr[0] == 0b101) // PRINT
        {
            if (cur->arr[1] == 0b10) // STK
            {
                int ret = stack_idx(stk_arr, cur->arr[2], index);
                index = ret;
                int val = head->stack[ret];
                printf("%d\n", val);
            }
            if (cur->arr[1] == 0b01) // REG
            {
                printf("%d\n", reg[cur->arr[2]]);
            }
            if (cur->arr[1] == 0b11) // pointer
            {
                // int ret = stack_idx(stk_arr, cur->arr[2], index);
                int val = 4;
                // int index = ret;
                printf("%d\n", val);
            }
            if (cur->arr[1] == 0b00) // value
            {
                printf("%d\n", cur->arr[2]);
            }
        }
        if (cur->arr[0] == 0b110) // NOT
        {
            reg[cur->arr[2]] = ~reg[cur->arr[2]];
        }
        if (cur->arr[0] == 0b111) // EQU
        {
            if (cur->arr[2] == 0)// EQU..REG
            {
                reg[cur->arr[1]] = 1;
            }
            reg[cur->arr[1]] = 0;
        }
        cur = cur->next;
    }
}

int main(int argc, char const* argv[])
{
    if (argc != 2)
    {
        return 1;
    }
    FILE* myfile = fopen(argv[1], "rb"); // read binary file from command line argument
    if (myfile == NULL)
    {
        return 1;
    }
    // 0 3 2 1 42 82 86 4 10 45 ff
    fseek(myfile, -1, SEEK_END);

    struct node* func[8]; // 2 ^ 8 = 256, 8 functions at most
    // int a = ftell(myfile);
    // printf("a is %d", a);
    int count = 0;
    while(ftell(myfile) > 1)
    {
        func[count] = get_func(myfile);
        count++; // 0 1 2
    }

    u_int8_t reg[8]; // 2 ^ 3 registers, 1 byte each

    // get main func
    int idx = get_func_idx(func, count, 0);
    if (idx == -1) { exit(1); } // error checking: none or more than one main fuc
    struct node* cur = func[idx]->next; // func 0
    // print_ll(cur);
    struct mynode* head = new_mynode(0, 0, 0, 0); // push head with main func
    func_operate(cur, head, reg, func, count); // add value from func into head
}
