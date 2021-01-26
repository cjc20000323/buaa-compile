#include <cstdio>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <vector>
void deal_token(char c);
void getsym();
int find_return_func(char* name);
int find_no_return_func(char* name);
void print_mips(char* mips_code);
void store_array_mips(int loc);
void mips_generate();
void mips_print_statement1(int loc);
void mips_print_statement2(int loc);
void mips_print_statement3(int loc);
void mips_scanf_statement(int loc);
void store_var(char* name, char* reg, int type);
void mips_calculate(int loc);
void mips_calculate(int loc);
void mips_value_statement(int loc);
void mips_jump(int loc);
void mips_condition(int loc);
void mips_function_use(int loc);
void mips_return(int loc);
void mips_parameter(int loc);
void mips_push(int loc);
void mips_save_recursion(int prev_loc, int now_loc);
void mips_reload_recursion();
void mips_function(int loc);
struct return_info judge_string();
struct return_info judge_program();
struct return_info judge_const_inform();
struct return_info judge_const_define();
struct return_info judge_unsigned_integer();
struct return_info judge_integer();
struct return_info judge_head();
struct return_info judge_const();
struct return_info judge_var_inform();
struct return_info judge_var_define();
struct return_info judge_function_has_return();
struct return_info judge_function_no_return();
struct return_info judge_mix_statement();
struct return_info judge_parameter_list();
struct return_info judge_main_function();
struct return_info judge_expression();
struct return_info judge_item();
struct return_info judge_factor();
struct return_info judge_statement();
struct return_info judge_value_statement();
struct return_info judge_condition_statement();
struct return_info judge_condition();
struct return_info judge_circulation_statement();
struct return_info judge_stepsize();
struct return_info judge_case_statement();
struct return_info judge_case_list(char* exp, int count);
struct return_info judge_case_son_statement(char* exp, int count);
struct return_info judge_default(int count);
struct return_info judge_returnfunc_use();
struct return_info judge_noreturnfunc_use();
struct return_info judge_value_parameter_list();
struct return_info judge_statement_list();
struct return_info judge_scanf();
struct return_info judge_printf();
struct return_info judge_return_statement();

FILE* in = freopen("testfile.txt", "r", stdin);
FILE* out = freopen("mips.txt", "w", stdout);
FILE* middle = fopen("middle.txt", "w");
char c;
char s[1000];
char s_low[1000];

enum Code
{
    IDENFR,
    INTCON,
    CHARCON,
    STRCON,
    CONSTTK,
    INTTK,
    CHARTK,
    VOIDTK,
    MAINTK,
    IFTK,
    ELSETK,
    SWITCHTK,
    CASETK,
    DEFAULTTK,
    WHILETK,
    FORTK,
    SCANFTK,
    PRINTFTK,
    RETURNTK,
    PLUS,
    MINU,
    MULT,
    DIV,
    LSS,
    LEQ,
    GRE,
    GEQ,
    EQL,
    NEQ,
    COLON,
    ASSIGN,
    SEMICN,
    COMMA,
    LPARENT,
    RPARENT,
    LBRACK,
    RBRACK,
    LBRACE,
    RBRACE
};

struct return_func_info {
    char name[1000];
    int return_type; //0:int, 1:char, 2:void
};

Code symbol, pre1, pre2;
char content1[1000], content2[1000];
int flag1 = 0, flag2 = 0;
struct return_func_info no_return_func[1000];
struct return_func_info has_return_func[1000];
int no_return_func_sum = 0, has_return_func_sum = 0;

enum Middle_type {
    VARIABLE_DECLARATIONS,  //变量声明
    CONSTANT_DECLARATIONS, //常量声明
    SCANF_STATEMENT, //读语句
    VALUE_STATEMENT, //赋值语句
    PRINT_STATEMENT1, //printf '(' ＜字符串＞,＜表达式＞ ')' 
    PRINT_STATEMENT2,//printf '('＜字符串＞ ')'
    PRINT_STATEMENT3,//printf '('＜表达式＞ ')'
    CALCULATE, //涉及到计算结果的语句
    MAIN, //主函数调用
    JUMP, //跳转
    LABEL, //跳转的标记
    RETURN_STATEMENT, //返回语句
    FUNCTION_USE, //函数调用语句
    COMPARE, //比较语句
    HEADER, //声明头部
    PUSH, //推入一个参数，调用的时候使用
    PARAMETER, //说明这是声明时候的参数
};

void print_type(Middle_type type)
{
    if (type == VARIABLE_DECLARATIONS)
        fprintf(middle, "VARIABLE_DECLARATIONS ");
    else if (type == CONSTANT_DECLARATIONS)
        fprintf(middle, "CONSTANT_DECLARATIONS ");
    else if (type == SCANF_STATEMENT)
        fprintf(middle, "SCANF_STATEMENT ");
    else if (type == VALUE_STATEMENT)
        fprintf(middle, "VALUE_STATEMENT ");
    else if (type == PRINT_STATEMENT1)
        fprintf(middle, "PRINT_STATEMENT1 ");
    else if (type == PRINT_STATEMENT2)
        fprintf(middle, "PRINT_STATEMENT2 ");
    else if (type == PRINT_STATEMENT3)
        fprintf(middle, "PRINT_STATEMENT3 ");
    else if (type == CALCULATE)
        fprintf(middle, "CALCULATE ");
    else if (type == MAIN)
        fprintf(middle, "MAIN ");
    else if (type == JUMP)
        fprintf(middle, "JUMP ");
    else if (type == LABEL)
        fprintf(middle, "LABEL ");
    else if (type == RETURN_STATEMENT)
        fprintf(middle, "RETURN_STATEMENT ");
    else if (type == FUNCTION_USE)
        fprintf(middle, "FUNCTION_USE ");
    else if (type == COMPARE)
        fprintf(middle, "COMPARE ");
    else if (type == HEADER)
        fprintf(middle, "HEADER ");
    else if (type == PUSH)
        fprintf(middle, "PUSH ");
    else if (type == PARAMETER)
        fprintf(middle, "PARAMETER ");
}

int reg_use[35]; //寄存器是否被使用过，0代表没有，1代表有

int choose_reg_s() {
    int i;
    for (i = 16; i <= 23; i++) {
        if (reg_use[i] == 0) {
            return i;
        }
    }
    return -1;
}

int choose_reg_t() {
    int i;
    for (i = 8; i <= 15; i++) {
        if (reg_use[i] == 0) {
            return i;
        }
    } //t8和t9先保留着不用
    return -1;
}

struct Middle_code {
    Middle_type type; //这一组中间代码是哪一种语句的
    int num; //code的数量
    int kind; //0:int, 1:char
    char code[10][100]; //中间代码的内容
    int level; //数组的维数
    int i_len; //行数
    int j_len; //列数  
    int map1[50000]; //存储整数数组
    char map2[50000]; //存储字符数组
};

struct name_info {
    char name[100]; //名字
    int type; //0:int, 1:char
    int use;
    int new_type; //0:int, 1:char
    char new_name[100]; //新的名字
};

struct return_info {
    int return_num;
    int num;
    int type;//0:int, 1:char
    char code[10][100];
};

struct return_info zero = { 0 };
struct return_info fail = { -1 };
struct return_info one = { 1 };

struct name_info name_list[1000];  //所有名字都保存在这里，包括变量，常量，函数名
int name_sum = 0; //现在一共声明了多少个名字

struct Middle_code middle_code_list[1000]; //中间代码的列表
int middle_code_sum = 0; //中间代码列表的指针
int main_location;
int label_sum = 0; //现在一共声明了多少个label
int case_label_sum = 0; //情况语句专用的label计数
int default_label_sum = 0; //缺省语句专用的label计数
int case_label[100]; //第几组情况语句

int int_map[2000][2000]; //存储int型的变量数组
char char_map[2000][2000]; //存储char型的变量数组

int reg_index = 0;

