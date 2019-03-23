#ifndef TOKEN
#define TOKEN

enum {
    END, ID, BREAK, CHAR, DOUBLE, ELSE, FOR, IF, INT, RETURN, STRUCT, VOID, WHILE,
    CT_INT, CT_REAL, CT_CHAR, CT_STRING, COMMA, SEMICOLON, LPAR, RPAR, LBRACKET, RBRACKET,
    LACC, RACC, ADD, SUB, MUL, DIV, DOT, AND, OR, NOT, ASSIGN, EQUAL, NOTEQ, LESS, LESSEQ, 
    GREATER, GREATEREQ
};

typedef struct _Token {
    int code;
    union {
        char *text;
        long int i;
        double r;
    };
    int line;
    struct _Token *next;
} Token;

char * toString(Token *t);

void tkerr(const Token *tk, const char *fmt, ...);

void prettyPrintList(Token *start);

#endif