#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
void delete_name_list(int depth);
void add_name_list(char* n, int deep, int type);
int checknow_name_list(char* n, int deep);
int search_name_list(char* n, int type);
void print_code();
void print_code_pre1();
void print_code_pre2();
void print_error(struct error_info error);
void deal_token(char c);
void getsym();
int find_return_func(char* name);
int find_no_return_func(char* name);
int judge_string();
int judge_program();
int judge_const_inform();
int judge_const_define();
int judge_unsigned_integer();
int judge_integer();
int judge_head();
int judge_const();
int judge_var_inform();
int judge_var_define();
int judge_function_has_return();
int judge_function_no_return();
int judge_mix_statement();
int judge_parameter_list(int type);
int judge_main_function();
int judge_expression();
int judge_item();
int judge_factor();
int judge_statement();
int judge_value_statement();
int judge_condition_statement();
int judge_condition();
int judge_circulation_statement();
int judge_stepsize();
int judge_case_statement();
int judge_case_list(int type);
int judge_case_son_statement(int type);
int judge_default();
int judge_returnfunc_use();
int judge_noreturnfunc_use();
int judge_func_call();
int judge_value_parameter_list(char* name);
int judge_statement_list();
int judge_scanf();
int judge_printf();
int judge_return_statement();

FILE* in = freopen("testfile.txt", "r", stdin);
FILE* out = freopen("error.txt", "w", stdout);
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
void Code_map(Code symbol)
{
    switch (symbol) {
    case 0: printf("IDENFR"); break;
    case 1: printf("INTCON"); break;
    case 2: printf("CHARCON"); break;
    case 3: printf("STRCON"); break;
    case 4: printf("CONSTTK"); break;
    case 5: printf("INTTK"); break;
    case 6: printf("CHARTK"); break;
    case 7: printf("VOIDTK"); break;
    case 8: printf("MAINTK"); break;
    case 9: printf("IFTK"); break;
    case 10: printf("ELSETK"); break;
    case 11: printf("SWITCHTK"); break;
    case 12: printf("CASETK"); break;
    case 13: printf("DEFAULTTK"); break;
    case 14: printf("WHILETK"); break;
    case 15: printf("FORTK"); break;
    case 16: printf("SCANFTK"); break;
    case 17: printf("PRINTFTK"); break;
    case 18: printf("RETURNTK"); break;
    case 19: printf("PLUS"); break;
    case 20: printf("MINU"); break;
    case 21: printf("MULT"); break;
    case 22: printf("DIV"); break;
    case 23: printf("LSS"); break;
    case 24: printf("LEQ"); break;
    case 25: printf("GRE"); break;
    case 26: printf("GEQ"); break;
    case 27: printf("EQL"); break;
    case 28: printf("NEQ"); break;
    case 29: printf("COLON"); break;
    case 30: printf("ASSIGN"); break;
    case 31: printf("SEMICN"); break;
    case 32: printf("COMMA"); break;
    case 33: printf("LPARENT"); break;
    case 34: printf("RPARENT"); break;
    case 35: printf("LBRACK"); break;
    case 36: printf("RBRACK"); break;
    case 37: printf("LBRACE"); break;
    case 38: printf("RBRACE"); break;
    }
}

int linenum = 1;//当前的行数
int mark_newline = 0; //判断前一个符号是不是换行符的标记，0不是1是

struct error_info {
    int line;
    char type;
};

struct name_info {
    char name[1000]; //名字
    int type; //类型，0:const_int; 1:const_char; 2:var_int; 3:var_char; 4:func;
    int depth; //所在层次
};

struct func_info {
    char name[1000]; //函数名
    int type; //0:void, 1:int, 2:char，表示函数返回值的种类
    int parameter_type[1000]; //0:int, 1:char，按顺序表示参数的种类
    int parameter_sum; //表示参数的数量
};

struct name_info name_list[1000];  //所有名字都保存在这里，包括变量，常量，函数名
int name_sum = 0; //现在一共声明了多少个名字
int depth = 0; //这表明了某个声明所在的层次

struct func_info no_return_func[1000];
struct func_info has_return_func[1000];
int no_return_func_sum = 0, has_return_func_sum = 0;
int func_type = 0;

struct func_info search_function(char* name)
{
    int i;
    struct func_info tmp;
    tmp.type = 3;
    for (i = 0; i < has_return_func_sum; i++) {
        if (strcmp(has_return_func[i].name, name) == 0) {
            return has_return_func[i];
        }
    }
    for (i = 0; i < no_return_func_sum; i++) {
        if (strcmp(no_return_func[i].name, name) == 0) {
            return no_return_func[i];
        }
    }
    return tmp;
}

void delete_name_list(int deep) //把当前层次的所有名字全部删掉
{
    while (name_list[name_sum - 1].depth == deep) {
        name_sum--;
    }
}

void add_name_list(char* n, int deep, int type)
{
    strcpy(name_list[name_sum].name, n);
    name_list[name_sum].depth = deep;
    name_list[name_sum].type = type;
    name_sum++;
}

int checknow_name_list(char* n, int deep) //检查当前层次是否有重名的
{
    int i;
    for (i = name_sum - 1; name_list[i].depth == deep; i--)
    {
        if (strcmp(name_list[i].name, n) == 0)
        {
            return 1;
        }

    }
    return 0;
}

int search_name_list(char* n, int type)
{
    int i;
    for (i = 0; i < name_sum; i++) {
        if (strcmp(name_list[i].name, n) == 0) {
            if ((name_list[i].type == 4 && type == 4) || type == 0) {
                return i;
            }
            else {
                return -1;
            }
        }
    }
    return -1;
}

struct name_info find_name_list(char* name)
{
    int i;
    struct name_info a;
    for (i = 0; i < name_sum; i++) {
        if (strcmp(name_list[i].name, name) == 0) {
            return name_list[i];
        }
    }
    return a;
}

void print_code()
{
    Code_map(symbol);
    printf(" %s\n", s);
}

void print_code_pre1()
{
    Code_map(pre1);
    printf(" %s\n", content1);
}

void print_code_pre2()
{
    Code_map(pre2);
    printf(" %s\n", content2);
}

void print_error(struct error_info error)
{
    printf("%d %c\n", error.line, error.type);
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
    mark_newline = 0;
    c = getchar();
    while (isspace(c)) {
       //TODO
        if (c == '\n')
        {
            linenum++;
            mark_newline = 1;
        }
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
            return 1;
    }
    return 0;
}

int find_no_return_func(char* name)
{
    int i;
    for (i = 0; i < no_return_func_sum; i++)
    {
        if (strcmp(no_return_func[i].name, name) == 0)
            return 1;
    }
    return 0;
}

