#include <stdio.h>
#include <ctype.h>
#include <string.h>
void print_code();
void print_code_pre1();
void print_code_pre2();
//void Code_map(Code symbol);
void deal_token(char c);
void getsym();
int find_return_func();
int find_no_return_func();
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
int judge_parameter_list();
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
int judge_case_list();
int judge_case_son_statement();
int judge_default();
int judge_returnfunc_use();
int judge_noreturnfunc_use();
int judge_value_parameter_list();
int judge_statement_list();
int judge_scanf();
int judge_printf();
int judge_return_statement();

FILE* in = freopen("testfile.txt", "r", stdin);
FILE* out = freopen("output.txt", "w", stdout);
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

int judge_string()
{
    if (symbol != STRCON)
        return -1;
    print_code();
    getsym();
    printf("<字符串>\n");
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
    printf("<程序>\n");
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
        print_code();
        getsym();
        r = judge_const_define();
        if (r < 0)
            return r;
        print_code();
        getsym();
    }
    printf("<常量说明>\n");
    return 0;
}

int judge_const_define()
{
    int r;
    if (symbol == INTTK)
    {
        print_code();
        getsym();  //读标识符
        print_code();
        getsym();  //读等号
        print_code();
        getsym();  //读整数
        r = judge_integer();
        if (r < 0)
            return r;
        while (symbol == COMMA) //不出意外的话，逗号在前面读了
        {
            print_code();
            getsym(); //读标识符
            print_code();
            getsym(); //读等号
            print_code();
            getsym(); //读整数
            r = judge_integer();
            if (r < 0)
                return r;
        }
    }
    else if (symbol == CHARTK) //???可否有else
    {
        print_code();
        getsym();  //读标识符
        print_code();
        getsym();  //读等号
        print_code();
        getsym();  //读字符
        print_code();
        getsym();  //读逗号，因为并没有处理逗号的函数
        while (symbol == COMMA) //不出意外的话，逗号在前面读了
        {
            print_code();
            getsym();  //读标识符
            print_code();
            getsym();  //读等号
            print_code();
            getsym();  //读字符
            print_code();
            getsym();  //读逗号，因为并没有处理逗号的函数，若不是逗号，那么读sym并跳出
        }
    }
    printf("<常量定义>\n");
    return 0;
}

int judge_unsigned_integer()
{
    if (symbol != INTCON) {
        return -1;
    }
    print_code(); //输出数
    getsym(); //读下一个
    printf("<无符号整数>\n");
    return 0;
}

int judge_integer()
{
    int r;
    if (symbol == PLUS || symbol == MINU) //???是否需要判断是不是char常量
    {
        print_code(); //输出符号
        getsym(); //读数
    }
    r = judge_unsigned_integer();
    if (r < 0)
        return r;
    printf("<整数>\n");
    return 0;
}

int judge_head()
{
    if ((flag1 == 1 && pre1 != INTTK && pre1 != CHARTK)
        || (flag1 == 0 && symbol != INTTK && symbol != CHARTK)) {
        return -1;
    }
    if (flag1 == 1) {
        flag1 = 0;
        print_code_pre1();
    }
    else {
        print_code();
        getsym();
    }
    if (flag2 == 1) {
        flag2 = 0;
        strcpy(has_return_func[has_return_func_sum++], content2);
        print_code_pre2(); //输出标识符
    }
    else {
        strcpy(has_return_func[has_return_func_sum++], s);
        print_code(); //输出标识符
        getsym();
    }
    printf("<声明头部>\n");
    return 0;
}

int judge_const()
{
    if (judge_integer() == 0)
    {
        printf("<常量>\n");
        return 0;
    }
    else if (s[0] == '+' || s[0] == '-' || s[0] == '*' || s[0] == '/' || (s[0] >= '0' && s[0] <= '9')
        || (s[0] >= 'a' && s[0] <= 'z') || (s[0] >= 'A' && s[0] <= 'Z') || s[0] == '_')
    {
        print_code(); //输出这个字符
        getsym();
        printf("<常量>\n");
        return 0;
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
        strcpy(content1, s);
        flag1 = 1;
        getsym();
        pre2 = symbol;
        strcpy(content2, s);
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
        print_code();
        getsym();
    }
    if (sum == 0)
        return 0; //如果为0，说明没有变量说明
    printf("<变量说明>\n");
    return 0;
}

