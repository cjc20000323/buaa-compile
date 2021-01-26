#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
using namespace std;
FILE* in = freopen("testfile.txt", "r", stdin);
FILE* out = freopen("output.txt", "w", stdout);
enum Code //类别码
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
void Code_map(Code symbol) //映射函数
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
void deal_token(char c)
{
    if (isalpha(c) || c == '_')
    {
        char s[1000];
        char s_low[1000];
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
            Code symbol = CONSTTK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "int") == 0) {
            Code symbol = INTTK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "char") == 0) {
            Code symbol = CHARTK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "void") == 0) {
            Code symbol = VOIDTK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "main") == 0) {
            Code symbol = MAINTK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "if") == 0) {
            Code symbol = IFTK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "else") == 0) {
            Code symbol = ELSETK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "switch") == 0) {
            Code symbol = SWITCHTK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "case") == 0) {
            Code symbol = CASETK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "default") == 0) {
            Code symbol = DEFAULTTK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "while") == 0) {
            Code symbol = WHILETK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "for") == 0) {
            Code symbol = FORTK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "scanf") == 0) {
            Code symbol = SCANFTK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "printf") == 0) {
            Code symbol = PRINTFTK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else if (strcmp(s_low, "return") == 0) {
            Code symbol = RETURNTK;
            Code_map(symbol);
            printf(" %s\n", s);
        }
        else {
            Code symbol = IDENFR;
            Code_map(symbol);
            printf(" %s\n", s);
        }
    }
    else if (isdigit(c)) {
        char s[1000];
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
        Code symbol = INTCON;
        Code_map(symbol);
        printf(" %s\n", s);
    }
    else if (c == '\'')
    {
        char s[1000];
        int i = 0;
        c = getchar();
        s[i++] = c;
        s[i] = '\0';
        c = getchar();
        Code symbol = CHARCON;
        Code_map(symbol);
        printf(" %s\n", s);
    }
    else if (c == '"')
    {
        char s[1000];
        int i = 0;
        while ((c = getchar())) {
            if (c == '"') {
                break;
            }
            s[i++] = c;
        }
        s[i] = '\0';
        Code symbol = STRCON;
        Code_map(symbol);
        printf(" %s\n", s);
    }
    else if (c == '+') {
        Code symbol = PLUS;
        Code_map(symbol);
        printf(" +\n");
    }
    else if (c == '-') {
        Code symbol = MINU;
        Code_map(symbol);
        printf(" -\n");
    }
    else if (c == '*') {
        Code symbol = MULT;
        Code_map(symbol);
        printf(" *\n");
    }
    else if (c == '/') {
        Code symbol = DIV;
        Code_map(symbol);
        printf(" /\n");
    }
    else if (c == '<') {
        c = getchar();
        if (c == '=') {
            Code symbol = LEQ;
            Code_map(symbol);
            printf(" <=\n");
        }
        else {
            fseek(in, -1, SEEK_CUR);
            Code symbol = LSS;
            Code_map(symbol);
            printf(" <\n");
        }
    }
    else if (c == '>') {
        c = getchar();
        if (c == '=') {
            Code symbol = GEQ;
            Code_map(symbol);
            printf(" >=\n");
        }
        else {
            fseek(in, -1, SEEK_CUR);
            Code symbol = GRE;
            Code_map(symbol);
            printf(" >\n");
        }
    }
    else if (c == '=') {
        c = getchar();
        if (c == '=') {
            Code symbol = EQL;
            Code_map(symbol);
            printf(" ==\n");
        }
        else {
            fseek(in, -1, SEEK_CUR);
            Code symbol = ASSIGN;
            Code_map(symbol);
            printf(" =\n");
        }
    }
    else if (c == '!') {
        c = getchar();
        if (c == '=') {
            Code symbol = NEQ;
            Code_map(symbol);
            printf(" !=\n");
        }
    }
    else if (c == ':') {
        Code symbol = COLON;
        Code_map(symbol);
        printf(" :\n");
    }
    else if (c == ';') {
        Code symbol = SEMICN;
        Code_map(symbol);
        printf(" ;\n");
    }
    else if (c == ',') {
        Code symbol = COMMA;
        Code_map(symbol);
        printf(" ,\n");
    }
    else if (c == '(') {
        Code symbol = LPARENT;
        Code_map(symbol);
        printf(" (\n");
    }
    else if (c == ')') {
        Code symbol = RPARENT;
        Code_map(symbol);
        printf(" )\n");
    }
    else if (c == '[') {
        Code symbol = LBRACK;
        Code_map(symbol);
        printf(" [\n");
    }
    else if (c == ']') {
        Code symbol = RBRACK;
        Code_map(symbol);
        printf(" ]\n");
    }
    else if (c == '{') {
        Code symbol = LBRACE;
        Code_map(symbol);
        printf(" {\n");
    }
    else if (c == '}') {
        Code symbol = RBRACE;
        Code_map(symbol);
        printf(" }\n");
    }
}
int main()
{
    char c = getchar();
    while (1)
    {
        while (isspace(c))
            c = getchar();
        if (c == EOF)
            break;
        deal_token(c);
        c = getchar();
    }
}