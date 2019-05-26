#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../headers/domain.h"

char *typeBaseStr(int typeBase) {
    switch (typeBase) {
        case TB_INT:
            return "TB_INT";
        case TB_CHAR:
            return "TB_CHAR";
        case TB_DOUBLE:
            return "TB_DOUBLE";
        case TB_STRUCT:
            return "TB_STRUCT";
        case TB_VOID:
            return "TB_VOID";
        default:
            return "TB_NOT_EXISTENT";
    }
}

char *classStr(int class) {
    switch(class) {
        case CLS_VAR:
            return "CLS_VAR";
        case CLS_FUNC:
            return "CLS_FUNC";
        case CLS_EXTFUNC:
            return "CLS_EXTFUNC";
        case CLS_STRUCT:
            return "CLS_STRUCT";
        default:
            return "CLS_NOT_EXISTENT";
    }
}

char *memStr(int mem) {
    switch(mem) {
        case MEM_ARG:
            return "MEM_ARG";
        case MEM_GLOBAL:
            return "MEM_GLOBAL";
        case MEM_LOCAL:
            return "MEM_LOCAL";
        default:
            return "MEM_NOT_EXISTENT";
    }
}

char *symbolToString(Symbol *s) {
    char *typeBase = typeBaseStr(s->type.typeBase);
    char *class = classStr(s->cls);
    char *mem = memStr(s->mem);
    char *buffer = (char *)malloc((strlen(s->name) + strlen(typeBase) + strlen(class) + strlen(mem) + 35 ) * sizeof(char));
    sprintf(buffer, "%s | DEPTH: %d | TB: %s | CLS: %s | MEM: %s\n", s->name, s->depth, typeBase, class, mem);
    return buffer;
}