int judge_var_define()
{
    int r;
    int flag = 0;
    print_code_pre1(); //输出类型标识符
    flag1 = 0;
    print_code_pre2(); //输出标识符
    flag2 = 0;
    if (symbol == LBRACK)
    {
        print_code(); //输出[
        getsym();
        r = judge_unsigned_integer();
        if (r < 0)
            return r;
        print_code(); //输出]
        getsym();
        if (symbol == LBRACK)
        {
            print_code(); //输出[
            getsym();
            r = judge_unsigned_integer();
            if (r < 0)
                return r;
            print_code(); //输出]
            getsym();
            if (symbol == ASSIGN)
            {
                flag = 1;
                print_code(); //输出=
                getsym();
                print_code(); //输出{
                getsym();
                print_code(); //输出{
                getsym(); 
                r = judge_const();
                if (r < 0)
                    return r;
                while (symbol == COMMA)
                {
                    print_code(); //输出,
                    getsym();
                    r = judge_const();
                    if (r < 0)
                        return r;
                }
                print_code(); //输出}
                getsym();
                while (symbol == COMMA)
                {
                    print_code(); //输出,
                    getsym();
                    print_code(); //输出{
                    getsym();
                    r = judge_const();
                    if (r < 0)
                        return r;
                    while (symbol == COMMA)
                    {
                        print_code(); //输出,
                        getsym();
                        r = judge_const();
                        if (r < 0)
                            return r;
                    }
                    print_code(); //输出}
                    getsym();
                }
                print_code(); //输出}
                getsym();
            }
        }
        else if(symbol == ASSIGN)
        {
            flag = 1;
            print_code(); //输出=
            getsym();
            print_code(); //输出{
            getsym();
            r = judge_const();
            if (r < 0)
                return r;
            while (symbol == COMMA)
            {
                print_code(); //输出,
                getsym();
                r = judge_const();
                if (r < 0)
                    return r;
            }
            print_code(); //输出}
            getsym();
        }
    }
    else if (symbol == ASSIGN)
    {
        flag = 1;
        print_code(); //输出=
        getsym();
        r = judge_const();
        if (r < 0)
            return r;
    }
    while (symbol == COMMA)
    {
        print_code(); //输出,
        getsym();
        print_code(); //输出标识符
        getsym();
        if (symbol == LBRACK)
        {
            print_code(); //输出[
            getsym();
            r = judge_unsigned_integer();
            if (r < 0)
                return r;
            print_code(); //输出]
            getsym();
            if (symbol == LBRACK)
            {
                print_code(); //输出[
                getsym();
                r = judge_unsigned_integer();
                if (r < 0)
                    return r;
                print_code(); //输出]
                getsym();
                if (symbol == ASSIGN)
                {
                    flag = 1;
                    print_code(); //输出=
                    getsym();
                    print_code(); //输出{
                    getsym();
                    print_code(); //输出{
                    getsym();
                    r = judge_const();
                    if (r < 0)
                        return r;
                    while (symbol == COMMA)
                    {
                        print_code(); //输出,
                        getsym();
                        r = judge_const();
                        if (r < 0)
                            return r;
                    }
                    print_code(); //输出}
                    getsym();
                    while (symbol == COMMA)
                    {
                        print_code(); //输出,
                        getsym();
                        print_code(); //输出{
                        getsym();
                        r = judge_const();
                        if (r < 0)
                            return r;
                        while (symbol == COMMA)
                        {
                            print_code(); //输出,
                            getsym();
                            r = judge_const();
                            if (r < 0)
                                return r;
                        }
                        print_code(); //输出}
                        getsym();
                    }
                    print_code(); //输出}
                    getsym();
                }
            }
            else if (symbol == ASSIGN)
            {
                flag = 1;
                print_code(); //输出=
                getsym();
                print_code(); //输出{
                getsym();
                r = judge_const();
                if (r < 0)
                    return r;
                while (symbol == COMMA)
                {
                    print_code(); //输出,
                    getsym();
                    r = judge_const();
                    if (r < 0)
                        return r;
                }
                print_code(); //输出}
                getsym();
            }
        }
        else if (symbol == ASSIGN)
        {
            flag = 1;
            print_code(); //输出=
            getsym();
            r = judge_const();
            if (r < 0)
                return r;
        }
        //???
    }
    if (flag)
        printf("<变量定义及初始化>\n");
    else
        printf("<变量定义无初始化>\n");
    printf("<变量定义>\n");
    return 0;
}