int judge_string()
{
    if (symbol != STRCON)
        return -1;
    //print_code();
    int i, biaoji = 0;
    for (i = 0; i < strlen(s); i++)
    {
        if (s[i] != 32 && s[i] != 33 && !(s[i] >= 35 && s[i] <= 126))
        {
            biaoji = 1;
            break;
        }
    }
    if (biaoji == 1 || strlen(s) == 0)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'a';
        print_error(error);
    }
    getsym();
    //printf("<字符串>\n");
    return 0;
}

int judge_program()
{
    int r;
    r = judge_const_inform();
    if (r < 0)
        return r;
    r = judge_var_inform();
    if (r < 0)
        return r;
    while (1)
    {
        if (judge_function_has_return() < 0)
        {
            if (judge_function_no_return() < 0)
            {
                break;
            }
        }
    }
    r = judge_main_function();
    if (r < 0)
        return r;
    //printf("<程序>\n");
    return 0;
}

int judge_const_inform()
{
    int r;
    if (symbol != CONSTTK) {
        return 0; //这里不应该返回负数，否则就直接退了
    }
    while (symbol == CONSTTK)
    {
        //print_code();
        getsym();
        r = judge_const_define();
        if (r < 0)
            return r;
        //print_code(); //输出;
        if (strcmp(s, ";") != 0)//到了句子末尾，关注是否为分号
        {
            struct error_info error;
            if (mark_newline)
            {
                error.line = linenum - 1;
                mark_newline = 0;
            }
            else
                error.line = linenum;//同时判断前面的符号是否为换行符，如果是，则错误在前一行，因为在读的时候已经加了1
            error.type = 'k';
            print_error(error);
        }
        else
        {
            getsym();
        } //如果没错，那么还要继续往下读一个
    }
    //printf("<常量说明>\n");
    return 0;
}

int judge_const_define()
{
    int r;
    if (symbol == INTTK)
    {
        //print_code();
        getsym();  //读标识符
        if (checknow_name_list(s_low, depth))
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'b';
            print_error(error);
        }
        else
        {
            add_name_list(s_low, depth, 0);
        }
        //print_code();
        getsym();  //读等号
        //print_code();
        getsym();  //读整数
        r = judge_integer();
        if (r < 0)
            return r;
        while (symbol == COMMA) //不出意外的话，逗号在前面读了
        {
            //print_code();
            getsym(); //读标识符
            if (checknow_name_list(s_low, depth))
            {
                struct error_info error;
                error.line = linenum;
                error.type = 'b';
                print_error(error);
            }
            else
            {
                add_name_list(s_low, depth, 0);
            }
            //print_code();
            getsym(); //读等号
            //print_code();
            getsym(); //读整数
            r = judge_integer();
            if (r < 0)
                return r;
        }
    }
    else if (symbol == CHARTK) //???可否有else
    {
        //print_code();
        getsym();  //读标识符
        if (checknow_name_list(s_low, depth))
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'b';
            print_error(error);
        }
        else
        {
            add_name_list(s_low, depth, 1);
        }
        //print_code();
        getsym();  //读等号
        //print_code();
        getsym();  //读字符
        if (s[0] != '_' && !(s[0] <= 'z' && s[0] >= 'a')
            && !(s[0] <= 'Z' && s[0] >= 'A') && !(s[0] >= '0' && s[0] <= '9')
            && s[0] != '+' && s[0] != '-' && s[0] != '*' && s[0] != '/')
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'a';
            print_error(error);
        }
        //print_code();
        getsym();  //读逗号，因为并没有处理逗号的函数
        while (symbol == COMMA) //不出意外的话，逗号在前面读了
        {
            //print_code();
            getsym();  //读标识符
            if (checknow_name_list(s_low, depth))
            {
                struct error_info error;
                error.line = linenum;
                error.type = 'b';
                print_error(error);
            }
            else
            {
                add_name_list(s_low, depth, 1);
            }
            //print_code();
            getsym();  //读等号
            //print_code();
            getsym();  //读字符
            if (s[0] != '_' && !(s[0] <= 'z' && s[0] >= 'a')
                && !(s[0] <= 'Z' && s[0] >= 'A') && !(s[0] >= '0' && s[0] <= '9')
                && s[0] != '+' && s[0] != '-' && s[0] != '*' && s[0] != '/')
            {
                struct error_info error;
                error.line = linenum;
                error.type = 'a';
                print_error(error);
            }
            //print_code();
            getsym();  //读逗号，因为并没有处理逗号的函数，若不是逗号，那么读sym并跳出
        }
    }
    //printf("<常量定义>\n");
    return 0;
}

int judge_unsigned_integer()
{
    if (symbol != INTCON) {
        return -1;
    }
    //print_code(); //输出数
    int tmp = atoi(s);
    getsym(); //读下一个
    //printf("<无符号整数>\n");
    return tmp;
}

int judge_integer()
{
    int r;
    if (symbol == PLUS || symbol == MINU) //???是否需要判断是不是char常量
    {
        //print_code(); //输出符号
        getsym(); //读数
    }
    r = judge_unsigned_integer();
    if (r < 0)
        return r;
    //printf("<整数>\n");
    return 0;
}

int judge_head()
{
    int type;
    if ((flag1 == 1 && pre1 != INTTK && pre1 != CHARTK)
        || (flag1 == 0 && symbol != INTTK && symbol != CHARTK)) {
        return -1;
    }
    if (flag1 == 1) {
        flag1 = 0;
        //print_code_pre1();
        if (pre1 == INTTK)
        {
            type = 1;
            has_return_func[has_return_func_sum].type = 1;
        }
        else if (pre1 == CHARTK)
        {
            type = 2;
            has_return_func[has_return_func_sum].type = 2;
        }
    }
    else {
        //print_code();
        if (symbol == INTTK)
        {
            type = 1;
            has_return_func[has_return_func_sum].type = 1;
        }
        else if (symbol == CHARTK)
        {
            type = 2;
            has_return_func[has_return_func_sum].type = 2;
        }
        getsym();
    }
    if (flag2 == 1) {
        flag2 = 0;
        if (checknow_name_list(content2, depth))
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'b';
            print_error(error);
        }
        else
        {
            add_name_list(content2, depth, 4);
        }
        strcpy(has_return_func[has_return_func_sum].name, content2);
        //print_code_pre2(); //输出标识符
    }
    else {
        if (checknow_name_list(s_low, depth))
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'b';
            print_error(error);
        }
        else
        {
            add_name_list(s_low, depth, 4);
        }
        strcpy(has_return_func[has_return_func_sum].name, s_low);
        //print_code(); //输出标识符
        getsym();
    }
    //printf("<声明头部>\n");
    return type;
}

