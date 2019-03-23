#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../headers/token.h"

char * toString(Token *t) {
    char *str;
    switch(t->code) {
            case END:
                return "END";
            case BREAK:
                return "BREAK";
            case CHAR:
                return "CHAR";
            case DOUBLE:
                return "DOUBLE";
            case ELSE:
                return "ELSE";
            case FOR:
                return "FOR";
            case IF:
                return "IF";
            case INT:
                return "INT";
            case RETURN:
                return "RETURN";
            case STRUCT:
                return "STRUCT";
            case VOID:
                return "VOID";
            case WHILE:
                return "WHILE";
            case ID: 
                str = (char *) malloc((4 + strlen(t->text)) * sizeof(char));
                sprintf(str, "ID(%s)", t->text);
                return str;
            case CT_CHAR: 
                str = (char *) malloc(10 * sizeof(char));
                sprintf(str, "CT_CHAR(%c)", (char) t->i);
                return str;
            case CT_STRING:
                str = (char *) malloc((11 + strlen(t->text)) * sizeof(char));
                sprintf(str, "CT_STRING(%s)", t->text);
                return str;
            case CT_INT:
                str = (char *) malloc((8 + 25) * sizeof(char));
                sprintf(str, "CT_INT(%ld)", t->i);
                return str;
            case CT_REAL:
                str = (char *) malloc((9 + 25) * sizeof(char));
                sprintf(str, "CT_REAL(%f)",t->r);
                return str;
            case DIV:
                return "DIV";
            case ADD:
                return "ADD";
            case SUB:
                return "SUB";
            case MUL:
                return "MUL";
            case DOT:
                return "DOT";
            case AND:
                return "AND";
            case OR:
                return "OR";
            case NOTEQ:
                return "NOTEQ";
            case EQUAL:
                return "EQUAL";
            case ASSIGN:
                return "ASSIGN";
            case LESSEQ:
                return "LESSEQ";
            case LESS:
                return "LESS";
            case GREATEREQ:
                return "GREATEREQ";
            case GREATER:
                return "GREATER";
            case COMMA:
                return "COMMA";
            case SEMICOLON:
                return "SEMICOLON";
            case LPAR:
                return "LPAR";
            case RPAR:
                return "RPAR";
            case LBRACKET:
                return "LBRACKET";
            case RBRACKET:
                return "RBRACKET";
            case LACC:
                return "LACC";
            case RACC:
                return "RACC";
            default:
                return "Unknown token";
    }
}

void tkerr(const Token *tk, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error in line %d: ", tk->line);
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}

void prettyPrintList(Token *start) {
    Token *q = start;
    while(q) {
        printf("%s\n", toString(q));
    }
}