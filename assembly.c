#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LEN 50
#define my_len 4
#include "assembly.h"

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

int get_idx(int* arr, int len, int target)
{
    for(int i = 0; i < len; i++)
    {
        if (arr[i] == target)
        {
            return i;
        }
    }
    return -1;
}
int check_arr_null(int* array)
{
    for(int i = 0; i < my_len; i++)
    {
        if (array[i] != -1)
        {
            return -1;
        }
    }
    return 1;
}
void print_val_type(struct node* cur, int idx, int* stk_arr)
{
    static int count;
    if (cur->arr[idx] == 0b10) // stack
    {
        printf(" STK");
        char letter;
        int val = cur->arr[idx + 1];
        if (check_arr_null(stk_arr) == 1) { count = 0;}
        // printf("count is %d", count);
        if (get_idx(stk_arr, count, val) == -1)
        {
            stk_arr[count] = cur->arr[idx + 1];
            // printf("ret is %d   ", ret);
            letter = 'A' + count;
            count++;
            // printf("count is %d   ", count);
        }
        else
        {
            int idx = get_idx(stk_arr, count, val);
            letter = 'A' + idx;
            // printf("idx is %d   ", idx);
        }
        printf(" %c", letter);
    }
    if (cur->arr[idx] == 0b00) // value
    {
        printf(" VAL");
        printf(" %d", cur->arr[idx + 1]);
    }
    if (cur->arr[idx] == 0b01) // register
    {
        printf(" REG");
        printf(" %d", cur->arr[idx + 1]);
    }
    if (cur->arr[idx] == 0b11) // pointer
    {
        printf(" PTR");
        char letter;
        int val = cur->arr[idx + 1];
        int idx = get_idx(stk_arr, count, val);
        letter = 'A' + idx;
        printf(" %c", letter);
    }
}

void print_line(struct node* head, int* stk_arr)
{
    struct node* cur = head;
    while(cur!= NULL)
    {
        if (cur->arr[0] == 0b000) // MOV: 5
        {
            printf("    MOV");
            print_val_type(cur, 1, stk_arr);
            print_val_type(cur, 3, stk_arr);
            printf("\n");
        }
        if (cur->arr[0] == 0b100) // ADD: 5
        {
            printf("    ADD");
            print_val_type(cur, 1, stk_arr);
            print_val_type(cur, 3, stk_arr);
            printf("\n");
        }
        if (cur->arr[0] == 0b011) // REF: 5
        {
            printf("    REF");
            print_val_type(cur, 1, stk_arr);
            print_val_type(cur, 3, stk_arr);
            printf("\n");
        }
        if (cur->arr[0] == 0b001) // CAL: 3
        {
            printf("    CAL");
            print_val_type(cur, 1, stk_arr);
            printf("\n");
        }
        if (cur->arr[0] == 0b101) // PRINT
        {
            printf("    PRINT");
            print_val_type(cur, 1, stk_arr);
            printf("\n");
        }
        if (cur->arr[0] == 0b110) // NOT: 3
        {
            printf("    NOT");
            print_val_type(cur, 1, stk_arr);
            printf("\n");
        }

        if(cur->arr[0] == 0b111) // EQU: 3
        {
            printf("    EQU");
            print_val_type(cur, 1, stk_arr);
            printf("\n");
        }
        if(cur->arr[0] == 0b010) // RET
        {
            printf("    RET");
            printf("\n");
        }
        cur = cur->next;
    }
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

int main(int argc, char const* argv[])
{
    FILE* myfile = fopen(argv[1], "rb"); // read binary file from command line argument
    if (myfile == NULL)
    {
        perror("file does not exists\n");
        exit(0);
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
    for (int i = count - 1; i >= 0; i--)
    {
        // print_ll(arr[i]);
        int stk_arr[my_len] = {-1, -1, -1, -1};
        // printf("stk is %d", stk_arr[1]);
        struct node* head = func[i];
        printf("FUNC LABEL %d\n", func[i]->arr[0]);
        print_line(func[i]->next, stk_arr);
        myfree(func[i]);
    }
}