int judge_const()
{
    if (judge_integer() == 0)
    {
        //printf("<常量>\n");
        return 0;
    }
    else if (s[0] == '+' || s[0] == '-' || s[0] == '*' || s[0] == '/' || (s[0] >= '0' && s[0] <= '9')
        || (s[0] >= 'a' && s[0] <= 'z') || (s[0] >= 'A' && s[0] <= 'Z') || s[0] == '_')
    {
        //print_code(); //输出这个字符
        getsym();
        //printf("<常量>\n");
        return 1;
    }
    else if (s[0] != '_' && !(s[0] <= 'z' && s[0] >= 'a')
        && !(s[0] <= 'Z' && s[0] >= 'A') && !(s[0] >= '0' && s[0] <= '9')
        && s[0] != '+' && s[0] != '-' && s[0] != '*' && s[0] != '/')
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'a';
        print_error(error);
        getsym(); //这里应该是要读一个，因为不存在缺少，只有错误的情况
        return 1; //不应返回负数
    }
    else
        return -1;
}

int judge_var_inform()
{
    int r;
    int sum = 0;
    while (1)
    {
        if (symbol != INTTK && symbol != CHARTK)
        {
            break;
        }
        pre1 = symbol;
        strcpy(content1, s_low); //注意这里和下面做出了改动，因为不需要输出内容，为了方便判断，将其改为小写存储
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
        if (r < 0)
            return r;
        sum++;
        //print_code();
        if (strcmp(s, ";") != 0)//到了句子末尾，关注是否为分号
        {
            struct error_info error;
            if (mark_newline)
            {
                error.line = linenum - 1;
                mark_newline = 0;
            }
            else
                error.line = linenum;//同时判断前面的符号是否为换行符，如果是，则错误在前一行，因为在读的时候已经加了1
            error.type = 'k';
            print_error(error);
        }
        else
        {
            getsym();
        } //如果没错，那么还要继续往下读一个
    }
    if (sum == 0)
        return 0; //如果为0，说明没有变量说明
    //printf("<变量说明>\n");
    return 0;
}

int judge_var_define()
{
    int r;
    int flag = 0;
    int type;
    int a1 = 0, a2 = 0;
    int sum1 = 0, sum2 = 0;
    //print_code_pre1(); //输出类型标识符
    if (pre1 == INTTK)
    {
        type = 2;
    }
    else if (pre1 == CHARTK)
    {
        type = 3;
    }
    flag1 = 0;
    //print_code_pre2(); //输出标识符
    if (checknow_name_list(content2, depth))
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'b';
        print_error(error);
    }
    else
    {
        add_name_list(content2, depth, type);
    }
    flag2 = 0;
    if (symbol == LBRACK)
    {
        sum1 = 0;
        //print_code(); //输出[
        getsym();
        r = judge_unsigned_integer();
        if (r < 0)
            return r;
        a1 = r; //这是第一个中括号里面的数
        //print_code(); //输出]
        if (strcmp(s, "]") != 0)
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'm';
            print_error(error);
        }
        else
        {
            getsym();
        }
        if (symbol == LBRACK)
        {
            sum2 = 0;
            //print_code(); //输出[
            getsym();
            r = judge_unsigned_integer();
            if (r < 0)
                return r;
            a2 = r;
            //print_code(); //输出]
            if (strcmp(s, "]") != 0)
            {
                struct error_info error;
                error.line = linenum;
                error.type = 'm';
                print_error(error);
            }
            else
            {
                getsym();
            }
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
                if (r < 0)
                    return r;
                if ((r == 0 && type == 3) || (r == 1 && type == 2))
                {
                    struct error_info error;
                    error.line = linenum;
                    error.type = 'o';
                    print_error(error);
                }
                sum2++;
                while (symbol == COMMA)
                {
                    //print_code(); //输出,
                    getsym();
                    r = judge_const();
                    if (r < 0)
                        return r;
                    if ((r == 0 && type == 3) || (r == 1 && type == 2))
                    {
                        struct error_info error;
                        error.line = linenum;
                        error.type = 'o';
                        print_error(error);
                    }
                    sum2++;
                }
                if (sum2 != a2)
                {
                    struct error_info error;
                    error.line = linenum;
                    error.type = 'n';
                    print_error(error);
                }
                //print_code(); //输出}
                sum1++;
                getsym();
                while (symbol == COMMA)
                {
                    sum2 = 0;
                    //print_code(); //输出,
                    getsym();
                    //print_code(); //输出{
                    getsym();
                    r = judge_const();
                    if (r < 0)
                        return r;
                    sum2++;
                    if ((r == 0 && type == 3) || (r == 1 && type == 2))
                    {
                        struct error_info error;
                        error.line = linenum;
                        error.type = 'o';
                        print_error(error);
                    }
                    while (symbol == COMMA)
                    {
                        //print_code(); //输出,
                        getsym();
                        r = judge_const();
                        if (r < 0)
                            return r;
                        sum2++;
                        if ((r == 0 && type == 3) || (r == 1 && type == 2))
                        {
                            struct error_info error;
                            error.line = linenum;
                            error.type = 'o';
                            print_error(error);
                        }
                    }
                    if (sum2 != a2)
                    {
                        struct error_info error;
                        error.line = linenum;
                        error.type = 'n';
                        print_error(error);
                    }
                    //print_code(); //输出}
                    sum1++;
                    getsym();
                }
                //print_code(); //输出}
                if (sum1 != a1)
                {
                    struct error_info error;
                    error.line = linenum;
                    error.type = 'n';
                    print_error(error);
                }
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
            if (r < 0)
                return r;
            sum1++;
            if ((r == 0 && type == 3) || (r == 1 && type == 2))
            {
                struct error_info error;
                error.line = linenum;
                error.type = 'o';
                print_error(error);
            }
            while (symbol == COMMA)
            {
                //print_code(); //输出,
                getsym();
                r = judge_const();
                if (r < 0)
                    return r;
                sum1++;
                if ((r == 0 && type == 3) || (r == 1 && type == 2))
                {
                    struct error_info error;
                    error.line = linenum;
                    error.type = 'o';
                    print_error(error);
                }
            }
            //print_code(); //输出}
            if (sum1 != a1)
            {
                struct error_info error;
                error.line = linenum;
                error.type = 'n';
                print_error(error);
            }
            getsym();
        }
    }
    else if (symbol == ASSIGN)
    {
        flag = 1;
        //print_code(); //输出=
        getsym();
        r = judge_const();
        if (r < 0)
            return r;
        if ((r == 0 && type == 3) || (r == 1 && type == 2))
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'o';
            print_error(error);
        }
    }
    while (symbol == COMMA)
    {
        //print_code(); //输出,
        getsym();
        if (checknow_name_list(s_low, depth))
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'b';
            print_error(error);
        }
        else
        {
            add_name_list(s_low, depth, type);
        }
        //print_code(); //输出标识符
        getsym();
        if (symbol == LBRACK)
        {
            sum1 = 0;
            sum2 = 0;
            //print_code(); //输出[
            getsym();
            r = judge_unsigned_integer();
            if (r < 0)
                return r;
            a1 = r;
            //print_code(); //输出]
            if (strcmp(s, "]") != 0)
            {
                struct error_info error;
                error.line = linenum;
                error.type = 'm';
                print_error(error);
            }
            else
            {
                getsym();
            }
            if (symbol == LBRACK)
            {
                //print_code(); //输出[
                getsym();
                r = judge_unsigned_integer();
                if (r < 0)
                    return r;
                a2 = r;
                //print_code(); //输出]
                if (strcmp(s, "]") != 0)
                {
                    struct error_info error;
                    error.line = linenum;
                    error.type = 'm';
                    print_error(error);
                }
                else
                {
                    getsym();
                }
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
                    if (r < 0)
                        return r;
                    sum2++;
                    if ((r == 0 && type == 3) || (r == 1 && type == 2))
                    {
                        struct error_info error;
                        error.line = linenum;
                        error.type = 'o';
                        print_error(error);
                    }
                    while (symbol == COMMA)
                    {
                        //print_code(); //输出,
                        getsym();
                        r = judge_const();
                        if (r < 0)
                            return r;
                        sum2++;
                        if ((r == 0 && type == 3) || (r == 1 && type == 2))
                        {
                            struct error_info error;
                            error.line = linenum;
                            error.type = 'o';
                            print_error(error);
                        }
                    }
                    //print_code(); //输出}
                    sum1++;
                    if (sum2 != a2)
                    {
                        struct error_info error;
                        error.line = linenum;
                        error.type = 'n';
                        print_error(error);
                    }
                    getsym();
                    while (symbol == COMMA)
                    {
                        sum2 = 0;
                        //print_code(); //输出,
                        getsym();
                        //print_code(); //输出{
                        getsym();
                        r = judge_const();
                        if (r < 0)
                            return r;
                        sum2++;
                        if ((r == 0 && type == 3) || (r == 1 && type == 2))
                        {
                            struct error_info error;
                            error.line = linenum;
                            error.type = 'o';
                            print_error(error);
                        }
                        while (symbol == COMMA)
                        {
                            //print_code(); //输出,
                            getsym();
                            r = judge_const();
                            if (r < 0)
                                return r;
                            sum2++;
                            if ((r == 0 && type == 3) || (r == 1 && type == 2))
                            {
                                struct error_info error;
                                error.line = linenum;
                                error.type = 'o';
                                print_error(error);
                            }
                        }
                        //print_code(); //输出}
                        sum1++;
                        if (sum2 != a2)
                        {
                            struct error_info error;
                            error.line = linenum;
                            error.type = 'n';
                            print_error(error);
                        }
                        getsym();
                    }
                    //print_code(); //输出}
                    if (sum1 != a1)
                    {
                        struct error_info error;
                        error.line = linenum;
                        error.type = 'n';
                        print_error(error);
                    }
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
                if (r < 0)
                    return r;
                sum1++;
                if ((r == 0 && type == 3) || (r == 1 && type == 2))
                {
                    struct error_info error;
                    error.line = linenum;
                    error.type = 'o';
                    print_error(error);
                }
                while (symbol == COMMA)
                {
                    //print_code(); //输出,
                    getsym();
                    r = judge_const();
                    if (r < 0)
                        return r;
                    sum1++;
                    if ((r == 0 && type == 3) || (r == 1 && type == 2))
                    {
                        struct error_info error;
                        error.line = linenum;
                        error.type = 'o';
                        print_error(error);
                    }
                }
                //print_code(); //输出}
                if (sum1 != a1)
                {
                    struct error_info error;
                    error.line = linenum;
                    error.type = 'n';
                    print_error(error);
                }
                getsym();
            }
        }
        else if (symbol == ASSIGN)
        {
            flag = 1;
            //print_code(); //输出=
            getsym();
            r = judge_const();
            if (r < 0)
                return r;
            if ((r == 0 && type == 3) || (r == 1 && type == 2))
            {
                struct error_info error;
                error.line = linenum;
                error.type = 'o';
                print_error(error);
            }
        }
        //???
    }
    //if (flag)
        //printf("<变量定义及初始化>\n");
    //else
        //printf("<变量定义无初始化>\n");
    //printf("<变量定义>\n");
    return 0;
}

