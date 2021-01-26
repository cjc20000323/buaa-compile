#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
void deal_token(char c);
void getsym();
int find_return_func(char* name);
int find_no_return_func(char* name);
void print_mips(char* mips_code);
void mips_generate();
void mips_print_statement1(int loc);
void mips_print_statement2(int loc);
void mips_print_statement3(int loc);
void mips_scanf_statement(int loc);
void store_var(char* name, char* reg, int type);
void mips_calculate(int loc);
void mips_calculate(int loc);
void mips_value_statement(int loc);
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
struct return_info judge_case_list();
struct return_info judge_case_son_statement();
struct return_info judge_default();
struct return_info judge_returnfunc_use();
struct return_info judge_noreturnfunc_use();
struct return_info judge_value_parameter_list();
struct return_info judge_statement_list();
struct return_info judge_scanf();
struct return_info judge_printf();
struct return_info judge_return_statement();

FILE* in = freopen("testfile.txt", "r", stdin);
FILE* out = freopen("mips.txt", "w", stdout);
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

Code symbol, pre1, pre2;
char content1[1000], content2[1000];
int flag1 = 0, flag2 = 0;
char no_return_func[1000][1000];
char has_return_func[1000][1000];
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
};

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

struct Middle_code middle_code_list[10000]; //中间代码的列表
int middle_code_sum = 0; //中间代码列表的指针
int main_location;

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
    Middle_code middle_code;
    middle_code.type = type;
    middle_code.num = num;
    middle_code.kind = kind;
    va_list ap;
    va_start(ap, num);
    int i;
    for (i = 0; i < num; i++) {
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
int global_pointer = 0x10008000; //???
int base_pointer = 0x10008000;

struct store_info {
    char name[100];
    int type;
    int reg_num;
    int stack_position;
};

struct store_info store_info_list[1000];
int store_info_sum = 0;

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

void store_var1() {
    int i, rs, rt;
    char code[100];
    for (i = 0; middle_code_list[i].type != MAIN; i++) { //注意，未来可能增加对于函数调用的处理
        if (middle_code_list[i].type == VARIABLE_DECLARATIONS) {
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
                        sprintf(code, "li $s%d, 0x%x", rs - 16, middle_code_list[i].code[4][0]);
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
                        sprintf(code, "li $t%d, 0x%x", rt - 8, middle_code_list[i].code[4][0]);
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
                        sprintf(code, "li $t8, 0x%x", middle_code_list[i].code[4][0]);
                        print_mips(code);
                        sprintf(code, "sw $t8, %d($gp)", global_pointer - base_pointer);
                        print_mips(code);
                    }
                }
                store_info_list[store_info_sum++].stack_position = global_pointer;
            }
        }
    }
}

void mips_generate() {
    int i, rs, rt;
    char code[100];
    sprintf(code, ".data");
    print_mips(code);
    depth++;
    store_string_mips();
    depth--;
    sprintf(code, ".text");
    print_mips(code);
    depth++;
    store_var1(); //注意后续的数组改动
    sprintf(code, "main:");
    print_mips(code);
    depth++;
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
        else if (middle_code_list[i].type == VARIABLE_DECLARATIONS) {
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
                        sprintf(code, "li $s%d, 0x%x", rs - 16, middle_code_list[i].code[4][0]);
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
                        sprintf(code, "li $t%d, 0x%x", rt - 8, middle_code_list[i].code[4][0]);
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
                        sprintf(code, "li $t8, 0x%x", middle_code_list[i].code[4][0]);
                        print_mips(code);
                        sprintf(code, "sw $t8, %d($gp)", global_pointer - base_pointer);
                        print_mips(code);
                    }
                }
                store_info_list[store_info_sum++].stack_position = global_pointer;
            }
        }
    }
    sprintf(code, "li $v0, 10");
    print_mips(code);
    sprintf(code, "syscall");
    print_mips(code);
    depth--;
}