int find_name(char* name) {
    int i;
    for (i = 0; i < name_sum; i++) {
        if (strcmp(name, name_list[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

Middle_code middle_code_generate(Middle_type type, int kind, int num, ...) {
    va_list ap;
    Middle_code middle_code;
    middle_code.type = type;
    middle_code.num = num;
    middle_code.kind = kind;
    middle_code.i_len = 0;
    middle_code.j_len = 0;
    middle_code.level = 0;
    va_start(ap, num);
    int i;
    for (i = 0; i < num; i++) {
        strcpy(middle_code.code[i], va_arg(ap, char*));
    }
    va_end(ap);
    return middle_code;
}

Middle_code middle_code_generate_array1(Middle_type type, int i_len, int j_len, int level, int kind, int num, ...)
{
    va_list ap;
    Middle_code middle_code;
    middle_code.type = type;
    middle_code.num = num;
    middle_code.kind = kind;
    middle_code.i_len = i_len;
    middle_code.j_len = j_len;
    middle_code.level = level;
    int i, j, count = 0;
    if (kind == 0 && num == 4)
    {
        for (i = 0; i < i_len; i++)
        {
            for (j = 0; j < j_len; j++)
            {
                middle_code.map1[count++] = int_map[i][j];
            }
        }
    }
    else if (kind == 1 && num == 4)
    {
        for (i = 0; i < i_len; i++)
        {
            for (j = 0; j < j_len; j++)
            {
                middle_code.map2[count++] = char_map[i][j];
            }
        }
    }
    va_start(ap, num);
    for (i = 0; i < num; i++)
    {
        strcpy(middle_code.code[i], va_arg(ap, char*));
    }
    va_end(ap);
    return middle_code;
}

int find_const(char* name) {
    int i;
    for (i = 0; i < middle_code_sum; i++) {
        if (middle_code_list[i].type == CONSTANT_DECLARATIONS &&
            strcmp(middle_code_list[i].code[2], name) == 0) {
            return i;
        }
    }
    return -1;
}

int mips_string_sum = 0;
int depth = 0;
int stack_pointer = 0x7fffeffc; //???
int global_pointer = 0x7ff00000;
int base_pointer = 0x7ff00000;
int dfs_array = 0; //数组处理函数调用的层数

struct store_info {
    char name[100];
    int type;
    int reg_num;
    int stack_position;
    int init;
    int sum;
    int isarray;
    int i_len;
    int j_len;
};

struct roll_back_info {
    struct store_info store_info_area[100];
    int roll_back; //当前一段的回滚长度
};

struct store_info store_info_list[1000]; //我们可以判断，经过中间代码的处理，肯定不会有重名的情况，因此只需要考虑函数调用的情况
struct roll_back_info back_store_info_list[1000];
int store_info_sum = 0;
int back_store_info_sum = 0;

char condition_exp1[100], condition_op[5], condition_exp2[100]; //分别表示跳转的条件判断中的表达式和符号

int find_var(char* name) {
    int i;
    for (i = 0; i < store_info_sum; i++) {
        if (strcmp(store_info_list[i].name, name) == 0 &&
            store_info_list[i].reg_num != -2) {
            return i;
        }
    }
    return -1;
}

void print_mips(char* mips_code) {
    int i;
    for (i = 0; i < depth; i++) {
        printf("      ");
    }
    printf("%s\n", mips_code);
}

void store_string_mips() {
    int i;
    char code[100];
    for (i = 0; i < middle_code_sum; i++) {
        if (middle_code_list[i].type == PRINT_STATEMENT1
            || middle_code_list[i].type == PRINT_STATEMENT2) {
            sprintf(code, "str%d: .asciiz \"%s\"", mips_string_sum, middle_code_list[i].code[1]);
            sprintf(middle_code_list[i].code[1], "str%d", mips_string_sum++);
            print_mips(code);
        }
    }
    sprintf(code, "changeline: .asciiz \"\\n\"");
    print_mips(code);
}

void store_array_mips(int loc)
{
    char code[100];
    strcpy(store_info_list[store_info_sum].name, middle_code_list[loc].code[2]);
    store_info_list[store_info_sum].reg_num = -1;
    store_info_list[store_info_sum].type = middle_code_list[loc].kind;
    store_info_list[store_info_sum].isarray = 1;
    store_info_list[store_info_sum].i_len = middle_code_list[loc].i_len;
    store_info_list[store_info_sum].j_len = middle_code_list[loc].j_len;
    if (middle_code_list[loc].num == 3)
    {
        store_info_list[store_info_sum].init = 0;
        if (middle_code_list[loc].level == 1)
        {
            global_pointer -= 4 * middle_code_list[loc].j_len;
            store_info_list[store_info_sum].sum = middle_code_list[loc].j_len;
            store_info_list[store_info_sum++].stack_position = global_pointer;
        }
        else if (middle_code_list[loc].level == 2)
        {
            global_pointer -= 4 * middle_code_list[loc].i_len * middle_code_list[loc].j_len;
            store_info_list[store_info_sum].sum = middle_code_list[loc].i_len * middle_code_list[loc].j_len;
            store_info_list[store_info_sum++].stack_position = global_pointer;
        }
    }
    else if (middle_code_list[loc].num == 4)
    {
        store_info_list[store_info_sum].init = 1;
        if (middle_code_list[loc].level == 1)
        {
            global_pointer -= 4 * middle_code_list[loc].j_len;
            store_info_list[store_info_sum].sum = middle_code_list[loc].j_len;
            store_info_list[store_info_sum++].stack_position = global_pointer;
            sprintf(code, "li $t9, 0x%x", global_pointer);
            print_mips(code);
            for (int i = 0; i < middle_code_list[loc].j_len; i++)
            {
                if (middle_code_list[loc].kind == 0)
                    sprintf(code, "li $t8, 0x%x", middle_code_list[loc].map1[i]);
                else
                    sprintf(code, "li $t8, 0x%x", middle_code_list[loc].map2[i]);
                print_mips(code);
                sprintf(code, "sw $t8, 0($t9)");
                print_mips(code);
                sprintf(code, "addi $t9, $t9, 4");
                print_mips(code);
            }
        }
        else if (middle_code_list[loc].level == 2)
        {
            global_pointer -= 4 * middle_code_list[loc].i_len * middle_code_list[loc].j_len;
            store_info_list[store_info_sum].sum = middle_code_list[loc].i_len * middle_code_list[loc].j_len;
            store_info_list[store_info_sum++].stack_position = global_pointer;
            sprintf(code, "li $t9, 0x%x", global_pointer);
            print_mips(code);
            for (int i = 0; i < middle_code_list[loc].i_len * middle_code_list[loc].j_len; i++)
            {
                    if (middle_code_list[loc].kind == 0)
                        sprintf(code, "li $t8, 0x%x", middle_code_list[loc].map1[i]);
                    else
                        sprintf(code, "li $t8, 0x%x", middle_code_list[loc].map2[i]);
                    print_mips(code);
                    sprintf(code, "sw $t8, 0($t9)");
                    print_mips(code);
                    sprintf(code, "addi $t9, $t9, 4");
                    print_mips(code);
            }
        }
    }
}

void store_var1() {
    int i, rs, rt;
    char code[100];
    for (i = 0; middle_code_list[i].type != MAIN && middle_code_list[i].type != HEADER; i++) { //注意，未来可能增加对于函数调用的处理
        if (middle_code_list[i].type == VARIABLE_DECLARATIONS && middle_code_list[i].level == 0) {
            rs = choose_reg_s();
            rt = choose_reg_t();
            store_info_list[store_info_sum].isarray = 0;
            store_info_list[store_info_sum].i_len = 0;
            store_info_list[store_info_sum].j_len = 0;
            if (rs > 0) {
                strcpy(store_info_list[store_info_sum].name, middle_code_list[i].code[2]);
                store_info_list[store_info_sum].type = middle_code_list[i].kind;
                store_info_list[store_info_sum++].reg_num = rs;
                if (middle_code_list[i].num == 5) {
                    if (middle_code_list[i].kind == 0) {
                        sprintf(code, "li $s%d, 0x%x", rs - 16, atoi(middle_code_list[i].code[4]));
                    }
                    else {
                        sprintf(code, "li $s%d, 0x%x", rs - 16, middle_code_list[i].code[4][1]);
                    }
                    print_mips(code);
                }
                reg_use[rs] = 1;
            }
            else if (rt > 0) {
                strcpy(store_info_list[store_info_sum].name, middle_code_list[i].code[2]);
                store_info_list[store_info_sum].type = middle_code_list[i].kind;
                store_info_list[store_info_sum++].reg_num = rt;
                if (middle_code_list[i].num == 5) {
                    if (middle_code_list[i].kind == 0) {
                        sprintf(code, "li $t%d, 0x%x", rt - 8, atoi(middle_code_list[i].code[4]));
                    }
                    else {
                        sprintf(code, "li $t%d, 0x%x", rt - 8, middle_code_list[i].code[4][1]);
                    }
                    print_mips(code);
                }
                reg_use[rt] = 1;
            }
            else {
                strcpy(store_info_list[store_info_sum].name, middle_code_list[i].code[2]);
                store_info_list[store_info_sum].type = middle_code_list[i].kind;
                store_info_list[store_info_sum].reg_num = -1;
                global_pointer -= 4;
                if (middle_code_list[i].num == 5) {
                    if (middle_code_list[i].kind == 0) {
                        sprintf(code, "li $t8, 0x%x", atoi(middle_code_list[i].code[4]));
                        print_mips(code);
                        sprintf(code, "sw $t8, %d($gp)", global_pointer - base_pointer);
                        print_mips(code);
                    }
                    else {
                        sprintf(code, "li $t8, 0x%x", middle_code_list[i].code[4][1]);
                        print_mips(code);
                        sprintf(code, "sw $t8, %d($gp)", global_pointer - base_pointer);
                        print_mips(code);
                    }
                }
                store_info_list[store_info_sum++].stack_position = global_pointer;
            }
        }
        else if(middle_code_list[i].type == VARIABLE_DECLARATIONS && middle_code_list[i].level > 0)
        {
            store_array_mips(i);
        }
    }
}

void mips_generate() {
    int i, rs, rt, pre;
    char code[100];
    sprintf(code, ".data");
    print_mips(code);
    depth++;
    store_string_mips();
    depth--;
    sprintf(code, ".text");
    print_mips(code);
    depth++;
    sprintf(code, "li $gp, 0x7ff00000");
    print_mips(code);
    store_var1(); //注意后续的数组改动
    sprintf(code, "jal main");
    print_mips(code);
    sprintf(code, "li $v0, 10");
    print_mips(code);
    sprintf(code, "syscall");
    print_mips(code);
    sprintf(code, "main:");
    print_mips(code);
    depth++;
    sprintf(code, "addi $sp, $sp, -4");
    print_mips(code);
    sprintf(code, "sw $ra, 0($sp)");
    print_mips(code);
    pre = store_info_sum;
    for (i = main_location + 1; i < middle_code_sum; i++) {
        if (middle_code_list[i].type == SCANF_STATEMENT) {
            mips_scanf_statement(i);
        }
        else if (middle_code_list[i].type == PRINT_STATEMENT1) {
            mips_print_statement1(i);
            sprintf(code, "li $v0, 4");
            print_mips(code);
            sprintf(code, "la $a0, changeline");
            print_mips(code);
            sprintf(code, "syscall");
            print_mips(code);
        }
        else if (middle_code_list[i].type == PRINT_STATEMENT2) {
            mips_print_statement2(i);
            sprintf(code, "li $v0, 4");
            print_mips(code);
            sprintf(code, "la $a0, changeline");
            print_mips(code);
            sprintf(code, "syscall");
            print_mips(code);
        }
        else if (middle_code_list[i].type == PRINT_STATEMENT3) {
            mips_print_statement3(i);
            sprintf(code, "li $v0, 4");
            print_mips(code);
            sprintf(code, "la $a0, changeline");
            print_mips(code);
            sprintf(code, "syscall");
            print_mips(code);
        }
        else if (middle_code_list[i].type == CALCULATE) {
            mips_calculate(i);
        }
        else if (middle_code_list[i].type == VALUE_STATEMENT) {
            mips_value_statement(i);
        }
        else if (middle_code_list[i].type == VARIABLE_DECLARATIONS && middle_code_list[i].level == 0) {
            rs = choose_reg_s();
            rt = choose_reg_t();
            if (rs > 0) {
                strcpy(store_info_list[store_info_sum].name, middle_code_list[i].code[2]);
                store_info_list[store_info_sum].type = middle_code_list[i].kind;
                store_info_list[store_info_sum++].reg_num = rs;
                if (middle_code_list[i].num == 5) {
                    if (middle_code_list[i].kind == 0) {
                        sprintf(code, "li $s%d, 0x%x", rs - 16, atoi(middle_code_list[i].code[4]));
                    }
                    else {
                        sprintf(code, "li $s%d, 0x%x", rs - 16, middle_code_list[i].code[4][1]);
                    }
                    print_mips(code);
                }
                reg_use[rs] = 1;
            }
            else if (rt > 0) {
                strcpy(store_info_list[store_info_sum].name, middle_code_list[i].code[2]);
                store_info_list[store_info_sum].type = middle_code_list[i].kind;
                store_info_list[store_info_sum++].reg_num = rt;
                if (middle_code_list[i].num == 5) {
                    if (middle_code_list[i].kind == 0) {
                        sprintf(code, "li $t%d, 0x%x", rt - 8, atoi(middle_code_list[i].code[4]));
                    }
                    else {
                        sprintf(code, "li $t%d, 0x%x", rt - 8, middle_code_list[i].code[4][1]);
                    }
                    print_mips(code);
                }
                reg_use[rt] = 1;
            }
            else {
                strcpy(store_info_list[store_info_sum].name, middle_code_list[i].code[2]);
                store_info_list[store_info_sum].type = middle_code_list[i].kind;
                store_info_list[store_info_sum].reg_num = -1;
                global_pointer -= 4;
                if (middle_code_list[i].num == 5) {
                    if (middle_code_list[i].kind == 0) {
                        sprintf(code, "li $t8, 0x%x", atoi(middle_code_list[i].code[4]));
                        print_mips(code);
                        sprintf(code, "sw $t8, %d($gp)", global_pointer - base_pointer);
                        print_mips(code);
                    }
                    else {
                        sprintf(code, "li $t8, 0x%x", middle_code_list[i].code[4][1]);
                        print_mips(code);
                        sprintf(code, "sw $t8, %d($gp)", global_pointer - base_pointer);
                        print_mips(code);
                    }
                }
                store_info_list[store_info_sum++].stack_position = global_pointer;
            }
        }
        else if (middle_code_list[i].type == VARIABLE_DECLARATIONS && middle_code_list[i].level > 0)
        {
            store_array_mips(i);
        }
        else if (middle_code_list[i].type == COMPARE)
        {
            mips_condition(i);
        }
        else if (middle_code_list[i].type == JUMP)
        {
            mips_jump(i);
        }
        else if (middle_code_list[i].type == LABEL)
        {
            print_mips(middle_code_list[i].code[0]);
        }
        else if (middle_code_list[i].type == FUNCTION_USE)
        {
            /*sprintf(code, "addi $sp, $sp, -4");
            print_mips(code);
            sprintf(code, "sw $ra, 0($sp)");
            print_mips(code);*/
            mips_save_recursion(pre, store_info_sum);
            mips_push(i);
            mips_function_use(i);
            mips_reload_recursion();
            /*sprintf(code, "lw $ra, 0($sp)");
            print_mips(code);
            sprintf(code, "addi $sp, $sp, 4");
            print_mips(code);*/
        }
        else if (middle_code_list[i].type == RETURN_STATEMENT)
        {
            mips_return(i);
        }
    }
    sprintf(code, "li $v0, 10");
    print_mips(code);
    sprintf(code, "syscall");
    print_mips(code);
    depth--;
    for (i = 0; i < main_location; i++)
    {
        if (middle_code_list[i].type == HEADER)
        {
            depth++;
            mips_function(i);
            depth--;
            i++;
            while (middle_code_list[i].type != HEADER && middle_code_list[i].type != MAIN)
            {
                i++;
            }
            i--;
        }
        else if (middle_code_list[i].type == LABEL)
        {
            print_mips(middle_code_list[i].code[0]);
        }
    }
}

int mips_deal_array(char* name, char* cal, char* reg, int mode) //mode=0需要返回数组元素的值，mode=1为需要返回数组元素的地址
{
    //注意，一定不能让reg是v1，因为需要用它来从栈中取出数据
    int i, ret = -1, j, count = 0, position; //level表示维数，count表示数组嵌套的层数
    char code[100], exp[100], exp1[100], exp2[100];
    char name_fuben[100];
    strcpy(name_fuben, name);
    if (name_fuben[strlen(name_fuben) - 1] == '?')
    {
        for (i = 0; i < strlen(name_fuben); i++)
        {
            if (name_fuben[i] == '[')
            {
                ret = 0;
                strcpy(exp, &name_fuben[i + 1]);
                name_fuben[i] = '\0';
                for (j = strlen(exp) - 1; j >= 0; j--)
                {
                    if (exp[j] == ']')
                    {
                        exp[j] = '\0';
                        dfs_array++;
                        mips_deal_array(exp, cal, reg, 0); //我个人理解应该都是0才对，之后再进行判断
                        dfs_array--;
                        sprintf(code, "sll %s, %s, 2", reg, cal);
                        print_mips(code);
                        position = find_var(name_fuben);
                        position = store_info_list[position].reg_num >= 0 ? 
                            store_info_list[position].reg_num : store_info_list[position].stack_position;
                        sprintf(code, "addi %s, %s, 0x%x", reg, reg, position);
                        print_mips(code);
                        if (mode == 1)
                        {
                            sprintf(code, "move $v1, %s", reg);
                            print_mips(code);
                        }
                        else if (mode == 0 || dfs_array > 0)
                        {
                            sprintf(code, "lw %s, 0(%s)", reg, reg);
                            print_mips(code);
                            strcpy(cal, reg);
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    else if (name_fuben[strlen(name_fuben) - 1] == '#')
    {
        for (i = 0; i < strlen(name_fuben); i++)
        {
            if (name_fuben[i] == '[')
            {
                ret = 0;
                count++;
                strcpy(exp, &name_fuben[i + 1]);
                name_fuben[i] = '\0';
                for (j = 0; j < strlen(exp); j++)
                {
                    if (exp[j] == '[')
                        count++;
                    else if (exp[j] == ']')
                        count--;
                    if (count == 0)
                    {
                        strcpy(exp2, &exp[j + 1]);
                        exp[j] = '\0';
                        strcpy(exp1, exp);
                        dfs_array++;
                        mips_deal_array(exp1, cal, reg, 0);
                        dfs_array--;
                        sprintf(code, "addi $sp, $sp, -4");
                        print_mips(code);
                        sprintf(code, "sw %s, 0($sp)", cal);
                        print_mips(code);
                        break;
                    }
                }
                count++;
                for (j = strlen(exp2) - 3; j >= 0; j--) //注意是减3，因为最后一个是标记，倒数第二个是]
                {
                    if (exp2[j] == ']')
                        count++;
                    else if (exp2[j] == '[')
                        count--;
                    if (count == 0)
                    {
                        exp2[strlen(exp2) - 2] = '\0';
                        strcpy(exp1, &exp2[j + 1]);
                        dfs_array++;
                        mips_deal_array(exp1, cal, reg, 0);
                        dfs_array--;
                        break;
                    }
                }
                position = find_var(name_fuben);
                sprintf(code, "lw $v1, 0($sp)");
                print_mips(code);
                sprintf(code, "addi $sp, $sp, 4");
                print_mips(code);
                sprintf(code, "li $v0, 0x%x", store_info_list[position].j_len);
                print_mips(code);
                sprintf(code, "mul $v1, $v1, $v0");
                print_mips(code);
                sprintf(code, "add %s, $v1, %s", reg, cal);
                print_mips(code);
                sprintf(code, "sll %s, %s, 2", reg, reg);
                print_mips(code);
                position = find_var(name_fuben);
                position = store_info_list[position].reg_num >= 0 ?
                    store_info_list[position].reg_num : store_info_list[position].stack_position;
                sprintf(code, "addi %s, %s, 0x%x", reg, reg, position);
                print_mips(code);
                if (mode == 1) //注意，该种情况有可能有问题，要注意是否改动
                {
                    sprintf(code, "move $v1, %s", reg);
                    print_mips(code);
                }
                else if (mode == 0 || dfs_array > 0)
                {
                    sprintf(code, "lw %s, 0(%s)", reg, reg);
                    print_mips(code);
                    strcpy(cal, reg);
                } 
            }
        }
    }
    if (ret == -1 && dfs_array > 0)
    {
        if (name_fuben[0] == '+' || name_fuben[0] == '-' || isdigit(name_fuben[0]))
        {
            sprintf(code, "li %s, 0x%x", reg, atoi(name_fuben));
            print_mips(code);
            strcpy(cal, reg);
        }
        else if ((position = find_const(name_fuben)) >= 0)
        {
            sprintf(code, "li %s, 0x%x", reg, atoi(middle_code_list[position].code[4]));
            print_mips(code);
            strcpy(cal, reg);
        }
        else {
            position = find_var(name_fuben);
            position = store_info_list[position].reg_num >= 0 ? 
                store_info_list[position].reg_num : store_info_list[position].stack_position;
            if (position < 31)
            {
                if (position >= 16 && position <= 23) {
                    sprintf(cal, "$s%d", position - 16);//注意这里会不会改动
                }
                else {
                    sprintf(cal, "$t%d", position - 8); //注意这里会不会改动
                }
            }
            else
            {
                sprintf(code, "lw %s, %d($gp)", reg, position - base_pointer);
                print_mips(code);
                strcpy(cal, reg);
            }
        }
    }
    return ret;
}

void mips_print_statement1(int loc) {
    char code[100], cal[100], reg[100];
    int pos;
    mips_print_statement2(loc);
    if (middle_code_list[loc].code[2][0] == '\'') {
        if (middle_code_list[loc].kind == 1)
        {
            sprintf(code, "li $v0, 11");
            print_mips(code);
        }
        else
        {
            sprintf(code, "li $v0, 1");
            print_mips(code);
        }
        sprintf(code, "li $a0, %d", middle_code_list[loc].code[2][1]);
        print_mips(code);
        sprintf(code, "syscall");
        print_mips(code);
    }
    else if (middle_code_list[loc].code[2][0] == '+' ||
        middle_code_list[loc].code[2][0] == '-' ||
        isdigit(middle_code_list[loc].code[2][0])) {
        sprintf(code, "li $v0, 1");
        print_mips(code);
        sprintf(code, "li $a0, 0x%x", atoi(middle_code_list[loc].code[2]));
        print_mips(code);
        sprintf(code, "syscall");
        print_mips(code);
    }
    else if ((pos = find_const(middle_code_list[loc].code[2])) >= 0) {
        if (strcmp(middle_code_list[pos].code[1], "int") == 0) {
            sprintf(code, "li $v0, 1");
            print_mips(code);
            sprintf(code, "li $a0, 0x%x", atoi(middle_code_list[pos].code[4]));
            print_mips(code);
            sprintf(code, "syscall");
            print_mips(code);
        }
        else {
            sprintf(code, "li $v0, 11");
            print_mips(code);
            sprintf(code, "li $a0, 0x%x", middle_code_list[pos].code[4][0]);
            print_mips(code);
            sprintf(code, "syscall");
            print_mips(code);
        }
    }
    else {
        sprintf(reg, "$a0");
        if (mips_deal_array(middle_code_list[loc].code[2], cal, reg, 0) == 0)
        {
            if (middle_code_list[loc].kind == 0)
                sprintf(code, "li $v0, 1");
            else
                sprintf(code, "li $v0, 11");
            print_mips(code);
            sprintf(code, "syscall");
            print_mips(code);
        }
        else
        {
            pos = find_var(middle_code_list[loc].code[2]);
            if (store_info_list[pos].type == 0) {
                sprintf(code, "li $v0, 1");
                print_mips(code);
            }
            else {
                sprintf(code, "li $v0, 11");
                print_mips(code);
            }
            pos = store_info_list[pos].reg_num >= 0 ?
                store_info_list[pos].reg_num : store_info_list[pos].stack_position;
            if (pos <= 31) {
                if (pos >= 16 && pos <= 25) {
                    sprintf(code, "move $a0, $s%d", pos - 16);
                }
                else {
                    sprintf(code, "move $a0, $t%d", pos - 8);
                }
                print_mips(code);
                sprintf(code, "syscall");
                print_mips(code);
            }
            else {
                sprintf(code, "lw $a0, %d($gp)", pos - base_pointer);
                print_mips(code);
                sprintf(code, "syscall");
                print_mips(code);
            }
        }
    }
}

void mips_print_statement2(int loc) {
    char code[100];
    sprintf(code, "li $v0, 4");
    print_mips(code);
    sprintf(code, "la $a0, %s", middle_code_list[loc].code[1]);
    print_mips(code);
    sprintf(code, "syscall");
    print_mips(code);
}

void mips_print_statement3(int loc) {
    char code[100], cal[100], reg[100];
    int pos;
    if (middle_code_list[loc].code[1][0] == '\'') {
        if (middle_code_list[loc].kind == 1)
        {
            sprintf(code, "li $v0, 11");
            print_mips(code);
        }
        else
        {
            sprintf(code, "li $v0, 1");
            print_mips(code);
        }
        sprintf(code, "li $a0, %d", middle_code_list[loc].code[1][1]);
        print_mips(code);
        sprintf(code, "syscall");
        print_mips(code);
    }
    else if (middle_code_list[loc].code[1][0] == '+' ||
        middle_code_list[loc].code[1][0] == '-' ||
        isdigit(middle_code_list[loc].code[1][0])) {
        sprintf(code, "li $v0, 1");
        print_mips(code);
        sprintf(code, "li $a0, 0x%x", atoi(middle_code_list[loc].code[1]));
        print_mips(code);
        sprintf(code, "syscall");
        print_mips(code);
    }
    else if ((pos = find_const(middle_code_list[loc].code[1])) >= 0) {
        if (strcmp(middle_code_list[pos].code[1], "int") == 0) {
            sprintf(code, "li $v0, 1");
            print_mips(code);
            sprintf(code, "li $a0, 0x%x", atoi(middle_code_list[pos].code[4]));
            print_mips(code);
            sprintf(code, "syscall");
            print_mips(code);
        }
        else {
            sprintf(code, "li $v0, 11");
            print_mips(code);
            sprintf(code, "li $a0, 0x%x", middle_code_list[pos].code[4][0]);
            print_mips(code);
            sprintf(code, "syscall");
            print_mips(code);
        }
    }
    else {
        sprintf(reg, "$a0");
        if (mips_deal_array(middle_code_list[loc].code[1], cal, reg, 0) == 0)
        {
            if (middle_code_list[loc].kind == 0)
                sprintf(code, "li $v0, 1");
            else
                sprintf(code, "li $v0, 11");
            print_mips(code);
            sprintf(code, "syscall");
            print_mips(code);
        }
        else
        {
            pos = find_var(middle_code_list[loc].code[1]);
            if (store_info_list[pos].type == 0) {
                sprintf(code, "li $v0, 1");
                print_mips(code);
            }
            else {
                sprintf(code, "li $v0, 11");
                print_mips(code);
            }
            pos = store_info_list[pos].reg_num >= 0 ?
                store_info_list[pos].reg_num : store_info_list[pos].stack_position;
            if (pos <= 31) {
                if (pos >= 16 && pos <= 25) {
                    sprintf(code, "move $a0, $s%d", pos - 16);
                }
                else {
                    sprintf(code, "move $a0, $t%d", pos - 8);
                }
                print_mips(code);
                sprintf(code, "syscall");
                print_mips(code);
            }
            else {
                sprintf(code, "lw $a0, %d($gp)", pos - base_pointer);
                print_mips(code);
                sprintf(code, "syscall");
                print_mips(code);
            }
        }
    }
}

void mips_scanf_statement(int loc) {
    int pos;
    char code[100];
    char reg[100];
    if (middle_code_list[loc].kind == 0) {
        sprintf(code, "li $v0, 5");
        print_mips(code);
        sprintf(code, "syscall");
        print_mips(code);
    }
    else if (middle_code_list[loc].kind == 1) { //注意是否需要增加空格
        sprintf(code, "li $v0, 12");
        print_mips(code);
        sprintf(code, "syscall");
        print_mips(code);
    }
    pos = find_var(middle_code_list[loc].code[1]);
    if (pos >= 0) {
        pos = store_info_list[pos].reg_num >= 0 ?
            store_info_list[pos].reg_num : store_info_list[pos].stack_position;
    }
    if (pos >= 0 && pos <= 31) {
        if (pos >= 16 && pos <= 25)
            sprintf(code, "move $s%d, $v0", pos - 16);
        else
            sprintf(code, "move $t%d, $v0", pos - 8);
        print_mips(code);
    }
    else if (pos > 31) {
        sprintf(code, "sw $v0, %d($gp)", pos - base_pointer);
        print_mips(code);
    }
    else {
        sprintf(reg, "$v0");
        store_var(middle_code_list[loc].code[1], reg, middle_code_list[loc].kind);
    }
}

void store_var(char* name, char* reg, int type) {
    char code[100];
    int rs, rt, loc;
    loc = find_var(name);
    if (loc < 0) {
        rs = choose_reg_s();
        rt = choose_reg_t();
        if (rs > 0) {
            strcpy(store_info_list[store_info_sum].name, name);
            store_info_list[store_info_sum].type = type;
            store_info_list[store_info_sum].isarray = 0;
            store_info_list[store_info_sum++].reg_num = rs;
            sprintf(code, "move $s%d, %s", rs - 16, reg);
            print_mips(code);
            reg_use[rs] = 1;
        }
        else if (rt > 0) {
            strcpy(store_info_list[store_info_sum].name, name);
            store_info_list[store_info_sum].type = type;
            store_info_list[store_info_sum].isarray = 0;
            store_info_list[store_info_sum++].reg_num = rt;
            sprintf(code, "move $t%d, %s", rt - 8, reg);
            print_mips(code);
            reg_use[rt] = 1;
        }
        else {
            strcpy(store_info_list[store_info_sum].name, name);
            store_info_list[store_info_sum].type = type;
            store_info_list[store_info_sum].isarray = 0;
            store_info_list[store_info_sum].reg_num = -1;
            global_pointer -= 4;
            sprintf(code, "sw %s, %d($gp)", reg, global_pointer - base_pointer);
            print_mips(code);
            store_info_list[store_info_sum++].stack_position = global_pointer;
        }
    }
    else {
        loc = store_info_list[loc].reg_num >= 0 ?
            store_info_list[loc].reg_num : store_info_list[loc].stack_position;
        if (loc <= 31) {
            if (loc >= 16 && loc <= 25)
                sprintf(code, "move $s%d, %s", loc - 16, reg);
            else
                sprintf(code, "move $t%d, %s", loc - 8, reg);
            print_mips(code);
        }
        else {
            sprintf(code, "sw %s, %d($gp)", reg, loc - base_pointer);
            print_mips(code);
        }
    }
}

void mips_oprand(int loc, char* reg, char* op, int count) {
    char code[100];
    int position;
    if (middle_code_list[loc].code[count][0] == '+' ||
        middle_code_list[loc].code[count][0] == '-' ||
        isdigit(middle_code_list[loc].code[count][0])) {
        sprintf(code, "li %s, 0x%x", reg, atoi(middle_code_list[loc].code[count])); //存入数
        strcpy(op, reg);
        print_mips(code);
    }
    else if (middle_code_list[loc].code[count][0] == '\'') {
        sprintf(code, "li %s, 0x%x", reg, middle_code_list[loc].code[count][1]); //存入字符的ASC码
        strcpy(op, reg);
        print_mips(code);
    }
    else if ((position = find_const(middle_code_list[loc].code[count])) >= 0) {
        if (strcmp(middle_code_list[position].code[1], "int") == 0) {
            sprintf(code, "li %s, 0x%x", reg, atoi(middle_code_list[position].code[4]));
        }
        else {
            sprintf(code, "li %s, 0x%x", reg, middle_code_list[position].code[4][0]);
        }
        strcpy(op, reg);
        print_mips(code);
    }
    else {
        position = find_var(middle_code_list[loc].code[count]);
        if (position >= 0) {
            position = store_info_list[position].reg_num >= 0 ?
                store_info_list[position].reg_num : store_info_list[position].stack_position;
        }
        if (position < 31) {
            if (position >= 16 && position <= 23) {
                sprintf(op, "$s%d", position - 16);//注意这里会不会改动
            }
            else {
                sprintf(op, "$t%d", position - 8); //注意这里会不会改动
            }
        }
        else {
            sprintf(code, "lw %s, %d($gp)", reg, position - base_pointer);
            strcpy(op, reg);
            print_mips(code);
        }
    }
}

void mips_calculate(int loc) {
    int position, flag = 0;
    char code[100], op1[100], op2[100];
    char reg[100];
    sprintf(reg, "$t8");
    if (mips_deal_array(middle_code_list[loc].code[0], op1, reg, 1) == 0)
    {
        flag = 1;
    }
    else
    {
        position = find_var(middle_code_list[loc].code[0]);
        if (position >= 0) { //注意，未来增加对于数组的操作
            position = store_info_list[position].reg_num >= 0 ?
                store_info_list[position].reg_num : store_info_list[position].stack_position;
        }
    }
    if (flag == 1)
    {
        sprintf(code, "addi $sp, $sp, -4");
        print_mips(code);
        sprintf(code, "sw $v1, 0($sp)");
        print_mips(code);
    }
    sprintf(reg, "$t8");
    if(mips_deal_array(middle_code_list[loc].code[2], op1, reg, 0) == -1)
        mips_oprand(loc, reg, op1, 2);
    sprintf(reg, "$t9");
    if(mips_deal_array(middle_code_list[loc].code[4], op2, reg, 0) == -1)
        mips_oprand(loc, reg, op2, 4);
    if (flag == 1)
    {
        sprintf(code, "lw $v1, 0($sp)");
        print_mips(code);
        sprintf(code, "addi $sp, $sp, 4");
        print_mips(code);
    }
    if (strcmp(middle_code_list[loc].code[3], "+") == 0) {
        if (flag == 1) {
            sprintf(code, "addu $t8, %s, %s", op1, op2);
            print_mips(code);
            sprintf(code, "sw $t8, 0($v1)");
            print_mips(code);
        }
        else if (position <= 31 && position >= 0) {
            if (position >= 16 && position <= 25) {
                position -= 16;
                sprintf(code, "addu $s%d, %s, %s", position, op1, op2);
            }
            else {
                position -= 8;
                sprintf(code, "addu $t%d, %s, %s", position, op1, op2);
            }
            print_mips(code);
        }
        else if (position > 31) {
            sprintf(code, "addu $t8, %s, %s", op1, op2);
            print_mips(code);
            sprintf(code, "sw $t8, %d($gp)", position - base_pointer);
            print_mips(code);
        }
        else {
            sprintf(code, "addu $t8, %s, %s", op1, op2);
            print_mips(code);
            sprintf(reg, "$t8");
            store_var(middle_code_list[loc].code[0], reg, 0);
        }
    }
    else if (strcmp(middle_code_list[loc].code[3], "-") == 0) {
        if (flag == 1) {
            sprintf(code, "subu $t8, %s, %s", op1, op2);
            print_mips(code);
            sprintf(code, "sw $t8, 0($v1)");
            print_mips(code);
        }
        else if (position <= 31 && position >= 0) {
            if (position >= 16 && position <= 25) {
                position -= 16;
                sprintf(code, "subu $s%d, %s, %s", position, op1, op2);
            }
            else {
                position -= 8;
                sprintf(code, "subu $t%d, %s, %s", position, op1, op2);
            }
            print_mips(code);
        }
        else if (position > 31) {
            sprintf(code, "subu $t8, %s, %s", op1, op2);
            print_mips(code);
            sprintf(code, "sw $t8, %d($gp)", position - base_pointer);
            print_mips(code);
        }
        else {
            sprintf(code, "subu $t8, %s, %s", op1, op2);
            print_mips(code);
            sprintf(reg, "$t8");
            store_var(middle_code_list[loc].code[0], reg, 0);
        }
    }
    else if (strcmp(middle_code_list[loc].code[3], "*") == 0) {
        sprintf(code, "mult %s, %s", op1, op2);
        print_mips(code);
        if (flag == 1)
        {
            sprintf(code, "mflo $t8");
            print_mips(code);
            sprintf(code, "sw $t8, 0($v1)");
            print_mips(code);
        }
        else if (position <= 31 && position >= 0) {
            if (position >= 16 && position <= 25) {
                sprintf(code, "mflo $s%d", position - 16);
            }
            else {
                sprintf(code, "mflo $t%d", position - 8);
            }
            print_mips(code);
        }
        else if (position > 31) {
            sprintf(code, "mflo $t8");
            print_mips(code);
            sprintf(code, "sw $t8, %d($gp)", position - base_pointer);
            print_mips(code);
        }
        else {
            sprintf(code, "mflo $t8");
            print_mips(code);
            sprintf(reg, "$t8");
            store_var(middle_code_list[loc].code[0], reg, 0);
        }
    }
    else if (strcmp(middle_code_list[loc].code[3], "/") == 0) {
        sprintf(code, "div %s, %s", op1, op2);
        print_mips(code);
        if (flag == 1)
        {
            sprintf(code, "mflo $t8");
            print_mips(code);
            sprintf(code, "sw $t8, 0($v1)");
            print_mips(code);
        }
        else if (position <= 31 && position >= 0) {
            if (position >= 16 && position <= 25) {
                sprintf(code, "mflo $s%d", position - 16);
            }
            else {
                sprintf(code, "mflo $t%d", position - 8);
            }
            print_mips(code);
        }
        else if (position > 31) {
            sprintf(code, "mflo $t8");
            print_mips(code);
            sprintf(code, "sw $t8, %d($gp)", position - base_pointer);
            print_mips(code);
        }
        else {
            sprintf(code, "mflo $t8");
            print_mips(code);
            sprintf(reg, "$t8");
            store_var(middle_code_list[loc].code[0], reg, 0);
        }
    }
}

void mips_value_statement(int loc) {
    int position, flag = 0;
    char code[100], op[100];
    char reg[100];
    sprintf(reg, "$t8");
    sprintf(code, "addi $sp, $sp, -4");
    print_mips(code);
    sprintf(code, "sw $v0, 0($sp)");
    print_mips(code);
    if (mips_deal_array(middle_code_list[loc].code[0], op, reg, 1) == 0)
    {
        flag = 1;
    }
    else
    {
        position = find_var(middle_code_list[loc].code[0]);
        if (position >= 0) { //注意，未来增加对于数组的操作以及函数调用的赋值
            position = store_info_list[position].reg_num >= 0 ?
                store_info_list[position].reg_num : store_info_list[position].stack_position;
        }
    }
    sprintf(code, "lw $v0, 0($sp)");
    print_mips(code);
    sprintf(code, "addi $sp, $sp, 4");
    print_mips(code);
    if (flag)
    {
        sprintf(code, "addi $sp, $sp, -4");
        print_mips(code);
        sprintf(code, "sw $v1, 0($sp)");
        print_mips(code);
    }
    if (strcmp(middle_code_list[loc].code[2], "return_result") == 0)
    {
        strcpy(op, "$v0");
    }
    else {
        sprintf(reg, "$t8");
        if (mips_deal_array(middle_code_list[loc].code[2], op, reg, 0) == -1)
        {
            mips_oprand(loc, reg, op, 2);
        }
    }
    if (flag == 1)
    {
        sprintf(code, "lw $v1, 0($sp)");
        print_mips(code);
        sprintf(code, "addi $sp, $sp, 4");
        print_mips(code);
        sprintf(code, "sw %s, 0($v1)", op);
        print_mips(code);
    }
    else if (position >= 0 && position <= 31) {
        if (position >= 16 && position <= 25) {
            sprintf(code, "move $s%d, %s", position - 16, op);
        }
        else {
            sprintf(code, "move $t%d, %s", position - 8, op);
        }
        print_mips(code);
    }
    else if (position > 31) {
        sprintf(code, "sw %s, %d($gp)", op, position - base_pointer);
        print_mips(code);
    }
    else {
        store_var(middle_code_list[loc].code[0], op, middle_code_list[loc].kind);
    }
}

void mips_jump(int loc)
{
    char code[100];
    if (strcmp(middle_code_list[loc].code[0], "jump") == 0)
    {
        sprintf(code, "j %s", middle_code_list[loc].code[1]);
    }
    else if (strcmp(middle_code_list[loc].code[0], "bz") == 0)
    {
        if (strcmp(condition_op, "==") == 0)
        {
            sprintf(code, "bne %s, %s, %s", condition_exp1, condition_exp2, middle_code_list[loc].code[1]);
        }
        else if (strcmp(condition_op, "!=") == 0)
        {
            sprintf(code, "beq %s, %s, %s", condition_exp1, condition_exp2, middle_code_list[loc].code[1]);
        }
        else if (strcmp(condition_op, ">") == 0)
        {
            sprintf(code, "ble %s, %s, %s", condition_exp1, condition_exp2, middle_code_list[loc].code[1]);
        }
        else if (strcmp(condition_op, "<") == 0)
        {
            sprintf(code, "bge %s, %s, %s", condition_exp1, condition_exp2, middle_code_list[loc].code[1]);
        }
        else if (strcmp(condition_op, ">=") == 0)
        {
            sprintf(code, "blt %s, %s, %s", condition_exp1, condition_exp2, middle_code_list[loc].code[1]);
        }
        else if (strcmp(condition_op, "<=") == 0)
        {
            sprintf(code, "bgt %s, %s, %s", condition_exp1, condition_exp2, middle_code_list[loc].code[1]);
        }
    }
    else if (strcmp(middle_code_list[loc].code[0], "bnz") == 0)
    {
        if (strcmp(condition_op, "<=") == 0)
        {
            sprintf(code, "ble %s, %s, %s", condition_exp1, condition_exp2, middle_code_list[loc].code[1]);
        }
        else if (strcmp(condition_op, ">=") == 0)
        {
            sprintf(code, "bge %s, %s, %s", condition_exp1, condition_exp2, middle_code_list[loc].code[1]);
        }
        else if (strcmp(condition_op, "<") == 0)
        {
            sprintf(code, "blt %s, %s, %s", condition_exp1, condition_exp2, middle_code_list[loc].code[1]);
        }
        else if (strcmp(condition_op, ">") == 0)
        {
            sprintf(code, "bgt %s, %s, %s", condition_exp1, condition_exp2, middle_code_list[loc].code[1]);
        }
        else if (strcmp(condition_op, "!=") == 0)
        {
            sprintf(code, "bne %s, %s, %s", condition_exp1, condition_exp2, middle_code_list[loc].code[1]);
        }
        else if (strcmp(condition_op, "==") == 0)
        {
            sprintf(code, "beq %s, %s, %s", condition_exp1, condition_exp2, middle_code_list[loc].code[1]);
        }
    }
    print_mips(code);
}

void mips_condition(int loc)
{
    char reg[100];
    sprintf(reg, "$t8");
    if (mips_deal_array(middle_code_list[loc].code[0], condition_exp1, reg, 0) == -1)
        mips_oprand(loc, reg, condition_exp1, 0);
    sprintf(reg, "$t9");
    if (mips_deal_array(middle_code_list[loc].code[2], condition_exp2, reg, 0) == -1)
        mips_oprand(loc, reg, condition_exp2, 2);
    strcpy(condition_op, middle_code_list[loc].code[1]);
}

void mips_function_use(int loc)
{
    char code[100];
    sprintf(code, "jal %s", middle_code_list[loc].code[1]);
    print_mips(code);
}

void mips_return(int loc)
{
    char code[100], ret[100], reg[100];
    if (middle_code_list[loc].num == 1)
    {
        sprintf(code, "lw $ra, 0($sp)");
        print_mips(code);
        sprintf(code, "addi $sp, $sp, 4");
        print_mips(code);
        sprintf(code, "jr $ra");
        print_mips(code);
    }
    else if (middle_code_list[loc].num == 2)
    {
        sprintf(reg, "$v0");
        if (mips_deal_array(middle_code_list[loc].code[1], ret, reg, 0) == -1)
        {
            mips_oprand(loc, reg, ret, 1);
        }
        if (strcmp(ret, "$v0") != 0)
        {
            sprintf(code, "move $v0, %s", ret);
            print_mips(code);
        }
        sprintf(code, "lw $ra, 0($sp)");
        print_mips(code);
        sprintf(code, "addi $sp, $sp, 4");
        print_mips(code);
        sprintf(code, "jr $ra");
        print_mips(code);
    }
}

void mips_parameter(int loc)
{
    char reg[100], code[100];
    int count = 0;
    for (int i = loc; middle_code_list[i].type == PARAMETER; i++)
    {
        if (count < 4)
        {
            sprintf(reg, "$a%d", count);
            store_var(middle_code_list[i].code[1], reg, middle_code_list[i].kind);
        }
        else
        {
            sprintf(code, "lw $t8, 0($sp)");
            print_mips(code);
            sprintf(code, "addi $sp, $sp, 4");
            print_mips(code);
            sprintf(reg, "$t8");
            store_var(middle_code_list[i].code[1], reg, middle_code_list[i].kind);
        }
        count++;
    }
}

void mips_push(int loc)
{
    char code[100], cal[100], reg[100];
    int position, count = 0;
    for (position = loc - 1; middle_code_list[position].type == PUSH; position--)
    {

    }
    for (int i = position + 1; middle_code_list[i].type == PUSH; i++)
    {
        if (count < 4)
        {
            sprintf(reg, "$a%d", count);
            if (mips_deal_array(middle_code_list[i].code[1], cal, reg, 0) == 0)
            {
                count++;
                continue;
            }
            mips_oprand(i, reg, cal, 1);
            if (strcmp(cal, reg) != 0)
            {
                sprintf(code, "move %s, %s", reg, cal);
                print_mips(code);
            }
        }
        count++;
    }
    sprintf(reg, "$t8");
    for (int i = count; i > 4; i--)
    {
        if (mips_deal_array(middle_code_list[position + i].code[1], cal, reg, 0) == -1)
        {
            mips_oprand(position + i, reg, cal, 1);
        }
        sprintf(code, "addi $sp, $sp, -4");
        print_mips(code);
        sprintf(code, "sw %s, 0($sp)", cal);
        print_mips(code);
    }
}

void mips_save_recursion(int prev_loc, int now_loc)
{
    int reg_num;
    char code[100];
    for (int i = now_loc - 1; i >= prev_loc; i--)
    {
        reg_num = store_info_list[i].reg_num;
        if (store_info_list[i].isarray)
        {
            sprintf(code, "li $t9, 0x%x", store_info_list[i].stack_position + (store_info_list[i].sum - 1) * 4);
            print_mips(code);
            for (int j = 0; j < store_info_list[i].sum; j++)
            {
                sprintf(code, "addi $sp, $sp, -4");
                print_mips(code);
                sprintf(code, "lw $t8, 0($t9)");
                print_mips(code);
                sprintf(code, "sw $t8, 0($sp)");
                print_mips(code);
                sprintf(code, "addi $t9, $t9, -4");
                print_mips(code);
            }
        }
        else
        {
            if (reg_num >= 0)
            {
                sprintf(code, "addi $sp, $sp, -4");
                print_mips(code);
                if (reg_num >= 16 && reg_num <= 23)
                {
                    sprintf(code, "sw $s%d, 0($sp)", reg_num - 16);
                    print_mips(code);
                }
                else
                {
                    sprintf(code, "sw $t%d, 0($sp)", reg_num - 8);
                    print_mips(code);
                }
            }
            else {
                sprintf(code, "addi $sp, $sp, -4");
                print_mips(code);
                sprintf(code, "lw $t8, %d($gp)", store_info_list[i].stack_position - base_pointer);
                print_mips(code);
                sprintf(code, "sw $t8, 0($sp)");
                print_mips(code);
            }
        }
    }
    int roll_back_len = 0;
    for (int i = prev_loc; i < now_loc; i++)
    {
        if (store_info_list[i].reg_num >= -1)
        {
            back_store_info_list[back_store_info_sum].store_info_area[roll_back_len++] = store_info_list[i];
        }
    }
    back_store_info_list[back_store_info_sum++].roll_back = roll_back_len;
}

void mips_reload_recursion()
{
    char code[100], reg[100];
    sprintf(reg, "$t8");
    for (int i = 0; i < back_store_info_list[back_store_info_sum - 1].roll_back; i++)
    {
        if (back_store_info_list[back_store_info_sum - 1].store_info_area[i].isarray)
        {
            sprintf(code, "li $t8, 0x%x", back_store_info_list[back_store_info_sum - 1].store_info_area[i].stack_position);
            print_mips(code);
            for (int j = 0; j < back_store_info_list[back_store_info_sum - 1].store_info_area[i].sum; j++) {
                sprintf(code, "lw $t9, 0($sp)");
                print_mips(code);
                sprintf(code, "addi $sp, $sp, 4");
                print_mips(code);
                sprintf(code, "sw $t9, 0($t8)");
                print_mips(code);
                sprintf(code, "addi $t8, $t8, 4");
                print_mips(code);
            }
        }
        else
        {
            sprintf(code, "lw $t8, 0($sp)");
            print_mips(code);
            sprintf(code, "addi $sp, $sp, 4");
            print_mips(code);
            store_var(back_store_info_list[back_store_info_sum - 1].store_info_area[i].name, 
                reg, back_store_info_list[back_store_info_sum - 1].store_info_area[i].type);
        }
    }
    back_store_info_sum--;
}

void mips_function(int loc)
{
    int pre = store_info_sum, rs, rt;
    char func_name[100], code[100];
    strcpy(func_name, middle_code_list[loc].code[1]);
    if (middle_code_list[loc + 1].type != PARAMETER)
    {
        sprintf(code, "addi $sp, $sp, -4");
        print_mips(code);
        sprintf(code, "sw $ra, 0($sp)");
        print_mips(code);
    }
    for (int i = loc + 1; middle_code_list[i].type != MAIN && middle_code_list[i].type != HEADER; i++)
    {
        if (middle_code_list[i].type == VARIABLE_DECLARATIONS && middle_code_list[i].level > 0)
        {
            store_array_mips(i);
        }
        else if (middle_code_list[i].type == VARIABLE_DECLARATIONS && middle_code_list[i].level == 0)
        {
            rs = choose_reg_s();
            rt = choose_reg_t();
            if (rs > 0) {
                strcpy(store_info_list[store_info_sum].name, middle_code_list[i].code[2]);
                store_info_list[store_info_sum].type = middle_code_list[i].kind;
                store_info_list[store_info_sum].isarray = 0;
                store_info_list[store_info_sum++].reg_num = rs;
                if (middle_code_list[i].num == 5) {
                    if (middle_code_list[i].kind == 0) {
                        sprintf(code, "li $s%d, 0x%x", rs - 16, atoi(middle_code_list[i].code[4]));
                    }
                    else {
                        sprintf(code, "li $s%d, 0x%x", rs - 16, middle_code_list[i].code[4][1]);
                    }
                    print_mips(code);
                }
                reg_use[rs] = 1;
            }
            else if (rt > 0) {
                strcpy(store_info_list[store_info_sum].name, middle_code_list[i].code[2]);
                store_info_list[store_info_sum].type = middle_code_list[i].kind;
                store_info_list[store_info_sum].isarray = 0;
                store_info_list[store_info_sum++].reg_num = rt;
                if (middle_code_list[i].num == 5) {
                    if (middle_code_list[i].kind == 0) {
                        sprintf(code, "li $t%d, 0x%x", rt - 8, atoi(middle_code_list[i].code[4]));
                    }
                    else {
                        sprintf(code, "li $t%d, 0x%x", rt - 8, middle_code_list[i].code[4][1]);
                    }
                    print_mips(code);
                }
                reg_use[rt] = 1;
            }
            else {
                strcpy(store_info_list[store_info_sum].name, middle_code_list[i].code[2]);
                store_info_list[store_info_sum].type = middle_code_list[i].kind;
                store_info_list[store_info_sum].isarray = 0;
                store_info_list[store_info_sum].reg_num = -1;
                global_pointer -= 4;
                if (middle_code_list[i].num == 5) {
                    if (middle_code_list[i].kind == 0) {
                        sprintf(code, "li $t8, 0x%x", atoi(middle_code_list[i].code[4]));
                        print_mips(code);
                        sprintf(code, "sw $t8, %d($gp)", global_pointer - base_pointer);
                        print_mips(code);
                    }
                    else {
                        sprintf(code, "li $t8, 0x%x", middle_code_list[i].code[4][1]);
                        print_mips(code);
                        sprintf(code, "sw $t8, %d($gp)", global_pointer - base_pointer);
                        print_mips(code);
                    }
                }
                store_info_list[store_info_sum++].stack_position = global_pointer;
            }
        }
        else if (middle_code_list[i].type == SCANF_STATEMENT)
        {
            mips_scanf_statement(i);
        }
        else if (middle_code_list[i].type == PRINT_STATEMENT1)
        {
            mips_print_statement1(i);
            sprintf(code, "li $v0, 4");
            print_mips(code);
            sprintf(code, "la $a0, changeline");
            print_mips(code);
            sprintf(code, "syscall");
            print_mips(code);
        }
        else if (middle_code_list[i].type == PRINT_STATEMENT2)
        {
            mips_print_statement2(i);
            sprintf(code, "li $v0, 4");
            print_mips(code);
            sprintf(code, "la $a0, changeline");
            print_mips(code);
            sprintf(code, "syscall");
            print_mips(code);
        }
        else if (middle_code_list[i].type == PRINT_STATEMENT3)
        {
            mips_print_statement3(i);
            sprintf(code, "li $v0, 4");
            print_mips(code);
            sprintf(code, "la $a0, changeline");
            print_mips(code);
            sprintf(code, "syscall");
            print_mips(code);
        }
        else if (middle_code_list[i].type == CALCULATE)
        {
            mips_calculate(i);
        }
        else if (middle_code_list[i].type == VALUE_STATEMENT)
        {
            mips_value_statement(i);
        }
        else if (middle_code_list[i].type == COMPARE)
        {
            mips_condition(i);
        }
        else if (middle_code_list[i].type == JUMP)
        {
            mips_jump(i);
        }
        else if (middle_code_list[i].type == LABEL)
        {
            print_mips(middle_code_list[i].code[0]);
        }
        else if (middle_code_list[i].type == RETURN_STATEMENT)
        {
            mips_return(i);
        }
        else if (middle_code_list[i].type == PARAMETER)
        {
            mips_parameter(i);
            while (middle_code_list[i].type == PARAMETER)
            {
                i++;
            }
            i--;
            sprintf(code, "addi $sp, $sp, -4");
            print_mips(code);
            sprintf(code, "sw $ra, 0($sp)");
            print_mips(code);
        }
        else if (middle_code_list[i].type == FUNCTION_USE)
        {
            /*sprintf(code, "addi $sp, $sp, -4");
            print_mips(code);
            sprintf(code, "sw $ra, 0($sp)");
            print_mips(code);*/
            mips_save_recursion(pre, store_info_sum);
            mips_push(i);
            mips_function_use(i);
            mips_reload_recursion();
            /*sprintf(code, "lw $ra, 0($sp)");
            print_mips(code);
            sprintf(code, "addi $sp, $sp, 4");
            print_mips(code);*/
        }
    }
}

void deal_token(char c)
{
    if (isalpha(c) || c == '_')
    {
        int i = 0;
        s[i] = c;
        if (c >= 'A' && c <= 'Z') {
            c = c - 'A' + 'a';
        }
        s_low[i++] = c;
        while ((c = getchar())) {
            if (!isalnum(c) && c != '_') {
                fseek(in, -1, SEEK_CUR);
                break;
            }
            s[i] = c;
            if (c >= 'A' && c <= 'Z') {
                c = c - 'A' + 'a';
            }
            s_low[i++] = c;
        }
        s[i] = '\0';
        s_low[i] = '\0';
        if (strcmp(s_low, "const") == 0) {
            symbol = CONSTTK;
        }
        else if (strcmp(s_low, "int") == 0) {
            symbol = INTTK;
        }
        else if (strcmp(s_low, "char") == 0) {
            symbol = CHARTK;
        }
        else if (strcmp(s_low, "void") == 0) {
            symbol = VOIDTK;
        }
        else if (strcmp(s_low, "main") == 0) {
            symbol = MAINTK;
        }
        else if (strcmp(s_low, "if") == 0) {
            symbol = IFTK;
        }
        else if (strcmp(s_low, "else") == 0) {
            symbol = ELSETK;
        }
        else if (strcmp(s_low, "switch") == 0) {
            symbol = SWITCHTK;
        }
        else if (strcmp(s_low, "case") == 0) {
            symbol = CASETK;
        }
        else if (strcmp(s_low, "default") == 0) {
            symbol = DEFAULTTK;
        }
        else if (strcmp(s_low, "while") == 0) {
            symbol = WHILETK;
        }
        else if (strcmp(s_low, "for") == 0) {
            symbol = FORTK;
        }
        else if (strcmp(s_low, "scanf") == 0) {
            symbol = SCANFTK;
        }
        else if (strcmp(s_low, "printf") == 0) {
            symbol = PRINTFTK;
        }
        else if (strcmp(s_low, "return") == 0) {
            symbol = RETURNTK;
        }
        else {
            symbol = IDENFR;
        }
    }
    else if (isdigit(c)) {
        int i = 0;
        s[i++] = c;
        while ((c = getchar())) {
            if (!isdigit(c)) {
                fseek(in, -1, SEEK_CUR);
                break;
            }
            s[i++] = c;
        }
        s[i] = '\0';
        symbol = INTCON;
    }
    else if (c == '\'')
    {
        int i = 0;
        c = getchar();
        s[i++] = c;
        s[i] = '\0';
        c = getchar();
        symbol = CHARCON;
    }
    else if (c == '"')
    {
        int i = 0;
        while ((c = getchar())) {
            if (c == '"') {
                break;
            }
            if (c == '\\') {
                s[i++] = '\\';
            }
            s[i++] = c;
        }
        s[i] = '\0';
        symbol = STRCON;
    }
    else if (c == '+') {
        symbol = PLUS;
        strcpy(s, "+");
    }
    else if (c == '-') {
        symbol = MINU;
        strcpy(s, "-");
    }
    else if (c == '*') {
        symbol = MULT;
        strcpy(s, "*");
    }
    else if (c == '/') {
        symbol = DIV;
        strcpy(s, "/");
    }
    else if (c == '<') {
        c = getchar();
        if (c == '=') {
            symbol = LEQ;
            strcpy(s, "<=");
        }
        else {
            fseek(in, -1, SEEK_CUR);
            symbol = LSS;
            strcpy(s, "<");
        }
    }
    else if (c == '>') {
        c = getchar();
        if (c == '=') {
            symbol = GEQ;
            strcpy(s, ">=");
        }
        else {
            fseek(in, -1, SEEK_CUR);
            symbol = GRE;
            strcpy(s, ">");
        }
    }
    else if (c == '=') {
        c = getchar();
        if (c == '=') {
            symbol = EQL;
            strcpy(s, "==");
        }
        else {
            fseek(in, -1, SEEK_CUR);
            symbol = ASSIGN;
            strcpy(s, "=");
        }
    }
    else if (c == '!') {
        c = getchar();
        if (c == '=') {
            symbol = NEQ;
            strcpy(s, "!=");
        }
    }
    else if (c == ':') {
        symbol = COLON;
        strcpy(s, ":");
    }
    else if (c == ';') {
        symbol = SEMICN;
        strcpy(s, ";");
    }
    else if (c == ',') {
        symbol = COMMA;
        strcpy(s, ",");
    }
    else if (c == '(') {
        symbol = LPARENT;
        strcpy(s, "(");
    }
    else if (c == ')') {
        symbol = RPARENT;
        strcpy(s, ")");
    }
    else if (c == '[') {
        symbol = LBRACK;
        strcpy(s, "[");
    }
    else if (c == ']') {
        symbol = RBRACK;
        strcpy(s, "]");
    }
    else if (c == '{') {
        symbol = LBRACE;
        strcpy(s, "{");
    }
    else if (c == '}') {
        symbol = RBRACE;
        strcpy(s, "}");
    }
}

void getsym() {
    c = getchar();
    while (isspace(c)) {
        c = getchar();
    }
    if (c == EOF) {
        return;
    }
    deal_token(c);
}

int find_return_func(char* name)
{
    int i;
    for (i = 0; i < has_return_func_sum; i++)
    {
        if (strcmp(has_return_func[i].name, name) == 0)
            return i;
    }
    return -1;
}

int find_no_return_func(char* name)
{
    int i;
    for (i = 0; i < no_return_func_sum; i++)
    {
        if (strcmp(no_return_func[i].name, name) == 0)
            return i;
    }
    return -1;
}

struct return_info judge_string()
{
    struct return_info tmp = { 0, 1 };
    if (symbol != STRCON)
        return fail;
    strcpy(tmp.code[0], s);
    getsym();
    //printf("<字符串>\n");
    return tmp;
}

struct return_info judge_program()
{
    struct return_info r;
    r = judge_const_inform();
    if (r.return_num < 0)
        return r;
    r = judge_var_inform();
    if (r.return_num < 0)
        return r;
    while (1)
    {
        if (judge_function_has_return().return_num < 0)
        {
            if (judge_function_no_return().return_num < 0)
            {
                break;
            }
        }
    }
    r = judge_main_function();
    if (r.return_num < 0)
        return r;
    //printf("<程序>\n");
    return zero;
}

struct return_info judge_const_inform()
{
    struct return_info r;
    if (symbol != CONSTTK) {
        return zero; //这里不应该返回负数，否则就直接退了
    }
    while (symbol == CONSTTK)
    {
        //print_code();
        getsym();
        r = judge_const_define();
        if (r.return_num < 0)
            return r;
        //print_code();
        getsym();
    }
    //printf("<常量说明>\n");
    return zero;
}

struct return_info judge_const_define()
{
    struct return_info r;
    int loc;
    char identify[100];
    if (symbol == INTTK)
    {
        //print_code();
        getsym();  //读标识符
        if ((loc = find_name(s_low)) == -1) {
            strcpy(name_list[name_sum].name, s_low);
            strcpy(identify, s_low); //记录下标识符
            name_list[name_sum].type = 0;
            name_list[name_sum++].use = 0;
        }
        else {
            name_list[loc].new_type = 0;
            name_list[loc].use = 1;
            sprintf(name_list[loc].new_name, "def%d", reg_index++);
            strcpy(identify, name_list[loc].new_name); //记录下标识符  
            //注意，这种情况目前不确定是否要改动
        }
        //print_code();
        getsym();  //读等号
        //print_code();
        getsym();  //读整数
        r = judge_integer();
        if (r.return_num < 0)
            return r;
        middle_code_list[middle_code_sum++] = middle_code_generate(
            CONSTANT_DECLARATIONS, 0, 5, "const", "int", identify, "=", r.code[0]);
        while (symbol == COMMA) //不出意外的话，逗号在前面读了
        {
            //print_code();
            getsym(); //读标识符
            if ((loc = find_name(s_low)) == -1) {
                strcpy(name_list[name_sum].name, s_low);
                strcpy(identify, s_low); //记录下标识符
                name_list[name_sum].type = 0;
                name_list[name_sum++].use = 0;
            }
            else {
                name_list[loc].new_type = 0;
                name_list[loc].use = 1;
                sprintf(name_list[loc].new_name, "def%d", reg_index++);
                strcpy(identify, name_list[loc].new_name); //记录下标识符 
                //注意，这种情况目前不确定是否要改动
            }
            //print_code();
            getsym(); //读等号
            //print_code();
            getsym(); //读整数
            r = judge_integer();
            if (r.return_num < 0)
                return r;
            middle_code_list[middle_code_sum++] = middle_code_generate(
                CONSTANT_DECLARATIONS, 0, 5, "const", "int", identify, "=", r.code[0]);
        }
    }
    else if (symbol == CHARTK) //???可否有else
    {
        //print_code();
        getsym();  //读标识符
        if ((loc = find_name(s_low)) == -1) {
            strcpy(name_list[name_sum].name, s_low);
            strcpy(identify, s_low); //记录下标识符
            name_list[name_sum].type = 1;
            name_list[name_sum++].use = 0;
        }
        else {
            name_list[loc].new_type = 1;
            name_list[loc].use = 1;
            sprintf(name_list[loc].new_name, "def%d", reg_index++);
            strcpy(identify, name_list[loc].new_name); //记录下标识符 
            //注意，这种情况目前不确定是否要改动
        }
        //print_code();
        getsym();  //读等号
        //print_code();
        getsym();  //读字符
        middle_code_list[middle_code_sum++] = middle_code_generate(
            CONSTANT_DECLARATIONS, 1, 5, "const", "char", identify, "=", s);
        //print_code();
        getsym();  //读逗号，因为并没有处理逗号的函数
        while (symbol == COMMA) //不出意外的话，逗号在前面读了
        {
            //print_code();
            getsym();  //读标识符
            if ((loc = find_name(s_low)) == -1) {
                strcpy(name_list[name_sum].name, s_low);
                strcpy(identify, s_low); //记录下标识符
                name_list[name_sum].type = 1;
                name_list[name_sum++].use = 0;
            }
            else {
                name_list[loc].new_type = 1;
                name_list[loc].use = 1;
                sprintf(name_list[loc].new_name, "def%d", reg_index++);
                strcpy(identify, name_list[loc].new_name); //记录下标识符 
                //注意，这种情况目前不确定是否要改动
            }
            //print_code();
            getsym();  //读等号
            //print_code();
            getsym();  //读字符
            middle_code_list[middle_code_sum++] = middle_code_generate(
                CONSTANT_DECLARATIONS, 1, 5, "const", "char", identify, "=", s);
            //print_code();
            getsym();  //读逗号，因为并没有处理逗号的函数，若不是逗号，那么读sym并跳出
        }
    }
    //printf("<常量定义>\n");
    return zero;
}

struct return_info judge_unsigned_integer()
{
    if (symbol != INTCON) {
        return fail;
    }
    struct return_info tmp = { 0, 1 };
    strcpy(tmp.code[0], s);
    //print_code(); //输出数
    getsym(); //读下一个
    //printf("<无符号整数>\n");
    return tmp;
}

struct return_info judge_integer()
{
    struct return_info r, tmp = { 0, 1 };

    if (symbol == PLUS || symbol == MINU) //???是否需要判断是不是char常量
    {
        //print_code(); //输出符号
        strcpy(tmp.code[0], s);
        getsym(); //读数
    }
    r = judge_unsigned_integer();
    if (r.return_num < 0)
        return r;
    strcat(tmp.code[0], r.code[0]);
    return tmp;
    //printf("<整数>\n");

}

struct return_info judge_head()
{
    char type[10], func_name[100], label[100];
    if ((flag1 == 1 && pre1 != INTTK && pre1 != CHARTK)
        || (flag1 == 0 && symbol != INTTK && symbol != CHARTK)) {
        return fail;
    }
    if (flag1 == 1) {
        flag1 = 0;
        strcpy(type, content1);
        if (pre1 == INTTK) {
            has_return_func[has_return_func_sum].return_type = 0;
        }
        else if (pre1 == CHARTK) {
            has_return_func[has_return_func_sum].return_type = 1;
        }
        //print_code_pre1();
    }
    else {
        //print_code();
        strcpy(type, s_low);
        if (symbol == INTTK) {
            has_return_func[has_return_func_sum].return_type = 0;
        }
        else {
            has_return_func[has_return_func_sum].return_type = 1;
        }
        getsym();
    }
    if (flag2 == 1) {
        flag2 = 0;
        strcpy(func_name, content2);
        strcpy(has_return_func[has_return_func_sum++].name, content2);
        //print_code_pre2(); //输出标识符
    }
    else {
        strcpy(func_name, s_low);
        strcpy(has_return_func[has_return_func_sum++].name, s_low);
        //print_code(); //输出标识符
        getsym();
    }
    //printf("<声明头部>\n");
    sprintf(label, "%s:", func_name);
    middle_code_list[middle_code_sum++] = middle_code_generate(LABEL, 0, 1, label);
    middle_code_list[middle_code_sum++] = middle_code_generate(HEADER, 0, 2, type, func_name);
    return zero;
}

struct return_info judge_const()
{
    struct return_info r, tmp = { 0, 1, 0 };
    if ((r = judge_integer()).return_num == 0)
    {
        //printf("<常量>\n");
        strcpy(tmp.code[0], r.code[0]);
        return tmp;
    }
    else if (s[0] == '+' || s[0] == '-' || s[0] == '*' || s[0] == '/' || (s[0] >= '0' && s[0] <= '9')
        || (s[0] >= 'a' && s[0] <= 'z') || (s[0] >= 'A' && s[0] <= 'Z') || s[0] == '_')
    {
        //print_code(); //输出这个字符
        sprintf(tmp.code[0], "\'%s\'", s);
        getsym();
        //printf("<常量>\n");
        return tmp;
    }
    else
        return fail;
}

struct return_info judge_var_inform()
{
    struct return_info r;
    int sum = 0;
    while (1)
    {
        if (symbol != INTTK && symbol != CHARTK)
        {
            break;
        }
        pre1 = symbol;
        strcpy(content1, s_low);
        flag1 = 1;
        getsym();
        pre2 = symbol;
        strcpy(content2, s_low);
        flag2 = 1;
        getsym(); //注意，下面接的有可能是无返回值函数定义，也有可能是有返回值函数定义，也有可能是个变量，也有可能是主函数
        if (symbol != LBRACK && symbol != COMMA && symbol != SEMICN && symbol != ASSIGN)
        {
            break;
        }
        r = judge_var_define();
        if (r.return_num < 0)
            return r;
        sum++;
        //print_code();
        getsym();
    }
    if (sum == 0)
        return zero; //如果为0，说明没有变量说明
    //printf("<变量说明>\n");
    return zero;
}

struct return_info judge_var_define()
{
    struct return_info r;
    int loc;
    int flag = 0, isarray = 0, i_len = 0, j_len = 0, level = 0, i = 0, j = 0;
    int type = (pre1 == INTTK) ? 0 : 1;
    char identity[100], type_string[100];
    strcpy(type_string, content1);
    //print_code_pre1(); //输出类型标识符
    flag1 = 0;
    //print_code_pre2(); //输出标识符
    if ((loc = find_name(content2)) == -1) {
        strcpy(name_list[name_sum].name, content2);
        name_list[name_sum].use = 0;
        name_list[name_sum++].type = type;
        strcpy(identity, content2);
    }
    else {
        name_list[loc].new_type = type;
        name_list[loc].use = 1;
        sprintf(name_list[loc].new_name, "def%d", reg_index++);
        strcpy(identity, name_list[loc].new_name);
        //注意，这里可能要进行改动
    }
    flag2 = 0;
    if (symbol == LBRACK)
    {
        //print_code(); //输出[
        isarray = 1;
        level++;
        getsym();
        r = judge_unsigned_integer();
        if (r.return_num < 0)
            return r;
        //print_code(); //输出]
        i_len = atoi(r.code[0]);
        getsym();
        if (symbol == LBRACK)
        {
            //print_code(); //输出[
            level++;
            getsym();
            r = judge_unsigned_integer();
            if (r.return_num < 0)
                return r;
            j_len = atoi(r.code[0]);
            //print_code(); //输出]
            getsym();
            if (symbol == ASSIGN)
            {
                flag = 1;
                //print_code(); //输出=
                getsym();
                //print_code(); //输出{
                getsym();
                //print_code(); //输出{
                getsym();
                r = judge_const();
                if (r.return_num < 0)
                    return r;
                if (strcmp(type_string, "int") == 0)
                {
                    int_map[i][j] = atoi(r.code[0]);
                    j++;
                }
                else if (strcmp(type_string, "char") == 0)
                {
                    char_map[i][j] = r.code[0][1];
                    j++;
                }
                while (symbol == COMMA)
                {
                    //print_code(); //输出,
                    getsym();
                    r = judge_const();
                    if (r.return_num < 0)
                        return r;
                    if (strcmp(type_string, "int") == 0)
                    {
                        int_map[i][j] = atoi(r.code[0]);
                        j++;
                    }
                    else if (strcmp(type_string, "char") == 0)
                    {
                        char_map[i][j] = r.code[0][1];
                        j++;
                    }
                }
                //print_code(); //输出}
                getsym();
                j = 0;
                while (symbol == COMMA)
                {
                    i++;
                    //print_code(); //输出,
                    getsym();
                    //print_code(); //输出{
                    getsym();
                    r = judge_const();
                    if (r.return_num < 0)
                        return r;
                    if (strcmp(type_string, "int") == 0)
                    {
                        int_map[i][j] = atoi(r.code[0]);
                        j++;
                    }
                    else if (strcmp(type_string, "char") == 0)
                    {
                        char_map[i][j] = r.code[0][1];
                        j++;
                    }
                    while (symbol == COMMA)
                    {
                        //print_code(); //输出,
                        getsym();
                        r = judge_const();
                        if (r.return_num < 0)
                            return r;
                        if (strcmp(type_string, "int") == 0)
                        {
                            int_map[i][j] = atoi(r.code[0]);
                            j++;
                        }
                        else if (strcmp(type_string, "char") == 0)
                        {
                            char_map[i][j] = r.code[0][1];
                            j++;
                        }
                    }
                    //print_code(); //输出}
                    j = 0;
                    getsym();
                }
                //print_code(); //输出}
                getsym();
            }
        }
        else if (symbol == ASSIGN)
        {
            flag = 1;
            //print_code(); //输出=
            getsym();
            //print_code(); //输出{
            getsym();
            r = judge_const();
            if (r.return_num < 0)
                return r;
            if (strcmp(type_string, "int") == 0)
            {
                int_map[i][j] = atoi(r.code[0]);
                j++;
            }
            else if (strcmp(type_string, "char") == 0)
            {
                char_map[i][j] = r.code[0][1];
                j++;
            }
            while (symbol == COMMA)
            {
                //print_code(); //输出,
                getsym();
                r = judge_const();
                if (r.return_num < 0)
                    return r;
                if (strcmp(type_string, "int") == 0)
                {
                    int_map[i][j] = atoi(r.code[0]);
                    j++;
                }
                else if (strcmp(type_string, "char") == 0)
                {
                    char_map[i][j] = r.code[0][1];
                    j++;
                }
            }
            //print_code(); //输出}
            getsym();
        }
    }
    else if (symbol == ASSIGN)
    {
        flag = 1;
        //print_code(); //输出=
        getsym();
        r = judge_const();
        if (r.return_num < 0)
            return r;
    }
    if (!isarray)
    {
        if (flag == 0)
            middle_code_list[middle_code_sum++] = middle_code_generate(
                VARIABLE_DECLARATIONS, type, 3, "var", type_string, identity);
        else
            middle_code_list[middle_code_sum++] = middle_code_generate(
                VARIABLE_DECLARATIONS, type, 5, "var", type_string, identity, "=", r.code[0]);
    }
    else
    {
        if (level == 1)
        {
            if (flag)
                middle_code_list[middle_code_sum++] = middle_code_generate_array1(
                    VARIABLE_DECLARATIONS, 1, i_len, level, type, 4, "var", type_string, identity, "init");
            else
                middle_code_list[middle_code_sum++] = middle_code_generate_array1(
                    VARIABLE_DECLARATIONS, 1, i_len, level, type, 3, "var", type_string, identity);
        }
        else if (level == 2)
        {
            if (flag)
                middle_code_list[middle_code_sum++] = middle_code_generate_array1(
                    VARIABLE_DECLARATIONS, i_len, j_len, level, type, 4, "var", type_string, identity, "init");
            else
                middle_code_list[middle_code_sum++] = middle_code_generate_array1(
                    VARIABLE_DECLARATIONS, i_len, j_len, level, type, 3, "var", type_string, identity);
        }
    }
    isarray = 0;
    level = 0;
    i_len = 0;
    j_len = 0;
    i = 0;
    j = 0;
    flag = 0;
    while (symbol == COMMA)
    {
        //print_code(); //输出,
        getsym();
        //print_code(); //输出标识符
        if ((loc = find_name(s_low)) == -1) {
            strcpy(name_list[name_sum].name, s_low);
            name_list[name_sum].use = 0;
            name_list[name_sum++].type = type;
            strcpy(identity, s_low);
        }
        else {
            name_list[loc].new_type = type;
            name_list[loc].use = 1;
            sprintf(name_list[loc].new_name, "def%d", reg_index++);
            strcpy(identity, name_list[loc].new_name);
            //注意，这里可能要进行改动
        }
        getsym();
        if (symbol == LBRACK)
        {
            isarray = 1;
            level++;
            //print_code(); //输出[
            getsym();
            r = judge_unsigned_integer();
            if (r.return_num < 0)
                return r;
            i_len = atoi(r.code[0]);
            //print_code(); //输出]
            getsym();
            if (symbol == LBRACK)
            {
                level++;
                //print_code(); //输出[
                getsym();
                r = judge_unsigned_integer();
                if (r.return_num < 0)
                    return r;
                j_len = atoi(r.code[0]);
                //print_code(); //输出]
                getsym();
                if (symbol == ASSIGN)
                {
                    flag = 1;
                    //print_code(); //输出=
                    getsym();
                    //print_code(); //输出{
                    getsym();
                    //print_code(); //输出{
                    getsym();
                    r = judge_const();
                    if (r.return_num < 0)
                        return r;
                    if (strcmp(type_string, "int") == 0)
                    {
                        int_map[i][j] = atoi(r.code[0]);
                        j++;
                    }
                    else if (strcmp(type_string, "char") == 0)
                    {
                        char_map[i][j] = r.code[0][0];
                        j++;
                    }
                    while (symbol == COMMA)
                    {
                        //print_code(); //输出,
                        getsym();
                        r = judge_const();
                        if (r.return_num < 0)
                            return r;
                        if (strcmp(type_string, "int") == 0)
                        {
                            int_map[i][j] = atoi(r.code[0]);
                            j++;
                        }
                        else if (strcmp(type_string, "char") == 0)
                        {
                            char_map[i][j] = r.code[0][0];
                            j++;
                        }
                    }
                    //print_code(); //输出}
                    getsym();
                    j = 0;
                    while (symbol == COMMA)
                    {
                        i++;
                        //print_code(); //输出,
                        getsym();
                        //print_code(); //输出{
                        getsym();
                        r = judge_const();
                        if (r.return_num < 0)
                            return r;
                        if (strcmp(type_string, "int") == 0)
                        {
                            int_map[i][j] = atoi(r.code[0]);
                            j++;
                        }
                        else if (strcmp(type_string, "char") == 0)
                        {
                            char_map[i][j] = r.code[0][0];
                            j++;
                        }
                        while (symbol == COMMA)
                        {
                            //print_code(); //输出,
                            getsym();
                            r = judge_const();
                            if (r.return_num < 0)
                                return r;
                            if (strcmp(type_string, "int") == 0)
                            {
                                int_map[i][j] = atoi(r.code[0]);
                                j++;
                            }
                            else if (strcmp(type_string, "char") == 0)
                            {
                                char_map[i][j] = r.code[0][0];
                                j++;
                            }
                        }
                        //print_code(); //输出}
                        getsym();
                        j = 0;
                    }
                    //print_code(); //输出}
                    getsym();
                }
            }
            else if (symbol == ASSIGN)
            {
                flag = 1;
                //print_code(); //输出=
                getsym();
                //print_code(); //输出{
                getsym();
                r = judge_const();
                if (r.return_num < 0)
                    return r;
                if (strcmp(type_string, "int") == 0)
                {
                    int_map[i][j] = atoi(r.code[0]);
                    j++;
                }
                else if (strcmp(type_string, "char") == 0)
                {
                    char_map[i][j] = r.code[0][0];
                    j++;
                }
                while (symbol == COMMA)
                {
                    //print_code(); //输出,
                    getsym();
                    r = judge_const();
                    if (r.return_num < 0)
                        return r;
                    if (strcmp(type_string, "int") == 0)
                    {
                        int_map[i][j] = atoi(r.code[0]);
                        j++;
                    }
                    else if (strcmp(type_string, "char") == 0)
                    {
                        char_map[i][j] = r.code[0][0];
                        j++;
                    }
                }
                //print_code(); //输出}
                getsym();
            }
        }
        else if (symbol == ASSIGN)
        {
            flag = 1;
            //print_code(); //输出=
            getsym();
            r = judge_const();
            if (r.return_num < 0)
                return r;
        }
        if (!isarray)
        {
            if (flag == 0)
                middle_code_list[middle_code_sum++] = middle_code_generate(
                    VARIABLE_DECLARATIONS, type, 3, "var", type_string, identity);
            else
                middle_code_list[middle_code_sum++] = middle_code_generate(
                    VARIABLE_DECLARATIONS, type, 5, "var", type_string, identity, "=", r.code[0]);
        }
        else
        {
            if (level == 1)
            {
                if (flag)
                    middle_code_list[middle_code_sum++] = middle_code_generate_array1(
                        VARIABLE_DECLARATIONS, 1, i_len, level, type, 4, "var", type_string, identity, "init");
                else
                    middle_code_list[middle_code_sum++] = middle_code_generate_array1(
                        VARIABLE_DECLARATIONS, 1, i_len, level, type, 3, "var", type_string, identity);
            }
            else if (level == 2)
            {
                if (flag)
                    middle_code_list[middle_code_sum++] = middle_code_generate_array1(
                        VARIABLE_DECLARATIONS, i_len, j_len, level, type, 4, "var", type_string, identity, "init");
                else
                    middle_code_list[middle_code_sum++] = middle_code_generate_array1(
                        VARIABLE_DECLARATIONS, i_len, j_len, level, type, 3, "var", type_string, identity);
            }
        }
        isarray = 0;
        level = 0;
        i_len = 0;
        j_len = 0;
        i = 0;
        j = 0;
        flag = 0;
        //???
    }
    /*if (flag)
        printf("<变量定义及初始化>\n");
    else
        printf("<变量定义无初始化>\n");
    printf("<变量定义>\n");*/
    return zero;
}

struct return_info judge_function_has_return()
{
    struct return_info r;
    r = judge_head();
    if (r.return_num < 0)
        return r;
    //print_code(); //输出(
    getsym();
    r = judge_parameter_list();
    if (r.return_num < 0)
        return r;
    //print_code(); //输出)
    getsym();
    //print_code(); //输出{
    getsym();
    r = judge_mix_statement();
    if (r.return_num < 0)
        return r;
    //print_code(); //输出}
    getsym();
    //printf("<有返回值函数定义>\n");
    for (int i = 0; i < name_sum; i++) {
        name_list[i].use = 0;
    }
    return zero;
}

struct return_info judge_function_no_return()
{
    struct return_info r;
    char label[100];
    pre1 = symbol;
    strcpy(content1, s_low);
    flag1 = 1;
    getsym();
    if (symbol == MAINTK)
        return fail;
    //print_code_pre1(); //输出void
    flag1 = 0;
    no_return_func[no_return_func_sum].return_type = 2;
    strcpy(no_return_func[no_return_func_sum++].name, s_low);
    sprintf(label, "%s:", s_low);
    middle_code_list[middle_code_sum++] = middle_code_generate(LABEL, 0, 1, label); //生成函数入口的标签
    middle_code_list[middle_code_sum++] = middle_code_generate(HEADER, 0, 2, "void", s_low); //记录下函数头部和名字
    //print_code(); //输出标识符
    getsym();
    //print_code(); //输出(
    getsym();
    r = judge_parameter_list();
    if (r.return_num < 0)
        return r;
    //print_code(); //输出)
    getsym();
    //print_code(); //输出{
    getsym();
    r = judge_mix_statement();
    if (r.return_num < 0)
        return r;
    //print_code(); //输出}
    getsym();
    //printf("<无返回值函数定义>\n");
    if (middle_code_list[middle_code_sum - 1].type != RETURN_STATEMENT) 
    {
        middle_code_list[middle_code_sum++] = middle_code_generate(RETURN_STATEMENT, 0, 1, "return");
    }
    for (int i = 0; i < name_sum; i++) 
    {
        name_list[i].use = 0;
    }
    return zero;
}

struct return_info judge_mix_statement()
{
    struct return_info r;
    r = judge_const_inform();
    if (r.return_num < 0)
        return r;
    r = judge_var_inform();
    if (r.return_num < 0)
        return r;
    r = judge_statement_list();
    if (r.return_num < 0)
        return r;
    //printf("<复合语句>\n");
    return zero;
}

struct return_info judge_parameter_list()
{
    int loc, type1;
    char identify[100], type[10];
    if (symbol == RPARENT) //空的情况
    {
        //printf("<参数表>\n");
        return zero;
    }
    //print_code(); //输出int或char
    strcpy(type, s_low);
    if (strcmp(type, "int") == 0) {
        type1 = 0;
    }
    else {
        type1 = 1;
    }
    getsym();
    //print_code(); //输出标识符
    loc = find_name(s_low);
    if (loc == -1) {
        strcpy(name_list[name_sum].name, s_low);
        name_list[name_sum].use = 0;
        name_list[name_sum++].type = type1;
        strcpy(identify, s_low);
    }
    else {
        name_list[loc].new_type = type1;
        name_list[loc].use = 1;
        sprintf(name_list[loc].new_name, "def%d", reg_index++);
        strcpy(identify, name_list[loc].new_name);
    }
    middle_code_list[middle_code_sum++] = 
        middle_code_generate(PARAMETER, type1, 2, type, identify);
    getsym();
    while (symbol == COMMA)
    {
        //print_code(); //输出,
        getsym();
        //print_code(); //输出int或char
        strcpy(type, s_low);
        if (strcmp(type, "int") == 0) {
            type1 = 0;
        }
        else {
            type1 = 1;
        }
        getsym();
        //print_code(); //输出标识符
        loc = find_name(s_low);
        if (loc == -1) {
            strcpy(name_list[name_sum].name, s_low);
            name_list[name_sum].use = 0;
            name_list[name_sum++].type = type1;
            strcpy(identify, s_low);
        }
        else {
            name_list[loc].new_type = type1;
            name_list[loc].use = 1;
            sprintf(name_list[loc].new_name, "def%d", reg_index++);
            strcpy(identify, name_list[loc].new_name);
        }
        middle_code_list[middle_code_sum++] =
            middle_code_generate(PARAMETER, type1, 2, type, identify);
        getsym();
    }
    //printf("<参数表>\n");
    return zero;
}

struct return_info judge_main_function()
{
    struct return_info r;
    if (flag1 == 1)
    {
        flag1 = 0;
        //print_code_pre1();
    }
    else
    {
        //print_code();
        getsym();
    }//两种情况决定输出void
    //print_code(); //输出main
    getsym();
    //print_code(); //输出(
    getsym();
    //print_code(); //输出)
    getsym();
    main_location = middle_code_sum;
    middle_code_list[middle_code_sum++] =
        middle_code_generate(MAIN, 0, 1, "main");
    //print_code(); //输出{
    getsym();
    r = judge_mix_statement();
    if (r.return_num < 0)
        return r;
    //print_code(); //输出}
    //到最后啦，不用读了
    //printf("<主函数>\n");
    return zero;
}

struct return_info judge_expression()
{
    struct return_info r, ret = { 0, 0, 0 };
    char y[100], z[100], op[10], x[100];
    int flag = 0;
    if (symbol == PLUS || symbol == MINU)
    {
        strcpy(op, s);
        flag = 1;
        //print_code();
        getsym();
    }
    r = judge_item();
    if (r.return_num < 0)
        return r;
    ret.return_num = r.return_num;
    if (flag) {
        ret.return_num = 1;
        strcpy(z, r.code[0]);
        sprintf(x, "reg%d", reg_index++);
        middle_code_list[middle_code_sum++] = middle_code_generate(
            CALCULATE, 0, 5, x, "=", "0", op, z);
        strcpy(y, x);
    }
    else {
        strcpy(y, r.code[0]);
        ret.type = r.type;
    }
    while (symbol == PLUS || symbol == MINU)
    {
        ret.return_num = 1;
        ret.type = 0;
        strcpy(op, s);
        //print_code();
        getsym();
        r = judge_item();
        if (r.return_num < 0)
            return r;
        strcpy(z, r.code[0]);
        sprintf(x, "reg%d", reg_index++);
        middle_code_list[middle_code_sum++] = middle_code_generate(
            CALCULATE, 0, 5, x, "=", y, op, z);
        strcpy(y, x);
    }
    strcpy(ret.code[0], y);
    //printf("<表达式>\n");
    return ret;
}

struct return_info judge_item()
{
    struct return_info r, ret = { 0 };
    char y[100], z[100], op[10], x[100];
    r = judge_factor();
    if (r.return_num < 0)
        return r;
    strcpy(y, r.code[0]);
    ret.type = r.type;
    ret.return_num = r.return_num;
    while (symbol == MULT || symbol == DIV)
    {
        //print_code();
        ret.return_num = 1;
        ret.type = 0;
        strcpy(op, s);
        getsym();
        r = judge_factor();
        if (r.return_num < 0)
            return r;
        strcpy(z, r.code[0]);
        sprintf(x, "reg%d", reg_index++);
        middle_code_list[middle_code_sum++] = middle_code_generate(
            CALCULATE, 0, 5, x, "=", y, op, z);
        strcpy(y, x);
    }
    strcpy(ret.code[0], y);
    //printf("<项>\n");
    return ret;
}

struct return_info judge_factor()
{
    struct return_info r, ret = { 0, 1, 0 };
    int loc, count = 0, flag = 0;
    char identify[100], exp1[100], exp2[100];
    if ((r = judge_returnfunc_use()).return_num == 0)
    {
        ret.return_num = 233; //这个表示当前的因子是有返回值函数调用
        strcpy(ret.code[0], r.code[0]);
        ret.type = r.type;
        //应该不用干什么
    }
    else if (symbol == IDENFR)
    {
        //print_code(); //输出标识符
        loc = find_name(s_low);
        if (name_list[loc].use == 1) {
            strcpy(identify, name_list[loc].new_name);
            ret.type = name_list[loc].new_type;
        }
        else {
            strcpy(identify, s_low);
            ret.type = name_list[loc].type;
        }
        getsym();
        while (symbol == LBRACK)
        {
            count++;
            flag = 1;
            //print_code(); //输出[
            getsym();
            r = judge_expression();
            if (r.return_num < 0)
                return r;
            if (count == 1)
            {
                strcpy(exp1, r.code[0]);
            }
            else if (count == 2)
            {
                strcpy(exp2, r.code[0]);
            }
            //print_code(); //输出]
            getsym();
        }
        if (count == 0)
        {
            strcpy(ret.code[0], identify);
        }
        else if (count == 1) 
        {
            sprintf(ret.code[0], "%s[%s]?", identify, exp1);
        }
        else if (count == 2) 
        {
            sprintf(ret.code[0], "%s[%s][%s]#", identify, exp1, exp2);
        }
    }
    else if (symbol == LPARENT)
    {
        //print_code(); //输出(
        getsym();
        r = judge_expression();
        if (r.return_num < 0)
            return r;
        //print_code(); //输出)
        strcpy(ret.code[0], r.code[0]);
        getsym();
    }
    else if (symbol == CHARCON)
    {
        //print_code(); //输出字符
        char charcon[100];
        sprintf(charcon, "\'%s\'", s);
        strcpy(ret.code[0], charcon);
        ret.type = 1;
        getsym();
    }
    else if ((r = judge_integer()).return_num == 0)
    {
        strcpy(ret.code[0], r.code[0]);
        //应该不用干什么
    }
    else {
        return fail;
    }
    //printf("<因子>\n");
    return ret;
}

struct return_info judge_statement()
{
    struct return_info r;
    if (symbol == SEMICN)
    { //空的情况
        //print_code(); //输出;
        getsym();
    }
    else if (judge_circulation_statement().return_num == 0)
    {
        //应该不用干什么
    }
    else if (judge_condition_statement().return_num == 0)
    {
        //应该不用干什么
    }
    else if (judge_return_statement().return_num == 0)
    {
        //print_code(); //输出;
        getsym();
    }
    else if (judge_returnfunc_use().return_num == 0)
    {
        //print_code(); //输出;
        getsym();
    }
    else if (judge_noreturnfunc_use().return_num == 0)
    {
        //print_code(); //输出;
        getsym();
    }
    else if (judge_value_statement().return_num == 0)
    {
        //print_code(); //输出;
        getsym();
    }
    else if (judge_scanf().return_num == 0)
    {
        //print_code(); //输出;
        getsym();
    }
    else if (judge_printf().return_num == 0)
    {
        //print_code(); //输出;
        getsym();
    }
    else if (judge_case_statement().return_num == 0)
    {
        //应该不用干什么
    }
    else if (symbol == LBRACE)
    {
        //print_code(); //输出{
        getsym();
        r = judge_statement_list();
        if (r.return_num < 0)
            return r;
        //print_code(); //输出}
        getsym();
    }
    else
        return fail;
    //printf("<语句>\n");
    return zero;
}

struct return_info judge_value_statement() //有针对数组的修改
{
    struct return_info r;
    char identity[100], x[100], y[100], exp1[100], exp2[100], code[100];
    int loc, count = 0;
    if (symbol != IDENFR)
    {
        return fail;
    }
    loc = find_name(s_low);
    if (name_list[loc].use == 1) {
        strcpy(identity, name_list[loc].new_name);
    }
    else {
        strcpy(identity, name_list[loc].name);
    }
    //print_code(); //输出标识符
    getsym();
    while (symbol == LBRACK)
    {
        count++;
        //print_code(); //输出[
        getsym();
        r = judge_expression();
        if (r.return_num < 0)
            return r;
        if (count == 1)
        {
            strcpy(exp1, r.code[0]);
        }
        else if (count == 2)
        {
            strcpy(exp2, r.code[0]);
        }
        //print_code(); //输出]
        getsym();
    }
    //print_code(); //输出=
    if (count == 1)
    {
        sprintf(code, "%s[%s]?", identity, exp1);
        strcpy(identity, code);
    }
    else if (count == 2)
    {
        sprintf(code, "%s[%s][%s]#", identity, exp1, exp2);
        strcpy(identity, code);
    }
    getsym();
    r = judge_expression();
    if (r.return_num < 0)
        return r;
    strcpy(y, r.code[0]);
    if (r.return_num == 1 || r.return_num == 233) { //注意，这里需要判断有返回值函数吗？？？
        strcpy(middle_code_list[middle_code_sum - 1].code[0], identity);
        middle_code_list[middle_code_sum - 1].kind = r.type;
    }
    else {
        middle_code_list[middle_code_sum++] = middle_code_generate(
            VALUE_STATEMENT, r.type, 3, identity, "=", y);
    }
    //printf("<赋值语句>\n");
    return zero;
} // 有针对数组的修改

struct return_info judge_condition_statement()
{
    struct return_info r;
    char else_label[100], end_label[100], else_label_use[100], end_label_use[100];
    if (symbol != IFTK)
        return fail;
    sprintf(else_label, "mark%d", label_sum);
    sprintf(else_label_use, "mark%d:", label_sum++);
    sprintf(end_label, "mark%d", label_sum);
    sprintf(end_label_use, "mark%d:", label_sum++);
    //print_code(); //输出if
    getsym();
    //print_code(); //输出(
    getsym();
    r = judge_condition();
    if (r.return_num < 0)
        return r;
    middle_code_list[middle_code_sum++] = middle_code_generate(JUMP, 0, 2, "bz", else_label); //???目前尚不清楚
    //print_code(); //输出)
    getsym();
    r = judge_statement();
    if (r.return_num < 0)
        return r;
    if (symbol == ELSETK)
    {
        //print_code();//输出else
        middle_code_list[middle_code_sum++] = middle_code_generate(JUMP, 0, 2, "jump", end_label);
        middle_code_list[middle_code_sum++] = middle_code_generate(LABEL, 0, 1, else_label_use);
        getsym();
        r = judge_statement();
        if (r.return_num < 0)
            return r;
    }
    else {
        middle_code_list[middle_code_sum++] = middle_code_generate(LABEL, 0, 1, else_label_use); //因为不知道前面会不会有else，为了简化处理，这里都加一个else_label
    }
    middle_code_list[middle_code_sum++] = middle_code_generate(LABEL, 0, 1, end_label_use);
    //printf("<条件语句>\n");
    return zero;
}

struct return_info judge_condition()
{
    struct return_info r;
    char exp1[100], exp2[100], op[5];
    r = judge_expression();
    if (r.return_num < 0)
        return r;
    strcpy(exp1, r.code[0]); //取出第一个数
    //print_code(); //输出关系运算符
    strcpy(op, s);
    getsym();
    r = judge_expression();
    if (r.return_num < 0)
        return r;
    strcpy(exp2, r.code[0]); //取出第二个数
    middle_code_list[middle_code_sum++] = middle_code_generate(COMPARE, 0, 3, exp1, op, exp2); //用比较语句来表示条件
    //printf("<条件>\n");
    return zero;
}

struct return_info judge_circulation_statement()
{
    struct return_info r;
    char begin_label[100], end_label[100], begin_label_use[100], end_label_use[100];
    int loc;
    char identify[100], exp1[100], exp2[100], op[5];
    if (symbol != WHILETK && symbol != FORTK)
        return fail;
    sprintf(begin_label_use, "mark%d:", label_sum);
    sprintf(begin_label, "mark%d", label_sum++);
    sprintf(end_label_use, "mark%d:", label_sum);
    sprintf(end_label, "mark%d", label_sum++);
    if (symbol == WHILETK)
    {
        middle_code_list[middle_code_sum++] = middle_code_generate(LABEL, 0, 1, begin_label_use);
        //print_code();//输出while
        getsym();
        //print_code(); //输出(
        getsym();
        r = judge_condition();
        if (r.return_num < 0)
            return r;
        middle_code_list[middle_code_sum++] = middle_code_generate(JUMP, 0, 2, "bz", end_label);
        //print_code(); //输出)
        getsym();
        r = judge_statement();
        if (r.return_num < 0)
            return r;
        middle_code_list[middle_code_sum++] = middle_code_generate(JUMP, 0, 2, "jump", begin_label); //返回循环开头的语句
        middle_code_list[middle_code_sum++] = middle_code_generate(LABEL, 0, 1, end_label_use); //跳出循环的标识
    }
    else if (symbol == FORTK)
    {
        //print_code(); //输出for
        getsym();
        //print_code(); //输出(
        getsym();
        //print_code(); //输出标识符
        loc = find_name(s_low);
        if (name_list[loc].use == 1) {
            strcpy(identify, name_list[loc].new_name);
        }
        else {
            strcpy(identify, name_list[loc].name);
        }
        getsym();
        //print_code(); //输出=
        getsym();
        r = judge_expression();
        if (r.return_num < 0)
            return r;
        //print_code(); //输出;
        strcpy(exp1, r.code[0]);
        if (r.return_num == 1) {
            strcpy(middle_code_list[middle_code_sum - 1].code[0], identify);
        }
        else {
            middle_code_list[middle_code_sum++] = middle_code_generate(
                VALUE_STATEMENT, r.type, 3, identify, "=", exp1);
        }
        getsym();
        middle_code_list[middle_code_sum++] = middle_code_generate(LABEL, 0, 1, begin_label_use);
        r = judge_condition();
        middle_code_list[middle_code_sum++] = middle_code_generate(JUMP, 0, 2, "bz", end_label);
        if (r.return_num < 0)
            return r;
        //print_code(); //输出;
        getsym();
        loc = find_name(s_low);
        if (name_list[loc].use == 1) { //遇到标识符还是要找一下
            strcpy(identify, name_list[loc].new_name);
        }
        else {
            strcpy(identify, name_list[loc].name);
        }
        //print_code(); //输出标识符
        getsym();
        //print_code(); //输出=
        getsym();
        //print_code(); //输出标识符
        loc = find_name(s_low);
        if (name_list[loc].use == 1) {
            strcpy(exp1, name_list[loc].new_name);
        }
        else {
            strcpy(exp1, name_list[loc].name);
        }
        getsym();
        //print_code(); //输出+或-
        strcpy(op, s);
        getsym();
        r = judge_stepsize();
        if (r.return_num < 0)
            return r;
        strcpy(exp2, r.code[0]);
        //print_code(); //输出)
        getsym();
        r = judge_statement();
        if (r.return_num < 0)
            return r;
        middle_code_list[middle_code_sum++] = 
            middle_code_generate(CALCULATE, 0, 5, identify, "=", exp1, op, exp2);
        middle_code_list[middle_code_sum++] = middle_code_generate(JUMP, 0, 2, "jump", begin_label);
        middle_code_list[middle_code_sum++] = middle_code_generate(LABEL, 0, 1, end_label_use);
    }
    //printf("<循环语句>\n");
    return zero;
}

struct return_info judge_stepsize()
{
    struct return_info r, ret = { 0, 1, 0 };
    r = judge_unsigned_integer();
    if (r.return_num < 0)
        return r;
    //printf("<步长>\n");
    return r;
}

struct return_info judge_case_statement()
{
    struct return_info r;
    char exp[100];
    if (symbol != SWITCHTK)
        return fail;
    //print_code(); //输出switch
    case_label_sum++; //遇到一组新的情况语句，那么就加1
    default_label_sum++; //缺省加1
    getsym();
    //print_code(); //输出(
    getsym();
    r = judge_expression();
    if (r.return_num < 0)
        return r;
    strcpy(exp, r.code[0]);
    //print_code(); //输出)
    getsym();
    //print_code(); //输出{
    getsym();
    r = judge_case_list(exp, case_label_sum);
    if (r.return_num < 0)
        return r;
    r = judge_default(case_label_sum);
    if (r.return_num < 0)
        return r;
    //print_code(); //输出}
    getsym();
    //printf("<情况语句>\n");
    return zero;
}

struct return_info judge_case_list(char* exp, int count)
{
    struct return_info r;
    r = judge_case_son_statement(exp, count);
    if (r.return_num < 0)
        return r;
    while (judge_case_son_statement(exp, count).return_num == 0)
    {
        //应该不需要干啥。。。
    }
    //printf("<情况表>\n");
    return zero;
}

struct return_info judge_case_son_statement(char* exp, int count)
{
    struct return_info r;
    char label[100], con[100], next_label[100], out_label[100], case_label_use[100], next_label_use[100], out_label_use[100];
    if (symbol != CASETK)
        return fail;
    sprintf(case_label_use, "case%dof%d:", count, case_label[count]);
    sprintf(label, "case%dof%d", count, case_label[count]++);
    sprintf(next_label_use, "case%dof%d:", count, case_label[count]);
    sprintf(next_label, "case%dof%d", count, case_label[count]);
    sprintf(out_label_use, "out%d:", count);
    sprintf(out_label, "out%d", count);
    //print_code(); //输出case
    middle_code_list[middle_code_sum++] = middle_code_generate(LABEL, 0, 1, case_label_use);
    getsym();
    r = judge_const();
    if (r.return_num < 0)
        return r;
    strcpy(con, r.code[0]); //把常量取出来
    middle_code_list[middle_code_sum++] = middle_code_generate(COMPARE, 0, 3, exp, "==", con);
    middle_code_list[middle_code_sum++] = middle_code_generate(JUMP, 0, 2, "bz", next_label);
    //print_code();//输出:
    getsym();
    r = judge_statement();
    if (r.return_num < 0)
        return r;
    middle_code_list[middle_code_sum++] = middle_code_generate(JUMP, 0, 2, "jump", out_label);
    //printf("<情况子语句>\n");
    return zero;
}

struct return_info judge_default(int count)
{
    struct return_info r;
    char else_label[100], end_label[100], else_label_use[100], end_label_use[100];
    if (symbol != DEFAULTTK)
        return fail;
    sprintf(else_label_use, "case%dof%d:", count, case_label[count]);
    sprintf(else_label, "case%dof%d", count, case_label[count]++);
    sprintf(end_label_use, "out%d:", count);
    sprintf(end_label, "out%d", count);
    middle_code_list[middle_code_sum++] = middle_code_generate(LABEL, 0, 1, else_label_use);
    //print_code(); //输出default
    getsym();
    //print_code(); //输出:
    getsym();
    r = judge_statement();
    if (r.return_num < 0)
        return r;
    //printf("<缺省>\n");
    middle_code_list[middle_code_sum++] = middle_code_generate(LABEL, 0, 1, end_label_use);
    return zero;
}

struct return_info judge_returnfunc_use() //注意，这里有可能要把有无函数调用合二为一
{
    struct return_info r, ret = { 0, 0, 0 };
    char tmp[1000], code[100];
    int loc;
    if (symbol != IDENFR)
        return fail;
    strcpy(tmp, s_low);
    if (find_return_func(tmp) == -1)
        return fail;
    loc = find_return_func(tmp);
    //print_code(); //输出标识符
    getsym();
    //print_code(); //输出(
    getsym();
    r = judge_value_parameter_list();
    if (r.return_num < 0)
        return r;
    //print_code(); //输出)
    ret.type = has_return_func[loc].return_type;
    sprintf(code, "def%d", reg_index++);
    middle_code_list[middle_code_sum++] = middle_code_generate(FUNCTION_USE, 0, 2, "call", tmp);
    middle_code_list[middle_code_sum++] = 
        middle_code_generate(VALUE_STATEMENT, ret.type, 3, code, "=", "return_result"); //表示函数调用的结果赋值，到$v0里面取
    strcpy(ret.code[0], code);
    getsym();
    //printf("<有返回值函数调用语句>\n");
    return ret;
}

struct return_info judge_noreturnfunc_use()
{
    struct return_info r, ret = { 0, 0, 0 };
    char tmp[1000];
    int loc;
    if (symbol != IDENFR)
        return fail;
    strcpy(tmp, s_low);
    if (find_no_return_func(tmp) == -1)
        return fail;
    //print_code(); //输出标识符
    loc = find_no_return_func(tmp);
    getsym();
    //print_code(); //输出(
    getsym();
    r = judge_value_parameter_list();
    if (r.return_num < 0)
        return fail;
    //print_code(); //输出)
    middle_code_list[middle_code_sum++] = middle_code_generate(FUNCTION_USE, 0, 2, "call", tmp);
    getsym();
    //printf("<无返回值函数调用语句>\n");
    return ret;
}

struct return_info judge_value_parameter_list()
{
    struct return_info r;
    int type[100], type_sum = 0;
    char name[100][100];
    if (symbol == RPARENT)
    { //空的情况
        //printf("<值参数表>\n");
        return zero;
    }
    r = judge_expression();
    if (r.return_num < 0)
        return r;
    strcpy(name[type_sum], r.code[0]);
    type[type_sum++] = r.type;
    while (symbol == COMMA)
    {
        //print_code();
        getsym();
        r = judge_expression();
        if (r.return_num < 0)
            return r;
        strcpy(name[type_sum], r.code[0]);
        type[type_sum++] = r.type;
    }
    for (int i = 0; i < type_sum; i++)
    {
        middle_code_list[middle_code_sum++] = middle_code_generate(PUSH, type[i], 2, "push", name[i]);
    }
    //printf("<值参数表>\n");
    return zero;
}

struct return_info judge_statement_list()
{
    while (judge_statement().return_num == 0)
    {
        //应该不用干什么。。。
    }
    //printf("<语句列>\n");
    return zero;
}

struct return_info judge_scanf()
{
    int loc;
    char identify[100];
    int type;
    if (symbol != SCANFTK)
        return fail;
    //print_code(); //输出scanf
    getsym();
    //print_code(); //输出(
    getsym();
    //print_code(); //输出标识符
    loc = find_name(s_low);
    if (name_list[loc].use == 1) {
        strcpy(identify, name_list[loc].new_name);
        type = name_list[loc].new_type;
    }
    else {
        strcpy(identify, name_list[loc].name);
        type = name_list[loc].type;
    }
    middle_code_list[middle_code_sum++] = middle_code_generate(
        SCANF_STATEMENT, type, 2, "scanf", identify);
    getsym();
    //print_code(); //输出)
    getsym();
    //printf("<读语句>\n");
    return zero;
}

struct return_info judge_printf()
{
    struct return_info r;
    char string[1000];
    if (symbol != PRINTFTK)
        return fail;
    //print_code(); //输出printf
    getsym();
    //print_code(); //输出(
    getsym();
    if ((r = judge_string()).return_num == 0)
    {
        strcpy(string, r.code[0]);
        //发现有字符串，判断有没有逗号来看有没有表达式
        if (symbol == COMMA)
        {
            //print_code();
            getsym();
            r = judge_expression();
            if (r.return_num < 0)
                return r;
            middle_code_list[middle_code_sum++] = middle_code_generate(
                PRINT_STATEMENT1, r.type, 3, "print", string, r.code[0]);
        }
        else {
            middle_code_list[middle_code_sum++] = middle_code_generate(
                PRINT_STATEMENT2, 0, 2, "print", string);
        }
    }
    else if ((r = judge_expression()).return_num >= 0)
    {
        middle_code_list[middle_code_sum++] = middle_code_generate(
            PRINT_STATEMENT3, r.type, 2, "print", r.code[0]);
    }
    //print_code();
    getsym();
    //printf("<写语句>\n");
    return zero;
}

struct return_info judge_return_statement()
{
    struct return_info r;
    char code[100];
    if (symbol != RETURNTK)
        return fail;
    //print_code(); //输出return
    getsym();
    if (symbol == LPARENT)
    {
        //print_code(); //输出(
        getsym();
        r = judge_expression();
        if (r.return_num < 0)
            return r;
        strcpy(code, r.code[0]);
        middle_code_list[middle_code_sum++] = middle_code_generate(RETURN_STATEMENT, 0, 2, "return", code);
        //print_code(); //输出）
        getsym();
    }
    else {
        middle_code_list[middle_code_sum++] = middle_code_generate(RETURN_STATEMENT, 0, 1, "return");
    }
    //printf("<返回语句>\n");
    return zero;
}

int main()
{
    for (int i = 0; i < 100; i++)
    {
        case_label[i] = 0;
    }
    getsym();
    judge_program();
    /*for (int i = 0; i < middle_code_sum; i++)
    {
        print_type(middle_code_list[i].type);
        for (int j = 0; j < middle_code_list[i].num; j++)
        {
            fprintf(middle, "%s ", middle_code_list[i].code[j]);
        }
        fprintf(middle, "\n");
    }*/
    mips_generate();
}