int judge_function_has_return()
{
    int r, type;
    r = judge_head();
    if (r < 0)
        return r;
    //print_code(); //输出(
    type = r;
    if (type == 1)
    {
        func_type = 1;
    }
    else if (type == 2)
    {
        func_type = 2;
    }
    depth++; //注意，在这里加depth可能有问题，一会改一改看看
    getsym();
    r = judge_parameter_list(1);
    if (r < 0)
        return r;
    has_return_func_sum++;
    //print_code(); //输出)
    if (strcmp(s, ")") != 0)
    {
        struct error_info error;
        if (mark_newline)
        {
            error.line = linenum - 1;
            mark_newline = 0;
        }
        else
            error.line = linenum;
        error.type = 'l';
        print_error(error);
    }
    else
    {
        getsym();
    }
    //print_code(); //输出{
    getsym();
    r = judge_mix_statement(); //TODO注意当前位置可能要修改
    if (r < 0)
        return r;
    //print_code(); //输出}
    delete_name_list(depth);
    depth--; //将当前层次的名字都删除
    getsym();
    //printf("<有返回值函数定义>\n");
    return 0;
}

int judge_function_no_return()
{
    func_type = 0;
    int r;
    pre1 = symbol;
    strcpy(content1, s_low);
    flag1 = 1;
    getsym();
    if (symbol == MAINTK)
        return -1;
    //print_code_pre1(); //输出void
    flag1 = 0;
    if (checknow_name_list(s_low, depth))
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'b';
        print_error(error);
    }
    else
    {
        add_name_list(s_low, depth, 4);
    }
    strcpy(no_return_func[no_return_func_sum].name, s_low);
    no_return_func[no_return_func_sum].type = 0;
    //print_code(); //输出标识符
    getsym();
    //print_code(); //输出(
    depth++; //进入高一级层次
    getsym();
    r = judge_parameter_list(0);
    if (r < 0)
        return r;
    //print_code(); //输出)
    no_return_func_sum++;
    if (strcmp(s, ")") != 0)
    {
        struct error_info error;
        if (mark_newline)
        {
            error.line = linenum - 1;
            mark_newline = 0;
        }
        else
            error.line = linenum;
        error.type = 'l';
        print_error(error);
    }
    else
    {
        getsym();
    }
    //print_code(); //输出{
    getsym();
    r = judge_mix_statement();
    if (r < 0)
        return r;
    delete_name_list(depth);
    depth--;
    //print_code(); //输出}
    getsym();
    //printf("<无返回值函数定义>\n");
    return 0;
}

