#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../headers/util.h"
#include "../headers/lexical.h"

#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL) err("not enough memory");
#define ESCAPED_CHARS_COMPARISON(c) c == 'a' || c == 'b' || c == 'f' || c == 'n' || c == 'r' || c == 't' || c == 'v' || c == '\'' || c == '?' || c == '"' || c == '\\' || c == '0'
#define HEXA_CHARS_COMPARISON(c) isnumber(c) || (c > 'a' && c < 'f') || (c > 'A' && c < 'F')

Token *tokens, *lastToken;
char *pCrtCh;
int currentLine = 1;

Token *addTk(int code) {
    Token *tk;
    SAFEALLOC(tk, Token)
    tk->code = code;
    tk->line = currentLine;
    tk->next = NULL;
    if(lastToken) {
        lastToken->next = tk;
    } else {
        tokens = tk;
    }
    lastToken = tk;
    return tk;
}

int getNextToken() {
    int state=0, nCh, base=0;
    char ch;
    const char *pStartCh;
    Token *tk;
    while(1) {
        ch=*pCrtCh;
        switch(state) {
            case 0:
                if (ch == '\0') {
                    addTk(END);
                    return END;
                }
                else if(ch == '\n') {
                    currentLine++;
                    pCrtCh++;
                }
                else if(ch == ' ' || ch == '\r' || ch == '\t') {
                    pCrtCh++;
                }
                else if(isalpha(ch) || ch == '_') {
                    pStartCh=pCrtCh;
                    pCrtCh++;
                    state=1;
                }
                else if(isnumber(ch)) {
                    pStartCh=pCrtCh;
                    pCrtCh++;
                    if(ch == '0') {
                        state=53;
                    }
                    else {
                        state=51;
                    }
                }
                else if(ch == '\'') {
                    pCrtCh++;
                    state=3;
                }
                else if(ch == '"') {
                    pCrtCh++;
                    pStartCh=pCrtCh;
                    state=7;
                }
                else if(ch == '/') {
                    pCrtCh++;
                    state=11;
                }
                else if(ch == '+') {
                    pCrtCh++;
                    state=16;
                }
                else if(ch == '-') {
                    pCrtCh++;
                    state=17;
                }
                else if(ch == '*') {
                    pCrtCh++;
                    state=18;
                }
                else if(ch == '.') {
                    pCrtCh++;
                    state=19;
                }
                else if(ch == '&') {
                    pCrtCh++;
                    state=20;
                }
                else if(ch == '|') {
                    pCrtCh++;
                    state=22;
                }
                else if(ch == '!') {
                    pCrtCh++;
                    state=24;
                }
                else if(ch == '=') {
                    pCrtCh++;
                    state=27;
                }
                else if(ch == '<') {
                    pCrtCh++;
                    state=30;
                }
                else if(ch == '>') {
                    pCrtCh++;
                    state=33;
                }
                else if(ch == ',') {
                    pCrtCh++;
                    state=36;
                }
                else if(ch == ';') {
                    pCrtCh++;
                    state=37;
                }
                else if(ch == '(') {
                    pCrtCh++;
                    state=38;
                }
                else if(ch == ')') {
                    pCrtCh++;
                    state=39;
                }
                else if(ch == '[') {
                    pCrtCh++;
                    state=40;
                }
                else if(ch == ']') {
                    pCrtCh++;
                    state=41;
                }
                else if(ch == '{') {
                    pCrtCh++;
                    state=42;
                }
                else if(ch == '}') {
                    pCrtCh++;
                    state=43;
                }
                else {
                    tkerr(addTk(END), "Caracter invalid: %c", ch);
                }
                break;
            case 1:
                if(isalnum(ch) || ch == '_') {
                    pCrtCh++;
                } else {
                    state=2;
                }
                break;
            case 2: 
                nCh=pCrtCh-pStartCh;
                if(nCh==5 && !memcmp(pStartCh, "break", 5)) tk=addTk(BREAK);
                else if(nCh==4 && !memcmp(pStartCh, "char", 4)) tk=addTk(CHAR);
                else if(nCh==6 && !memcmp(pStartCh, "double", 6)) tk=addTk(DOUBLE);
                else if(nCh==4 && !memcmp(pStartCh, "else", 4)) tk=addTk(ELSE);
                else if(nCh==3 && !memcmp(pStartCh, "for", 3)) tk=addTk(FOR);
                else if(nCh==2 && !memcmp(pStartCh, "if", 2)) tk=addTk(IF);
                else if(nCh==3 && !memcmp(pStartCh, "int", 3)) tk=addTk(INT);
                else if(nCh==6 && !memcmp(pStartCh, "return", 6)) tk=addTk(RETURN);
                else if(nCh==6 && !memcmp(pStartCh, "struct", 6)) tk=addTk(STRUCT);
                else if(nCh==4 && !memcmp(pStartCh, "void", 4)) tk=addTk(VOID);
                else if(nCh==5 && !memcmp(pStartCh, "while", 5)) tk=addTk(WHILE);
                else {
                    tk=addTk(ID);
                    tk->text=createString(pStartCh, pCrtCh);
                }
                return tk->code;
            case 3: 
                if(ch == '\\') {
                    pStartCh=pCrtCh;
                    pCrtCh++;
                    state=4;
                }
                else {
                    pStartCh=pCrtCh;
                    pCrtCh++;
                    state=5;
                }
                break;
            case 4:
                if(ESCAPED_CHARS_COMPARISON(ch)) {
                    pCrtCh++;
                    state=5;
                }
                else {
                    tkerr(addTk(END), "Caracter invalid: %c", ch);
                }
                break;
            case 5: 
                if(ch == '\'') {
                    state=6;
                }
                else {
                    tkerr(addTk(END), "Caracter invalid: %c", ch);
                }
                break;
            case 6:
                tk=addTk(CT_CHAR);
                tk->i=convertFromEscaped(pStartCh, pCrtCh-1)[0];
                pCrtCh++;
                return CT_CHAR;
            case 7: 
                if(ch == '\\') {
                    pCrtCh++;
                    state=8;
                } 
                else if(ch == '"') {
                    pCrtCh++;
                    state=10;
                } 
                else {
                    pCrtCh++;
                    state=9;
                }
                break;
            case 8:
                if(ESCAPED_CHARS_COMPARISON(ch)) {
                    pCrtCh++;
                    state=9;
                }
                else {
                    tkerr(addTk(END), "Caracter invalid: %c", ch);
                }
                break;
            case 9:
                if(ch == '"') {
                    state=10;
                } else {
                    state=7;
                }
                break;
            case 10: 
                tk=addTk(CT_STRING);
                tk->text=convertFromEscaped(pStartCh, pCrtCh-1);
                pCrtCh++;
                return CT_STRING;
            case 11:
                if(ch == '/') {
                    pCrtCh++;
                    state=12;
                }
                else if(ch == '*') {
                    pCrtCh++;
                    state=13;
                }
                else {
                    state=15;
                }
                break;
            case 12:
                if(ch == '\n' || ch == '\r' || ch == '\0') {
                    state=0;
                } else {
                    pCrtCh++;
                }
                break;
            case 13:
                if(ch == '*') {
                    pCrtCh++;
                    state=14;
                } 
                else {
                    if(ch == '\n') {
                        currentLine++;
                    }
                    pCrtCh++;
                }
                break;
            case 14:
                if(ch == '/') {
                    pCrtCh++;
                    state=0;
                } else if(ch == '*'){
                    pCrtCh++;
                } else {
                    if(ch == '\n') {
                        currentLine++;
                    }
                    pCrtCh++;
                    state=13;
                }
                break;
            case 15:
                tk=addTk(DIV);
                return DIV;
            case 16:
                tk=addTk(ADD);
                return ADD;
            case 17:
                tk=addTk(SUB);
                return SUB;
            case 18:
                tk=addTk(MUL);
                return MUL;
            case 19:
                tk=addTk(DOT);
                return DOT;
            case 20:
                if(ch == '&') {
                    pCrtCh++;
                    state=21;
                }
                else {
                    tkerr(addTk(END), "Caracter invalid: %c", ch);
                }
                break;
            case 21:
                tk=addTk(AND);
                return AND;
            case 22:
                if(ch == '|') {
                    pCrtCh++;
                    state=23;
                }
                else {
                    tkerr(addTk(END), "Caracter invalid: %c", ch);
                }
                break;
            case 23:
                tk=addTk(OR);
                return OR;
            case 24:
                if(ch == '=') {
                    pCrtCh++;
                    state=25;
                }
                else {
                    state=26;
                }
                break;
            case 25:
                tk=addTk(NOTEQ);
                return NOTEQ;
            case 26:
                tk=addTk(NOT);
                return NOT;
            case 27:
                if(ch == '=') {
                    pCrtCh++;
                    state=28;
                }
                else {
                    state=29;
                }
                break;
            case 28:
                tk=addTk(EQUAL);
                return EQUAL;
            case 29:
                tk=addTk(ASSIGN);
                return ASSIGN;
            case 30:
                if(ch == '=') {
                    pCrtCh++;
                    state=31;
                }
                else {
                    state=32;
                }
                break;
            case 31:
                tk=addTk(LESSEQ);
                return LESSEQ;
            case 32:
                tk=addTk(LESS);
                return LESS;
            case 33:
                if(ch == '=') {
                    pCrtCh++;
                    state=34;
                }
                else {
                    state=35;
                }
                break;
            case 34:
                tk=addTk(GREATEREQ);
                return GREATEREQ;
            case 35: 
                tk=addTk(GREATER);
                return GREATER;
            case 36:
                tk=addTk(COMMA);
                return COMMA;
            case 37:
                tk=addTk(SEMICOLON);
                return SEMICOLON;
            case 38:
                tk=addTk(LPAR);
                return LPAR;
            case 39:
                tk=addTk(RPAR);
                return RPAR;
            case 40:
                tk=addTk(LBRACKET);
                return LBRACKET;
            case 41:
                tk=addTk(RBRACKET);
                return RBRACKET;
            case 42:
                tk=addTk(LACC);
                return LACC;
            case 43:
                tk=addTk(RACC);
                return RACC;
            case 51: // here starts the numbers state diagram
                if(isnumber(ch)) {
                    pCrtCh++;
                }
                else if(ch == '.') {
                    pCrtCh++;
                    state=58;
                }
                else if(ch == 'e' || ch == 'E') {
                    pCrtCh++;
                    state=61;
                }
                else {
                    base=10;
                    state=52;
                }
                break;
            case 52:
                tk=addTk(CT_INT);
                tk->i=strtol(createString(pStartCh, pCrtCh), NULL, base);
                return CT_INT;
            case 53:
                if(ch == 'x') {
                    pCrtCh++;
                    state=55;
                }
                else if(ch == '.') {
                    pCrtCh++;
                    state=58;
                }
                else if(ch == 'e' || ch == 'E') {
                    pCrtCh++;
                    state=61;
                }
                else if(isnumber(ch)) {
                    pCrtCh++;
                    if(ch == '8' || ch == '9') {
                        state=57;
                    }
                    else {
                        state=54;
                    }
                }
                else {
                    base=10;
                    state=52;
                }
                break;
            case 54:
                if(ch == '.') {
                    pCrtCh++;
                    state=58;
                }
                else if(ch == 'e' || ch == 'E') {
                    pCrtCh++;
                    state=61;
                }
                else if(isnumber(ch)) {
                    pCrtCh++;
                    if(ch == '8' || ch == '9') {
                        state=57;
                    }
                }
                else {
                    base=8;
                    state=52;
                }
                break;
            case 55:
                if(HEXA_CHARS_COMPARISON(ch)) {
                    pCrtCh++;
                    state=56;
                }
                else {
                    tkerr(addTk(END), "Caracter invalid: %c", ch);
                }
                break;
            case 56:
                if(HEXA_CHARS_COMPARISON(ch)) {
                    pCrtCh++;
                }
                else {
                    base=16;
                    state=52;
                }
                break;
            case 57:
                if(isnumber(ch)) {
                    pCrtCh++;
                }
                else if(ch == '.') {
                    pCrtCh++;
                    state=58;
                }
                else if(ch == 'e' || ch == 'E') {
                    pCrtCh++;
                    state=61;
                }
                else {
                    tkerr(addTk(END), "Caracter invalid: %c", ch);
                }
                break;
            case 58:
                if(isnumber(ch)) {
                    pCrtCh++;
                    state=59;
                }
                else {
                    tkerr(addTk(END), "Caracter invalid: %c", ch);
                }
                break;
            case 59:
                if(isnumber(ch)) {
                    pCrtCh++;
                }
                else if(ch == 'e' || ch == 'E') {
                    pCrtCh++;
                    state=61;
                }
                else {
                    state=60;
                }
                break;
            case 60:
                tk=addTk(CT_REAL);
                tk->r=strtof(createString(pStartCh, pCrtCh), NULL);
                return CT_REAL;
            case 61:
                if(isnumber(ch)) {
                    pCrtCh++;
                    state=63;
                }
                else if(ch == '+' || ch == '-') {
                    pCrtCh++;
                    state=62;
                }
                else {
                    tkerr(addTk(END), "Caracter invalid: %c", ch);
                }
                break;
            case 62:
                if(isnumber(ch)) {
                    pCrtCh++;
                    state=63;
                }
                else {
                    tkerr(addTk(END), "Caracter invalid: %c", ch);
                }
                break;
            case 63:
                if(isnumber(ch)) {
                    pCrtCh++;
                }
                else {
                    state=60;
                }
                break;
            default:
                err("State not implemented");
        }
    }
}

void cleanInit(char *buffer) {
    tokens = NULL;
    lastToken = NULL;
    currentLine = 1;
    pCrtCh = buffer;
}

Token *lexicalAnalysis(char *buffer) {
    int readToken;
    cleanInit(buffer);
    do {
        readToken = getNextToken();
    } while(readToken != END);
    return tokens;
}
