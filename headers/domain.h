#ifndef DOMAIN
#define DOMAIN

#include "token.h"

enum { TB_INT, TB_DOUBLE, TB_CHAR, TB_STRUCT, TB_VOID };
enum { CLS_VAR, CLS_FUNC, CLS_EXTFUNC, CLS_STRUCT };
enum { MEM_GLOBAL, MEM_ARG, MEM_LOCAL };

struct _Symbol;
typedef struct _Symbol Symbol;

typedef struct {
    int typeBase;
    Symbol *s;
    int nElements;
} Type;

typedef struct _Symbols {
    Symbol **begin;
    Symbol **end;
    Symbol **after;
} Symbols;

typedef struct _Symbol {
    const char *name;
    int cls;
    int mem;
    Type type;
    int depth;
    union {
        Symbols args;
        Symbols members;
    };
} Symbol;

char *symbolToString(Symbol *s);

#endif