int judge_mix_statement()
{
    int r;
    r = judge_const_inform();
    if (r < 0)
        return r;
    r = judge_var_inform();
    if (r < 0)
        return r;
    r = judge_statement_list();
    if (r < 0)
        return r;
    if ((func_type == 1 || func_type == 2) && r == 0)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'h';
        print_error(error);
    }
    //printf("<复合语句>\n");
    return 0;
}

int judge_parameter_list(int type)
{
    int type1;
    if (symbol == RPARENT) //空的情况
    {
        //printf("<参数表>\n");
        return 0;
    }
    has_return_func[has_return_func_sum].parameter_sum = 0; //从0开始计数
    if (type == 1) {
        if (symbol == INTTK) {
            type1 = 2;
            has_return_func[has_return_func_sum].parameter_type[has_return_func[has_return_func_sum].parameter_sum++] = 0;
        }
        else {
            type1 = 3;
            has_return_func[has_return_func_sum].parameter_type[has_return_func[has_return_func_sum].parameter_sum++] = 1;
        }
    }
    else
    {
        if (symbol == INTTK) {
            type1 = 2;
            no_return_func[no_return_func_sum].parameter_type[no_return_func[no_return_func_sum].parameter_sum++] = 0;
        }
        else {
            type1 = 3;
            no_return_func[no_return_func_sum].parameter_type[no_return_func[no_return_func_sum].parameter_sum++] = 1;
        }
    }
    //print_code(); //输出int或char
    getsym();
    if (checknow_name_list(s_low, depth))
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'b';
        print_error(error);
    }
    else
    {
        add_name_list(s_low, depth, type1);
    }
    //print_code(); //输出标识符
    getsym();
    while (symbol == COMMA)
    {
        //print_code(); //输出,
        getsym();
        if (type == 1) {
            if (symbol == INTTK) {
                type1 = 2;
                has_return_func[has_return_func_sum].parameter_type[has_return_func[has_return_func_sum].parameter_sum++] = 0;
            }
            else {
                type1 = 3;
                has_return_func[has_return_func_sum].parameter_type[has_return_func[has_return_func_sum].parameter_sum++] = 1;
            }
        }
        else
        {
            if (symbol == INTTK) {
                type1 = 2;
                no_return_func[no_return_func_sum].parameter_type[no_return_func[no_return_func_sum].parameter_sum++] = 0;
            }
            else {
                type1 = 3;
                no_return_func[no_return_func_sum].parameter_type[no_return_func[no_return_func_sum].parameter_sum++] = 1;
            }
        }
        //print_code(); //输出int或char
        getsym();
        if (checknow_name_list(s_low, depth))
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'b';
            print_error(error);
        }
        else
        {
            add_name_list(s_low, depth, type1);
        }
        //print_code(); //输出标识符
        getsym();
    }
    //printf("<参数表>\n");
    return 0;
}

int judge_main_function()
{
    func_type = 0;
    int r;
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
    if (strcmp(s, ")") != 0)
    {
        struct error_info error;
        if (mark_newline)
        {
            error.line = linenum - 1;
            mark_newline = 0;
        }
        else
            error.line = linenum;
        error.type = 'l';
        print_error(error);
    }
    else
    {
        getsym();
    }
    depth++;
    //print_code(); //输出{
    getsym();
    r = judge_mix_statement();
    if (r < 0)
        return r;
    delete_name_list(depth);
    depth--;
    //print_code(); //输出}
    //到最后啦，不用读了
    //printf("<主函数>\n");
    return 0;
}

int judge_expression()
{
    int r;
    int mark = 0;
    if (symbol == PLUS || symbol == MINU)
    {
        //print_code();
        mark = 1;
        getsym();
    }
    r = judge_item();
    if (r < 0)
        return r;
    while (symbol == PLUS || symbol == MINU)
    {
        //print_code();
        mark = 1;
        getsym();
        r = judge_item();
        if (r < 0)
            return r;
    }
    //printf("<表达式>\n");
    if (mark == 1)
    {
        return 0;
    }
    else
    {
        return r;
    }
    return 0;
}

int judge_item()
{
    int r, mark = 0;
    r = judge_factor();
    if (r < 0)
        return r;
    while (symbol == MULT || symbol == DIV)
    {
        mark = 1;
        //print_code();
        getsym();
        r = judge_factor();
        if (r < 0)
            return r;
    }
    if (mark)
    {
        return 0;
    }
    else
    {
        return r;
    }
    //printf("<项>\n");
    return 0;
}

int judge_factor()
{
    int r;
    if (symbol == LPARENT)
    {
        //print_code(); //输出(
        getsym();
        r = judge_expression();
        if (r < 0)
            return r;
        //print_code(); //输出)
        if (strcmp(s, ")") != 0)
        {
            struct error_info error;
            if (mark_newline)
            {
                error.line = linenum - 1;
                mark_newline = 0;
            }
            else
                error.line = linenum;
            error.type = 'l';
            print_error(error);
        }
        else
        {
            getsym();
        }
        return 0;
    }
    else if (symbol == CHARCON)
    {
        //print_code(); //输出字符
        if (s[0] != '_' && !(s[0] <= 'z' && s[0] >= 'a')
            && !(s[0] <= 'Z' && s[0] >= 'A') && !(s[0] >= '0' && s[0] <= '9')
            && s[0] != '+' && s[0] != '-' && s[0] != '*' && s[0] != '/')
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'a';
            print_error(error);
        }
        getsym();
        return 1;
    }
    else if (judge_integer() == 0)
    {
        //应该不用干什么
        return 0;
    }
    else if (symbol == IDENFR)
    {
        //print_code(); //输出标识符
        int loc = search_name_list(s_low, 0);
        if (search_name_list(s_low, 0) < 0)
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'c';
            print_error(error);
        }
        strcpy(content1, s_low);
        pre1 = symbol;
        flag1 = 1;
        getsym();
        if (symbol == LPARENT)
        {
            if (judge_func_call() == 0)
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
        while (symbol == LBRACK)
        {
            //print_code(); //输出[
            getsym();
            r = judge_expression();
            if (r < 0)
                return r;
            if (r != 0)
            {
                struct error_info error;
                error.line = linenum;
                error.type = 'i';
                print_error(error);
            }
            //print_code(); //输出]
            if (strcmp(s, "]") != 0)
            {
                struct error_info error;
                error.line = linenum;
                error.type = 'm';
                print_error(error);
            }
            else
            {
                getsym();
            }
        }
        if (flag1 == 1) {
            flag1 = 0;
        }
        if (name_list[loc].type == 1 || name_list[loc].type == 3) {
            return 1;
        }
        else {
            return 0;
        }
    }
    else
    {
        return -1;
    }
    //printf("<因子>\n");
    return 0;
}

