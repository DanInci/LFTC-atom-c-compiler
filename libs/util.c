#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../headers/util.h"

void err(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}

char *createString(const char *startCh, const char *endCh) {
    int n = endCh-startCh;
    char *s = (char *)malloc((n+1)*sizeof(char));
    strncpy(s, startCh, n);
    s[n] = '\0';
    return s;
}

char *getOutputFilePath(const char *inputFilePath) {
    char *ret = strchr(inputFilePath, '.');
    int baseFilePathLength = ret - inputFilePath;
    char *outputFilePath = (char *)malloc((baseFilePathLength + 4) * sizeof(char));
    strncpy(outputFilePath, inputFilePath, baseFilePathLength);
    strcpy(outputFilePath+baseFilePathLength, ".out");
    return outputFilePath;
}

char *convertToEscaped(const char *str) {
    char ch;
    int length = strlen(str);
    int newLength = 0;
    const char *q = str;
    char *s = (char *) malloc(2 * length * sizeof(char));
    while(q<str+length) {
        ch = *q;
        switch(ch) {
            case '\a':
                s[newLength] = '\\';
                newLength++;
                s[newLength] = 'a';
                break;
            case '\b':
                s[newLength] = '\\';
                newLength++;
                s[newLength] = 'b';
                break;
            case '\f':
                s[newLength] = '\\';
                newLength++;
                s[newLength] = 'f';
                break;
            case '\n':
                s[newLength] = '\\';
                newLength++;
                s[newLength] = 'n';
                break;
            case '\r':
                s[newLength] = '\\';
                newLength++;
                s[newLength] = 'r';
                break;
            case '\t':
                s[newLength] = '\\';
                newLength++;
                s[newLength] = 't';
                break;
            case '\v':
                s[newLength] = '\\';
                newLength++;
                s[newLength] = 'v';
                break;
            case '\'':
                s[newLength] = '\\';
                newLength++;
                s[newLength] = '\'';
                break;
            case '"':
                s[newLength] = '\\';
                newLength++;
                s[newLength] = '"';
                break;
            case '\\':
                s[newLength] = '\\';
                newLength++;
                s[newLength] = '\\';
                break;
            case '\0':
                s[newLength] = '\\';
                newLength++;
                s[newLength] = '0';
                break;
            default:
                s[newLength] = ch;
                break;
        }
        newLength++;
        q++;
    }
    s[newLength] = '\0';
    s = (char *)realloc(s, newLength*sizeof(char));
    return s;
}

char *convertFromEscaped(const char *startCh, const char *endCh) {
    int length = endCh-startCh;
    int newLength=0;
    char ch;
    const char *q = startCh;
    char *s = (char *) malloc((length+1)*sizeof(char));
    while(q<=endCh) {
        ch = *q;
        if(ch == '\\') {
            q++;
            switch(*q) {
                case 'a':
                    ch = '\a';
                    break;
                case 'b':
                    ch = '\b';
                    break;
                case 'f':
                    ch = '\f';
                    break;
                case 'n':
                    ch = '\n';
                    break;
                case 'r':
                    ch = '\r';
                    break;
                case 't':
                    ch = '\t';
                    break;
                case 'v':
                    ch = '\v';
                    break;
                case '\'':
                    ch = '\'';
                    break;
                case '"':
                    ch = '"';
                    break;
                case '?':
                    ch = '\?';
                    break;
                case '\\':
                    ch = '\\';
                    break;
                case '0':
                    ch = '\0';
                    break;
                default:
                    err("Unknown escape sequence");
                    break;
            }
        }
        s[newLength] = ch;
        newLength++;
        q++;
    }
    s = (char *)realloc(s, newLength*sizeof(char));
    s[newLength] = '\0';
    return s;
}