int judge_function_has_return()
{
    int r;
    r = judge_head();
    if (r < 0)
        return r;
    print_code(); //输出(
    getsym();
    r = judge_parameter_list();
    if (r < 0)
        return r;
    print_code(); //输出)
    getsym();
    print_code(); //输出{
    getsym();
    r = judge_mix_statement();
    if (r < 0)
        return r;
    print_code(); //输出}
    getsym();
    printf("<有返回值函数定义>\n");
    return 0;
}

int judge_function_no_return()
{
    int r;
    pre1 = symbol;
    strcpy(content1, s);
    flag1 = 1;
    getsym();
    if (symbol == MAINTK)
        return -1;
    print_code_pre1(); //输出void
    flag1 = 0;
    strcpy(no_return_func[no_return_func_sum++], s);
    print_code(); //输出标识符
    getsym();
    print_code(); //输出(
    getsym();
    r = judge_parameter_list();
    if (r < 0)
        return r;
    print_code(); //输出)
    getsym();
    print_code(); //输出{
    getsym();
    r = judge_mix_statement();
    if (r < 0)
        return r;
    print_code(); //输出}
    getsym();
    printf("<无返回值函数定义>\n");
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
    printf("<复合语句>\n");
    return 0;
}

int judge_parameter_list()
{
    if (symbol == RPARENT) //空的情况
    {
        printf("<参数表>\n");
        return 0;
    }
    print_code(); //输出int或char
    getsym();
    print_code(); //输出标识符
    getsym();
    while (symbol == COMMA)
    {
        print_code(); //输出,
        getsym();
        print_code(); //输出int或char
        getsym();
        print_code(); //输出标识符
        getsym();
    }
    printf("<参数表>\n");
    return 0;
}

int judge_main_function()
{
    int r;
    if (flag1 == 1) 
    {
        flag1 = 0;
        print_code_pre1();
    }
    else 
    {
        print_code();
        getsym();
    }//两种情况决定输出void
    print_code(); //输出main
    getsym();
    print_code(); //输出(
    getsym();
    print_code(); //输出)
    getsym();
    print_code(); //输出{
    getsym();
    r = judge_mix_statement();
    if (r < 0)
        return r;
    print_code(); //输出}
    //到最后啦，不用读了
    printf("<主函数>\n");
    return 0;
}

int judge_expression()
{
    int r;
    if (symbol == PLUS || symbol == MINU)
    {
        print_code();
        getsym();
    }
    r = judge_item();
    if (r < 0)
        return r;
    while (symbol == PLUS || symbol == MINU)
    {
        print_code();
        getsym();
        r = judge_item();
        if (r < 0)
            return r;
    }
    printf("<表达式>\n");
    return 0;
}

int judge_item()
{
    int r;
    r = judge_factor();
    if (r < 0)
        return r;
    while (symbol == MULT || symbol == DIV)
    {
        print_code();
        getsym();
        r = judge_factor();
        if (r < 0)
            return r;
    }
    printf("<项>\n");
    return 0;
}

int judge_factor()
{
    int r;
    if (judge_returnfunc_use() == 0)
    {
        //应该不用干什么
    }
    else if (symbol == IDENFR)
    {
        print_code(); //输出标识符
        getsym();
        while (symbol == LBRACK)
        {
            print_code(); //输出[
            getsym();
            r = judge_expression();
            if (r < 0)
                return r;
            print_code(); //输出]
            getsym();
        }
    }
    else if (symbol == LPARENT)
    {
        print_code(); //输出(
        getsym();
        r = judge_expression();
        if (r < 0)
            return r;
        print_code(); //输出)
        getsym();
    }
    else if (symbol == CHARCON)
    {
        print_code(); //输出字符
        getsym();
    }
    else if (judge_integer() == 0)
    {
        //应该不用干什么
    }
    printf("<因子>\n");
    return 0;
}