int judge_statement()
{
    int r, mark = 0;
    if (symbol == SEMICN)
    { //空的情况
        //print_code(); //输出;
        if (strcmp(s, ";") != 0)//到了句子末尾，关注是否为分号
        {
            struct error_info error;
            if (mark_newline)
            {
                error.line = linenum - 1;
                mark_newline = 0;
            }
            else
                error.line = linenum;//同时判断前面的符号是否为换行符，如果是，则错误在前一行，因为在读的时候已经加了1
            error.type = 'k';
            print_error(error);
        }
        else
        {
            getsym();
        } //如果没错，那么还要继续往下读一个
    }
    else if ((r = judge_circulation_statement()) >= 0)
    {
        if (r == 1)
        {
            mark = 1;
        }
        //应该不用干什么
    }
    else if ((r = judge_condition_statement()) >= 0)
    {
        if (r == 1)
        {
            mark = 1;
        }
        //应该不用干什么
    }
    else if ((r = judge_return_statement()) >= 0)
    {
        mark = 1;
        //print_code(); //输出;
        if (strcmp(s, ";") != 0)//到了句子末尾，关注是否为分号
        {
            struct error_info error;
            if (mark_newline)
            {
                error.line = linenum - 1;
                mark_newline = 0;
            }
            else
                error.line = linenum;//同时判断前面的符号是否为换行符，如果是，则错误在前一行，因为在读的时候已经加了1
            error.type = 'k';
            print_error(error);
        }
        else
        {
            getsym();
        } //如果没错，那么还要继续往下读一个
    }
    else if (judge_scanf() >= 0)
    {
        //print_code(); //输出;
        if (strcmp(s, ";") != 0)//到了句子末尾，关注是否为分号
        {
            struct error_info error;
            if (mark_newline)
            {
                error.line = linenum - 1;
                mark_newline = 0;
            }
            else
                error.line = linenum;//同时判断前面的符号是否为换行符，如果是，则错误在前一行，因为在读的时候已经加了1
            error.type = 'k';
            print_error(error);
        }
        else
        {
            getsym();
        } //如果没错，那么还要继续往下读一个
    }
    else if (judge_printf() >= 0)
    {
        //print_code(); //输出;
        if (strcmp(s, ";") != 0)//到了句子末尾，关注是否为分号
        {
            struct error_info error;
            if (mark_newline)
            {
                error.line = linenum - 1;
                mark_newline = 0;
            }
            else
                error.line = linenum;//同时判断前面的符号是否为换行符，如果是，则错误在前一行，因为在读的时候已经加了1
            error.type = 'k';
            print_error(error);
        }
        else
        {
            getsym();
        } //如果没错，那么还要继续往下读一个
    }
    else if (judge_case_statement() >= 0)
    {
        //应该不用干什么
    }
    else if (symbol == LBRACE)
    {
        //print_code(); //输出{
        getsym();
        r = judge_statement_list();
        if (r < 0)
            return r;
        //print_code(); //输出}
        getsym();
    }
    else if (judge_value_statement() >= 0)
    {
    //print_code(); //输出;
    if (strcmp(s, ";") != 0)//到了句子末尾，关注是否为分号
    {
        struct error_info error;
        if (mark_newline)
        {
            error.line = linenum - 1;
            mark_newline = 0;
        }
        else
            error.line = linenum;//同时判断前面的符号是否为换行符，如果是，则错误在前一行，因为在读的时候已经加了1
        error.type = 'k';
        print_error(error);
    }
    else
    {
        getsym();
    } //如果没错，那么还要继续往下读一个
    }
    else if (judge_func_call() >= 0)
    {
    //print_code(); //输出;
    if (strcmp(s, ";") != 0)//到了句子末尾，关注是否为分号
    {
        struct error_info error;
        if (mark_newline)
        {
            error.line = linenum - 1;
            mark_newline = 0;
        }
        else
            error.line = linenum;//同时判断前面的符号是否为换行符，如果是，则错误在前一行，因为在读的时候已经加了1
        error.type = 'k';
        print_error(error);
    }
    else
    {
        getsym();
    } //如果没错，那么还要继续往下读一个
    }
    else
        return -1;
    //printf("<语句>\n");
    if (mark)
        return 1;
    return 0;
}

int judge_value_statement()
{
    int r;
    struct name_info name;
    if (symbol != IDENFR)
    {
        return -1;
    }
    //print_code(); //输出标识符
    pre1 = symbol;
    strcpy(content1, s_low);
    flag1 = 1;
    getsym();
    if (symbol == LPARENT)
    {
        return -1;
    }
    if (search_name_list(content1, 0) < 0) //似乎是除了函数其他的名字随便填一个数就行，不管能不能匹配，找出来不是函数的就行
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'c';
        print_error(error);
    }
    name = find_name_list(content1);
    if (name.type == 0 || name.type == 1)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'j';
        print_error(error);
    }
    while (symbol == LBRACK)
    {
        //print_code(); //输出[
        flag1 = 0;
        getsym();
        r = judge_expression();
        if (r < 0)
            return r;
        if (r != 0)
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'i';
            print_error(error);  
        }
        //print_code(); //输出]
        if (strcmp(s, "]") != 0)
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'm';
            print_error(error);
        }
        else
        {
            getsym();
        }
    }
    if (flag1)
        flag1 = 0;
    //print_code(); //输出=
    getsym();
    r = judge_expression();
    if (r < 0)
        return r;
    //printf("<赋值语句>\n");
    return 0;
}

int judge_condition_statement()
{
    int r, mark = 0;
    if (symbol != IFTK)
        return -1;
    //print_code(); //输出if
    getsym();
    //print_code(); //输出(
    getsym();
    r = judge_condition();
    if (r < 0)
        return r;
    //print_code(); //输出)
    if (strcmp(s, ")") != 0)
    {
        struct error_info error;
        if (mark_newline)
        {
            error.line = linenum - 1;
            mark_newline = 0;
        }
        else
            error.line = linenum;
        error.type = 'l';
        print_error(error);
    }
    else
    {
        getsym();
    }
    r = judge_statement();
    if (r < 0)
        return r;
    if (r == 1)
    {
        mark = 1;
    }
    if (symbol == ELSETK)
    {
        //print_code();//输出else
        getsym();
        r = judge_statement();
        if (r < 0)
            return r;
        if (r == 1)
        {
            mark = 1;
        }
    }
    //printf("<条件语句>\n");
    if (mark == 1)
        return 1;
    return 0;
}

int judge_condition()
{
    int r;
    r = judge_expression();
    if (r < 0)
        return r;
    if (r != 0)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'f';
        print_error(error);
    }
    //print_code(); //输出关系运算符
    getsym();
    r = judge_expression();
    if (r < 0)
        return r;
    if (r != 0)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'f';
        print_error(error);
    }
    //printf("<条件>\n");
    return 0;
}