void mips_print_statement1(int loc) {
    char code[100];
    int pos;
    mips_print_statement2(loc);
    if (middle_code_list[loc].code[2][0] == '\'') {
        sprintf(code, "li $v0, 11");
        print_mips(code);
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
    char code[100];
    int pos;
    if (middle_code_list[loc].code[1][0] == '\'') {
        sprintf(code, "li $v0, 11");
        print_mips(code);
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
    else if (middle_code_list[loc].kind == 1) {
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
            store_info_list[store_info_sum++].reg_num = rs;
            sprintf(code, "move $s%d, %s", rs - 16, reg);
            print_mips(code);
            reg_use[rs] = 1;
        }
        else if (rt > 0) {
            strcpy(store_info_list[store_info_sum].name, name);
            store_info_list[store_info_sum].type = type;
            store_info_list[store_info_sum++].reg_num = rt;
            sprintf(code, "move $t%d, %s", rt - 8, reg);
            print_mips(code);
            reg_use[rt] = 1;
        }
        else {
            strcpy(store_info_list[store_info_sum].name, name);
            store_info_list[store_info_sum].type = type;
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
    int position;
    char code[100], op1[100], op2[100];
    char reg[100];
    position = find_var(middle_code_list[loc].code[0]);
    if (position >= 0) { //注意，未来增加对于数组的操作
        position = store_info_list[position].reg_num >= 0 ?
            store_info_list[position].reg_num : store_info_list[position].stack_position;
    }
    sprintf(reg, "$t8");
    mips_oprand(loc, reg, op1, 2);
    sprintf(reg, "$t9");
    mips_oprand(loc, reg, op2, 4);
    if (strcmp(middle_code_list[loc].code[3], "+") == 0) {
        if (position <= 31 && position >= 0) {
            if (position >= 16 && position <= 25) {
                position -= 16;
                sprintf(code, "add $s%d, %s, %s", position, op1, op2);
            }
            else {
                position -= 8;
                sprintf(code, "add $t%d, %s, %s", position, op1, op2);
            }
            print_mips(code);
        }
        else if (position > 31) {
            sprintf(code, "add $t8, %s, %s", op1, op2);
            print_mips(code);
            sprintf(code, "sw $t8, %d($gp)", position - base_pointer);
            print_mips(code);
        }
        else {
            sprintf(code, "add $t8, %s, %s", op1, op2);
            print_mips(code);
            sprintf(reg, "$t8");
            store_var(middle_code_list[loc].code[0], reg, 0);
        }
    }
    else if (strcmp(middle_code_list[loc].code[3], "-") == 0) {
        if (position <= 31 && position >= 0) {
            if (position >= 16 && position <= 25) {
                position -= 16;
                sprintf(code, "sub $s%d, %s, %s", position, op1, op2);
            }
            else {
                position -= 8;
                sprintf(code, "sub $t%d, %s, %s", position, op1, op2);
            }
            print_mips(code);
        }
        else if (position > 31) {
            sprintf(code, "sub $t8, %s, %s", op1, op2);
            print_mips(code);
            sprintf(code, "sw $t8, %d($gp)", position - base_pointer);
            print_mips(code);
        }
        else {
            sprintf(code, "sub $t8, %s, %s", op1, op2);
            print_mips(code);
            sprintf(reg, "$t8");
            store_var(middle_code_list[loc].code[0], reg, 0);
        }
    }
    else if (strcmp(middle_code_list[loc].code[3], "*") == 0) {
        sprintf(code, "mult %s, %s", op1, op2);
        print_mips(code);
        if (position <= 31 && position >= 0) {
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
        if (position <= 31 && position >= 0) {
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
    int position;
    char code[100], op[100];
    char reg[100];
    position = find_var(middle_code_list[loc].code[0]);
    if (position >= 0) { //注意，未来增加对于数组的操作
        position = store_info_list[position].reg_num >= 0 ?
            store_info_list[position].reg_num : store_info_list[position].stack_position;
    }
    sprintf(reg, "$t8");
    mips_oprand(loc, reg, op, 2);
    if (position >= 0 && position <= 31) {
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
        if (strcmp(has_return_func[i], name) == 0)
            return 1;
    }
    return 0;
}

int find_no_return_func(char* name)
{
    int i;
    for (i = 0; i < no_return_func_sum; i++)
    {
        if (strcmp(no_return_func[i], name) == 0)
            return 1;
    }
    return 0;
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
    if ((flag1 == 1 && pre1 != INTTK && pre1 != CHARTK)
        || (flag1 == 0 && symbol != INTTK && symbol != CHARTK)) {
        return fail;
    }
    if (flag1 == 1) {
        flag1 = 0;
        //print_code_pre1();
    }
    else {
        //print_code();
        getsym();
    }
    if (flag2 == 1) {
        flag2 = 0;
        strcpy(has_return_func[has_return_func_sum++], content2);
        //print_code_pre2(); //输出标识符
    }
    else {
        strcpy(has_return_func[has_return_func_sum++], s);
        //print_code(); //输出标识符
        getsym();
    }
    //printf("<声明头部>\n");
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
        strcpy(tmp.code[0], s);
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
    int flag = 0;
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
        getsym();
        r = judge_unsigned_integer();
        if (r.return_num < 0)
            return r;
        //print_code(); //输出]
        getsym();
        if (symbol == LBRACK)
        {
            //print_code(); //输出[
            getsym();
            r = judge_unsigned_integer();
            if (r.return_num < 0)
                return r;
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
                while (symbol == COMMA)
                {
                    //print_code(); //输出,
                    getsym();
                    r = judge_const();
                    if (r.return_num < 0)
                        return r;
                }
                //print_code(); //输出}
                getsym();
                while (symbol == COMMA)
                {
                    //print_code(); //输出,
                    getsym();
                    //print_code(); //输出{
                    getsym();
                    r = judge_const();
                    if (r.return_num < 0)
                        return r;
                    while (symbol == COMMA)
                    {
                        //print_code(); //输出,
                        getsym();
                        r = judge_const();
                        if (r.return_num < 0)
                            return r;
                    }
                    //print_code(); //输出}
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
            while (symbol == COMMA)
            {
                //print_code(); //输出,
                getsym();
                r = judge_const();
                if (r.return_num < 0)
                    return r;
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
    if (flag == 0)
        middle_code_list[middle_code_sum++] = middle_code_generate(
            VARIABLE_DECLARATIONS, type, 3, "var", type_string, identity);
    else
        middle_code_list[middle_code_sum++] = middle_code_generate(
            VARIABLE_DECLARATIONS, type, 5, "var", type_string, identity, "=", r.code[0]);
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
            //print_code(); //输出[
            getsym();
            r = judge_unsigned_integer();
            if (r.return_num < 0)
                return r;
            //print_code(); //输出]
            getsym();
            if (symbol == LBRACK)
            {
                //print_code(); //输出[
                getsym();
                r = judge_unsigned_integer();
                if (r.return_num < 0)
                    return r;
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
                    while (symbol == COMMA)
                    {
                        //print_code(); //输出,
                        getsym();
                        r = judge_const();
                        if (r.return_num < 0)
                            return r;
                    }
                    //print_code(); //输出}
                    getsym();
                    while (symbol == COMMA)
                    {
                        //print_code(); //输出,
                        getsym();
                        //print_code(); //输出{
                        getsym();
                        r = judge_const();
                        if (r.return_num < 0)
                            return r;
                        while (symbol == COMMA)
                        {
                            //print_code(); //输出,
                            getsym();
                            r = judge_const();
                            if (r.return_num < 0)
                                return r;
                        }
                        //print_code(); //输出}
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
                while (symbol == COMMA)
                {
                    //print_code(); //输出,
                    getsym();
                    r = judge_const();
                    if (r.return_num < 0)
                        return r;
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
        if (flag == 0)
            middle_code_list[middle_code_sum++] = middle_code_generate(
                VARIABLE_DECLARATIONS, type, 3, "var", type_string, identity);
        else
            middle_code_list[middle_code_sum++] = middle_code_generate(
                VARIABLE_DECLARATIONS, type, 5, "var", type_string, identity, "=", r.code[0]);
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
    return zero;
}

struct return_info judge_function_no_return()
{
    struct return_info r;
    pre1 = symbol;
    strcpy(content1, s_low);
    flag1 = 1;
    getsym();
    if (symbol == MAINTK)
        return fail;
    //print_code_pre1(); //输出void
    flag1 = 0;
    strcpy(no_return_func[no_return_func_sum++], s_low);
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
    if (symbol == RPARENT) //空的情况
    {
        //printf("<参数表>\n");
        return zero;
    }
    //print_code(); //输出int或char
    getsym();
    //print_code(); //输出标识符
    getsym();
    while (symbol == COMMA)
    {
        //print_code(); //输出,
        getsym();
        //print_code(); //输出int或char
        getsym();
        //print_code(); //输出标识符
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
    int loc;
    char identify[100];
    if (judge_returnfunc_use().return_num == 0)
    {
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
            //print_code(); //输出[
            getsym();
            r = judge_expression();
            if (r.return_num < 0)
                return r;
            //print_code(); //输出]
            getsym();
        }
        strcpy(ret.code[0], identify); //注意，目前不需要判断数组的情况
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
        char charcon[10];
        sprintf(charcon, "\'%s\'", s_low);
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

struct return_info judge_value_statement()
{
    struct return_info r;
    char identity[100], x[100], y[100];
    int loc;
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
        //print_code(); //输出[
        getsym();
        r = judge_expression();
        if (r.return_num < 0)
            return r;
        //print_code(); //输出]
        getsym();
    }
    //print_code(); //输出=
    getsym();
    r = judge_expression();
    if (r.return_num < 0)
        return r;
    strcpy(y, r.code[0]);
    if (r.return_num == 1) {
        strcpy(middle_code_list[middle_code_sum - 1].code[0], identity);
    }
    else {
        middle_code_list[middle_code_sum++] = middle_code_generate(
            VALUE_STATEMENT, r.type, 3, identity, "=", y);
    }
    //printf("<赋值语句>\n");
    return zero;
}

struct return_info judge_condition_statement()
{
    struct return_info r;
    if (symbol != IFTK)
        return fail;
    //print_code(); //输出if
    getsym();
    //print_code(); //输出(
    getsym();
    r = judge_condition();
    if (r.return_num < 0)
        return r;
    //print_code(); //输出)
    getsym();
    r = judge_statement();
    if (r.return_num < 0)
        return r;
    if (symbol == ELSETK)
    {
        //print_code();//输出else
        getsym();
        r = judge_statement();
        if (r.return_num < 0)
            return r;
    }
    //printf("<条件语句>\n");
    return zero;
}

struct return_info judge_condition()
{
    struct return_info r;
    r = judge_expression();
    if (r.return_num < 0)
        return r;
    //print_code(); //输出关系运算符
    getsym();
    r = judge_expression();
    if (r.return_num < 0)
        return r;
    //printf("<条件>\n");
    return zero;
}

struct return_info judge_circulation_statement()
{
    struct return_info r;
    if (symbol != WHILETK && symbol != FORTK)
        return fail;
    if (symbol == WHILETK)
    {
        //print_code();//输出while
        getsym();
        //print_code(); //输出(
        getsym();
        r = judge_condition();
        if (r.return_num < 0)
            return r;
        //print_code(); //输出)
        getsym();
        r = judge_statement();
        if (r.return_num < 0)
            return r;
    }
    else if (symbol == FORTK)
    {
        //print_code(); //输出for
        getsym();
        //print_code(); //输出(
        getsym();
        //print_code(); //输出标识符
        getsym();
        //print_code(); //输出=
        getsym();
        r = judge_expression();
        if (r.return_num < 0)
            return r;
        //print_code(); //输出;
        getsym();
        r = judge_condition();
        if (r.return_num < 0)
            return r;
        //print_code(); //输出;
        getsym();
        //print_code(); //输出标识符
        getsym();
        //print_code(); //输出=
        getsym();
        //print_code(); //输出标识符
        getsym();
        //print_code(); //输出+或-
        getsym();
        r = judge_stepsize();
        if (r.return_num < 0)
            return r;
        //print_code(); //输出)
        getsym();
        r = judge_statement();
        if (r.return_num < 0)
            return r;
    }
    //printf("<循环语句>\n");
    return zero;
}

struct return_info judge_stepsize()
{
    struct return_info r;
    r = judge_unsigned_integer();
    if (r.return_num < 0)
        return r;
    //printf("<步长>\n");
    return zero;
}

struct return_info judge_case_statement()
{
    struct return_info r;
    if (symbol != SWITCHTK)
        return fail;
    //print_code(); //输出switch
    getsym();
    //print_code(); //输出(
    getsym();
    r = judge_expression();
    if (r.return_num < 0)
        return r;
    //print_code(); //输出)
    getsym();
    //print_code(); //输出{
    getsym();
    r = judge_case_list();
    if (r.return_num < 0)
        return r;
    r = judge_default();
    if (r.return_num < 0)
        return r;
    //print_code(); //输出}
    getsym();
    //printf("<情况语句>\n");
    return zero;
}

struct return_info judge_case_list()
{
    struct return_info r;
    r = judge_case_son_statement();
    if (r.return_num < 0)
        return r;
    while (judge_case_son_statement().return_num == 0)
    {
        //应该不需要干啥。。。
    }
    //printf("<情况表>\n");
    return zero;
}

struct return_info judge_case_son_statement()
{
    struct return_info r;
    if (symbol != CASETK)
        return fail;
    //print_code(); //输出case
    getsym();
    r = judge_const();
    if (r.return_num < 0)
        return r;
    //print_code();//输出:
    getsym();
    r = judge_statement();
    if (r.return_num < 0)
        return r;
    //printf("<情况子语句>\n");
    return zero;
}

struct return_info judge_default()
{
    struct return_info r;
    if (symbol != DEFAULTTK)
        return fail;
    //print_code(); //输出default
    getsym();
    //print_code(); //输出:
    getsym();
    r = judge_statement();
    if (r.return_num < 0)
        return r;
    //printf("<缺省>\n");
    return zero;
}

struct return_info judge_returnfunc_use()
{
    struct return_info r;
    char tmp[1000];
    if (symbol != IDENFR)
        return fail;
    strcpy(tmp, s);
    if (find_return_func(tmp) == 0)
        return fail;
    //print_code(); //输出标识符
    getsym();
    //print_code(); //输出(
    getsym();
    r = judge_value_parameter_list();
    if (r.return_num < 0)
        return r;
    //print_code(); //输出)
    getsym();
    //printf("<有返回值函数调用语句>\n");
    return zero;
}

struct return_info judge_noreturnfunc_use()
{
    struct return_info r;
    char tmp[1000];
    if (symbol != IDENFR)
        return fail;
    strcpy(tmp, s);
    if (find_no_return_func(tmp) == 0)
        return fail;
    //print_code(); //输出标识符
    getsym();
    //print_code(); //输出(
    getsym();
    r = judge_value_parameter_list();
    if (r.return_num < 0)
        return fail;
    //print_code(); //输出)
    getsym();
    //printf("<无返回值函数调用语句>\n");
    return zero;
}

struct return_info judge_value_parameter_list()
{
    struct return_info r;
    if (symbol == RPARENT)
    { //空的情况
        //printf("<值参数表>\n");
        return zero;
    }
    r = judge_expression();
    if (r.return_num < 0)
        return r;
    while (symbol == COMMA)
    {
        //print_code();
        getsym();
        r = judge_expression();
        if (r.return_num < 0)
            return r;
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
        //print_code(); //输出）
        getsym();
    }
    //printf("<返回语句>\n");
    return zero;
}

int main()
{
    getsym();
    judge_program();
    mips_generate();
}