int judge_statement()
{
    int r;
    if (symbol == SEMICN)
    { //空的情况
        print_code(); //输出;
        getsym();
    }
    else if (judge_circulation_statement() == 0)
    {
        //应该不用干什么
    }
    else if (judge_condition_statement() == 0)
    {
        //应该不用干什么
    }
    else if (judge_return_statement() == 0)
    {
        print_code(); //输出;
        getsym();
    }
    else if (judge_returnfunc_use() == 0)
    {
        print_code(); //输出;
        getsym();
    }
    else if (judge_noreturnfunc_use() == 0)
    {
        print_code(); //输出;
        getsym();
    }
    else if (judge_value_statement() == 0)
    {
        print_code(); //输出;
        getsym();
    }
    else if (judge_scanf() == 0)
    {
        print_code(); //输出;
        getsym();
    }
    else if (judge_printf() == 0)
    {
        print_code(); //输出;
        getsym();
    }
    else if (judge_case_statement() == 0)
    {
        //应该不用干什么
    }
    else if (symbol == LBRACE)
    {
        print_code(); //输出{
        getsym();
        r = judge_statement_list();
        if (r < 0)
            return r;
        print_code(); //输出}
        getsym();
    }
    else
        return -1;
    printf("<语句>\n");
    return 0;
}

int judge_value_statement()
{
    int r;
    if (symbol != IDENFR)
    {
        return -1;
    }
    print_code(); //输出标识符
    getsym();
    while (symbol == LBRACK)
    {
        print_code(); //输出[
        getsym();
        r = judge_expression();
        if (r < 0)
            return r;
        print_code(); //输出]
        getsym();
    }
    print_code(); //输出=
    getsym();
    r = judge_expression();
    if (r < 0)
        return r;
    printf("<赋值语句>\n");
    return 0;
}

int judge_condition_statement()
{
    int r;
    if (symbol != IFTK)
        return -1;
    print_code(); //输出if
    getsym();
    print_code(); //输出(
    getsym();
    r = judge_condition();
    if (r < 0)
        return r;
    print_code(); //输出)
    getsym();
    r = judge_statement();
    if (r < 0)
        return r;
    if (symbol == ELSETK)
    {
        print_code();//输出else
        getsym();
        r = judge_statement();
        if (r < 0)
            return r;
    }
    printf("<条件语句>\n");
    return 0;
}

int judge_condition()
{
    int r;
    r = judge_expression();
    if (r < 0)
        return r;
    print_code(); //输出关系运算符
    getsym();
    r = judge_expression();
    if (r < 0)
        return r;
    printf("<条件>\n");
    return 0;
}

int judge_circulation_statement()
{
    int r;
    if (symbol != WHILETK && symbol != FORTK)
        return -1;
    if (symbol == WHILETK)
    {
        print_code();//输出while
        getsym();
        print_code(); //输出(
        getsym();
        r = judge_condition();
        if (r < 0)
            return r;
        print_code(); //输出)
        getsym();
        r = judge_statement();
        if (r < 0)
            return r;
    }
    else if (symbol == FORTK)
    {
        print_code(); //输出for
        getsym();
        print_code(); //输出(
        getsym();
        print_code(); //输出标识符
        getsym();
        print_code(); //输出=
        getsym();
        r = judge_expression();
        if (r < 0)
            return r;
        print_code(); //输出;
        getsym();
        r = judge_condition();
        if (r < 0)
            return r;
        print_code(); //输出;
        getsym();
        print_code(); //输出标识符
        getsym();
        print_code(); //输出=
        getsym();
        print_code(); //输出标识符
        getsym();
        print_code(); //输出+或-
        getsym();
        r = judge_stepsize();
        if (r < 0)
            return r;
        print_code(); //输出)
        getsym();
        r = judge_statement();
        if (r < 0)
            return r;
    }
    printf("<循环语句>\n");
    return 0;
}

int judge_stepsize()
{
    int r;
    r = judge_unsigned_integer();
    if (r < 0)
        return r;
    printf("<步长>\n");
    return 0;
}