int judge_circulation_statement()
{
    int r, mark = 0;
    struct name_info name;
    if (symbol != WHILETK && symbol != FORTK)
        return -1;
    if (symbol == WHILETK)
    {
        //print_code();//输出while
        getsym();
        //print_code(); //输出(
        getsym();
        r = judge_condition();
        if (r < 0)
            return r;
        //print_code(); //输出)
        if (strcmp(s, ")") != 0)
        {
            struct error_info error;
            if (mark_newline)
            {
                error.line = linenum - 1;
                mark_newline = 0;
            }
            else
                error.line = linenum;
            error.type = 'l';
            print_error(error);
        }
        else
        {
            getsym();
        }
        r = judge_statement();
        if (r < 0)
            return r;
        if (r == 1)
        {
            mark = 1;
        }
    }
    else if (symbol == FORTK)
    {
        //print_code(); //输出for
        getsym();
        //print_code(); //输出(
        getsym();
        //print_code(); //输出标识符
        if (search_name_list(s_low, 0) < 0)
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'c';
            print_error(error);
        }
        name = find_name_list(s_low);
        if (name.type == 0 || name.type == 1)
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'j';
            print_error(error);
        }
        getsym();
        //print_code(); //输出=
        getsym();
        r = judge_expression();
        if (r < 0)
            return r;
        //print_code(); //输出;
        if (strcmp(s, ";") != 0)//到了句子末尾，关注是否为分号
        {
            struct error_info error;
            if (mark_newline)
            {
                error.line = linenum - 1;
                mark_newline = 0;
            }
            else
                error.line = linenum;//同时判断前面的符号是否为换行符，如果是，则错误在前一行，因为在读的时候已经加了1
            error.type = 'k';
            print_error(error);
        }
        else
        {
            getsym();
        } //如果没错，那么还要继续往下读一个
        r = judge_condition();
        if (r < 0)
            return r;
        //print_code(); //输出;
        if (strcmp(s, ";") != 0)//到了句子末尾，关注是否为分号
        {
            struct error_info error;
            if (mark_newline)
            {
                error.line = linenum - 1;
                mark_newline = 0;
            }
            else
                error.line = linenum;//同时判断前面的符号是否为换行符，如果是，则错误在前一行，因为在读的时候已经加了1
            error.type = 'k';
            print_error(error);
        }
        else
        {
            getsym();
        } //如果没错，那么还要继续往下读一个
        //print_code(); //输出标识符
        if (search_name_list(s_low, 0) < 0)
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'c';
            print_error(error);
        }
        name = find_name_list(s_low);
        if (name.type == 0 || name.type == 1)
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'j';
            print_error(error);
        }
        getsym();
        //print_code(); //输出=
        getsym();
        //print_code(); //输出标识符
        if (search_name_list(s_low, 0) < 0)
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'c';
            print_error(error);
        }
        name = find_name_list(s_low);
        if (name.type == 0 || name.type == 1)
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'j';
            print_error(error);
        }
        getsym();
        //print_code(); //输出+或-
        getsym();
        r = judge_stepsize();
        if (r < 0)
            return r;
        //print_code(); //输出)
        if (strcmp(s, ")") != 0)
        {
            struct error_info error;
            if (mark_newline)
            {
                error.line = linenum - 1;
                mark_newline = 0;
            }
            else
                error.line = linenum;
            error.type = 'l';
            print_error(error);
        }
        else
        {
            getsym();
        }
        r = judge_statement();
        if (r < 0)
            return r;
        if (r == 1)
        {
            mark = 1;
        }
    }
    //printf("<循环语句>\n");
    if (mark)
        return 1;
    return 0;
}

int judge_stepsize()
{
    int r;
    r = judge_unsigned_integer();
    if (r < 0)
        return r;
    //printf("<步长>\n");
    return 0;
}

int judge_case_statement()
{
    int r, type;
    if (symbol != SWITCHTK)
        return -1;
    //print_code(); //输出switch
    getsym();
    //print_code(); //输出(
    getsym();
    r = judge_expression();
    if (r < 0)
        return r;
    type = r;
    //print_code(); //输出)
    if (strcmp(s, ")") != 0)
    {
        struct error_info error;
        if (mark_newline)
        {
            error.line = linenum - 1;
            mark_newline = 0;
        }
        else
            error.line = linenum;
        error.type = 'l';
        print_error(error);
    }
    else
    {
        getsym();
    }
    //print_code(); //输出{
    getsym();
    r = judge_case_list(type);
    if (r < 0)
        return r;
    r = judge_default();
    if (r < 0)
        return r;
    //print_code(); //输出}
    getsym(); //这里应该暂时不需要进行关于错误p的额外的判断，因为如果没有错误必定当前是}，往下读就行了
    //printf("<情况语句>\n");
    return 0;
}

int judge_case_list(int type)
{
    int r;
    r = judge_case_son_statement(type);
    if (r < 0)
        return r;
    while (judge_case_son_statement(type) == 0)
    {
        //应该不需要干啥。。。
    }
    //printf("<情况表>\n");
    return 0;
}

int judge_case_son_statement(int type)
{
    int r;
    if (symbol != CASETK)
        return -1;
    //print_code(); //输出case
    getsym();
    r = judge_const();
    if (r < 0)
        return r;
    if ((r == 0 && type == 1) || (r == 1 && type == 0))
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'o';
        print_error(error);
    }
    //print_code();//输出:
    getsym();
    r = judge_statement();
    if (r < 0)
        return r;
    //printf("<情况子语句>\n");
    return 0;
}

int judge_default()
{
    int r;
    if (symbol != DEFAULTTK)
    {
        //TODO
        struct error_info error;
        error.line = linenum;
        error.type = 'p';
        print_error(error);
        return 1; //应该是不返回-1不然就判断错误不往下走了，注意该想法有可能有错误
    }
    //print_code(); //输出default
    getsym();
    //print_code(); //输出:
    getsym();
    r = judge_statement();
    if (r < 0)
        return r;
    //printf("<缺省>\n");
    return 0;
}

/*int judge_returnfunc_use()
{
    int r;
    char tmp[1000];
    if ((flag1 == 0 && symbol != IDENFR) || (flag1 == 1 && pre1 != IDENFR)) {
        return -1;
    }
    if (flag1)
    {
        if (symbol != LPARENT)
        {
            return -1;
        }
        //print_code_pre1();
        strcpy(tmp, content1);
        if (find_no_return_func(tmp) == 1)
        {
            return -1;
        }
        flag1 = 0;
    }
    else
    {
        //print_code();
        getsym();  //读入(
        if (symbol != LPARENT)
        {
            return -1;
        }
        strcpy(tmp, s_low);
        if (find_no_return_func(tmp) == 1)
        {
            return -1;
        }
    }
    if (search_name_list(tmp, 4) < 0)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'c';
        print_error(error);
    }
    //print_code(); //输出(
    getsym();
    r = judge_value_parameter_list();
    if (r < 0)
        return r;
    //print_code(); //输出)
    if (strcmp(s, ")") != 0)
    {
        struct error_info error;
        if (mark_newline)
        {
            error.line = linenum - 1;
            mark_newline = 0;
        }
        else
            error.line = linenum;
        error.type = 'l';
        print_error(error);
    }
    else
    {
        getsym();
    }
    //printf("<有返回值函数调用语句>\n");
    return 0;
}

int judge_noreturnfunc_use()
{
    int r;
    char tmp[1000];
    if ((flag1 == 0 && symbol != IDENFR) || (flag1 == 1 && pre1 != IDENFR)) {
        return -1;
    }
    if (flag1)
    {
        if (symbol != LPARENT)
        {
            return -1;
        }
        flag1 = 0;
        strcpy(tmp, content1);
    }
    else
    {

        strcpy(tmp, s_low);
    }
    if (find_no_return_func(tmp) == 0)
        return -1;
    //print_code(); //输出标识符
    if (search_name_list(s_low, 4) < 0)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'c';
        print_error(error);
    }
    getsym();
    //print_code(); //输出(
    getsym();
    r = judge_value_parameter_list();
    if (r < 0)
        return -1;
    //print_code(); //输出)
    if (strcmp(s, ")") != 0)
    {
        struct error_info error;
        if (mark_newline)
        {
            error.line = linenum - 1;
            mark_newline = 0;
        }
        else
            error.line = linenum;
        error.type = 'l';
        print_error(error);
    }
    else
    {
        getsym();
    }
    //printf("<无返回值函数调用语句>\n");
    return 0;
}*/

int judge_func_call()
{
    int r;
    char tmp[1000];
    if ((flag1 == 0 && symbol != IDENFR) || (flag1 == 1 && pre1 != IDENFR)) {
        return -1;
    }
    if (flag1)
    {
        //print_code_pre1();
        strcpy(tmp, content1);
        flag1 = 0;
    }
    else
    {
        //print_code();
        getsym();  //读入(
        strcpy(tmp, s_low);
    }
    if (search_name_list(tmp, 4) < 0)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'c';
        print_error(error);
    }
    if (symbol != LPARENT)
    {
        return -1;
    }
    //print_code(); //输出(
    getsym();
    r = judge_value_parameter_list(tmp);
    if (r < 0)
        return r;
    //print_code(); //输出)
    if (strcmp(s, ")") != 0)
    {
        struct error_info error;
        if (mark_newline)
        {
            error.line = linenum - 1;
            mark_newline = 0;
        }
        else
            error.line = linenum;
        error.type = 'l';
        print_error(error);
    }
    else
    {
        getsym();
    }
    struct func_info func = search_function(tmp);
    if (func.type == 1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
    return 0;
}

int judge_value_parameter_list(char* name)
{
    int r, sum = 0;
    struct func_info func = search_function(name);
    if (symbol == RPARENT || symbol == SEMICN)
    { //空的情况
        //printf("<值参数表>\n");
        if (sum != func.parameter_sum)
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'd';
            print_error(error);
        }
        return 0;
    }
    r = judge_expression();
    if (r < 0)
        return r;
    if (r != func.parameter_type[sum] && func.parameter_sum > 0)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'e';
        print_error(error);
    }
    sum++;
    while (symbol == COMMA)
    {
        //print_code();
        getsym();
        r = judge_expression();
        if (r < 0)
            return r;
        if (r != func.parameter_type[sum] && func.parameter_sum > 0)
        {
            struct error_info error;
            error.line = linenum;
            error.type = 'e';
            print_error(error);
        }
        sum++;
    }
    if (sum != func.parameter_sum)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'd';
        print_error(error);
    }
    //printf("<值参数表>\n");
    return 0;
}

int judge_statement_list()
{
    int r, mark = 0;
    while ((r = judge_statement()) >= 0)
    {
        if (r == 1)
        {
            mark = 1;
        }
        //应该不用干什么。。。
    }
    if (mark)
    {
        return 1;
    }
    //printf("<语句列>\n");
    return 0;
}

int judge_scanf()
{
    struct name_info name;
    if (symbol != SCANFTK)
        return -1;
    //print_code(); //输出scanf
    getsym();
    //print_code(); //输出(
    getsym();
    //print_code(); //输出标识符
    if (search_name_list(s_low, 0) < 0)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'c';
        print_error(error);
    }
    name = find_name_list(s_low);
    if (name.type == 0 || name.type == 1)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'j';
        print_error(error);
    }
    getsym();
    //print_code(); //输出)
    if (strcmp(s, ")") != 0)
    {
        struct error_info error;
        if (mark_newline)
        {
            error.line = linenum - 1;
            mark_newline = 0;
        }
        else
            error.line = linenum;
        error.type = 'l';
        print_error(error);
    }
    else
    {
        getsym();
    }
    //printf("<读语句>\n");
    return 0;
}

int judge_printf()
{
    int r;
    if (symbol != PRINTFTK)
        return -1;
    //print_code(); //输出printf
    getsym();
    //print_code(); //输出(
    getsym();
    if (judge_string() == 0)
    {
        //发现有字符串，判断有没有逗号来看有没有表达式
        if (symbol == COMMA)
        {
            //print_code(); //输出,
            getsym();
            r = judge_expression();
            if (r < 0)
                return r;
        }
    }
    else if (judge_expression() == 0)
    {

    }
    //print_code(); //输出)
    if (strcmp(s, ")") != 0)
    {
        struct error_info error;
        if (mark_newline)
        {
            error.line = linenum - 1;
            mark_newline = 0;
        }
        else
            error.line = linenum;
        error.type = 'l';
        print_error(error);
    }
    else
    {
        getsym();
    }
    //printf("<写语句>\n");
    return 0;
}

int judge_return_statement()
{
    int r, mark = 0, flag = 0;
    if (symbol != RETURNTK)
        return -1;
    //print_code(); //输出return
    getsym();
    if (symbol == LPARENT)
    {
        mark = 1;
        //print_code(); //输出(
        getsym();
        r = judge_expression();
        if (r < 0)
        {
            flag = 1;
        }
        //print_code(); //输出）
        if (strcmp(s, ")") != 0)
        {
            struct error_info error;
            if (mark_newline)
            {
                error.line = linenum - 1;
                mark_newline = 0;
            }
            else
                error.line = linenum;
            error.type = 'l';
            print_error(error);
        }
        else
        {
            getsym();
        }
    }
    if (mark == 1 && func_type == 0)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'g';
        print_error(error);
    }
    else if ((func_type == 1 && (mark == 0 || r == 1)) || (func_type == 2 && (mark == 0 || r == 0)))
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'h';
        print_error(error);
    }
    if (flag && func_type == 1)
    {
        struct error_info error;
        error.line = linenum;
        error.type = 'h';
        print_error(error);
    }
        
    //printf("<返回语句>\n");
    return 0;
}
int main()
{
    getsym();
    judge_program();
}