int judge_case_statement()
{
    int r;
    if (symbol != SWITCHTK)
        return -1;
    print_code(); //输出switch
    getsym();
    print_code(); //输出(
    getsym();
    r = judge_expression();
    if (r < 0)
        return r;
    print_code(); //输出)
    getsym(); 
    print_code(); //输出{
    getsym();
    r = judge_case_list();
    if (r < 0)
        return r;
    r = judge_default();
    if (r < 0)
        return r;
    print_code(); //输出}
    getsym();
    printf("<情况语句>\n");
    return 0;
}

int judge_case_list()
{
    int r;
    r = judge_case_son_statement();
    if (r < 0)
        return r;
    while (judge_case_son_statement() == 0)
    {
        //应该不需要干啥。。。
    }
    printf("<情况表>\n");
    return 0;
}

int judge_case_son_statement()
{
    int r;
    if (symbol != CASETK)
        return -1;
    print_code(); //输出case
    getsym();
    r = judge_const();
    if (r < 0)
        return r;
    print_code();//输出:
    getsym();
    r = judge_statement();
    if (r < 0)
        return r;
    printf("<情况子语句>\n");
    return 0;
}

int judge_default()
{
    int r;
    if (symbol != DEFAULTTK)
        return -1;
    print_code(); //输出default
    getsym();
    print_code(); //输出:
    getsym();
    r = judge_statement();
    if (r < 0)
        return r;
    printf("<缺省>\n");
    return 0;
}

int judge_returnfunc_use()
{
    int r;
    char tmp[1000];
    if (symbol != IDENFR)
        return -1;
    strcpy(tmp, s);
    if (find_return_func(tmp) == 0)
        return -1;
    print_code(); //输出标识符
    getsym();
    print_code(); //输出(
    getsym();
    r = judge_value_parameter_list();
    if (r < 0)
        return r;
    print_code(); //输出)
    getsym();
    printf("<有返回值函数调用语句>\n");
    return 0;
}

int judge_noreturnfunc_use()
{
    int r;
    char tmp[1000];
    if (symbol != IDENFR)
        return -1;
    strcpy(tmp, s);
    if (find_no_return_func(tmp) == 0)
        return -1;
    print_code(); //输出标识符
    getsym();
    print_code(); //输出(
    getsym();
    r = judge_value_parameter_list();
    if (r < 0)
        return -1;
    print_code(); //输出)
    getsym();
    printf("<无返回值函数调用语句>\n");
    return 0;
}

int judge_value_parameter_list()
{
    int r;
    if (symbol == RPARENT) 
    { //空的情况
        printf("<值参数表>\n");
        return 0;
    }
    r = judge_expression();
    if (r < 0)
        return r;
    while (symbol == COMMA)
    {
        print_code();
        getsym();
        r = judge_expression();
        if (r < 0)
            return r;
    }
    printf("<值参数表>\n");
    return 0;
}

int judge_statement_list()
{
    while (judge_statement() == 0)
    {
        //应该不用干什么。。。
    }
    printf("<语句列>\n");
    return 0;
}

int judge_scanf()
{
    if (symbol != SCANFTK)
        return -1;
    print_code(); //输出scanf
    getsym();
    print_code(); //输出(
    getsym();
    print_code(); //输出标识符
    getsym();
    print_code(); //输出)
    getsym();
    printf("<读语句>\n");
    return 0;
}

int judge_printf()
{
    int r;
    if (symbol != PRINTFTK)
        return -1;
    print_code(); //输出printf
    getsym();
    print_code(); //输出(
    getsym();
    if (judge_string() == 0)
    {
        //发现有字符串，判断有没有逗号来看有没有表达式
        if (symbol == COMMA)
        {
            print_code();
            getsym();
            r = judge_expression();
            if (r < 0)
                return r;
        }
    }
    else if (judge_expression() == 0)
    {

    }
    print_code();
    getsym();
    printf("<写语句>\n");
    return 0;
}

int judge_return_statement()
{
    int r;
    if (symbol != RETURNTK)
        return -1;
    print_code(); //输出return
    getsym();
    if (symbol == LPARENT)
    {
        print_code(); //输出(
        getsym();
        r = judge_expression();
        if (r < 0)
            return r;
        print_code(); //输出）
        getsym();
    }
    printf("<返回语句>\n");
    return 0;
}
int main()
{
    getsym();
    judge_program();
}