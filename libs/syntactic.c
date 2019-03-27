#include <stdio.h>
#include <stdlib.h>

#include "../headers/util.h"
#include "../headers/syntactic.h"

Token *crtTk, *consumedTk;

int expr();
int stm();
int stmCompound();
int declVar();
int typeName();

int consume(int code) {
    if(crtTk->code == code) {
        consumedTk=crtTk;
        crtTk=crtTk->next;
        return 1;
    }
    return 0;
}

int exprPrimary() {
    if(consume(ID)) {
        if(consume(LPAR)) {
            if(expr()) {
                while(1) {
                    if(consume(COMMA)) {
                        if(!expr()) tkerr(crtTk, "missing expression after ,");
                    }
                    else break;
                }
            }
            if(!consume(RPAR)) tkerr(crtTk, "missing )");
        }
        return 1;
    }
    else if(consume(CT_INT) || consume(CT_REAL) || consume(CT_CHAR) || consume(CT_STRING)) {
        return 1;
    }
    else if(consume(LPAR)) {
        if(!expr()) tkerr(crtTk, "invalid expression");
        if(!consume(RPAR)) tkerr(crtTk, "missing )");
        return 1;
    }
    return 0;
}

int exprPostfix1() {
    if(consume(LBRACKET)) {
        if(!expr()) tkerr(crtTk, "invalid expression");
        if(!consume(RBRACKET)) tkerr(crtTk, "missing ]");
        if(!exprPostfix1()) tkerr(crtTk, "missing postfix1 expression");
        return 1;
    }
    else if(consume(DOT)) {
        if(!consume(ID)) tkerr(crtTk, "missind ID");
        if(!exprPostfix1()) tkerr(crtTk, "missing postfix1 expression");
        return 1;
    }
    return 1;
}

int exprPostfix() {
    if(!exprPrimary()) return 0;
    if(!exprPostfix1()) tkerr(crtTk, "missing postfix1 expression");
    return 1;
}

int exprUnary() {
    if(consume(SUB) || consume(NOT)) {
        if(!exprUnary()) tkerr(crtTk, "missing unary expression");
        return 1;
    }
    else if(exprPostfix()) {
        return 1;
    }
    return 0;
}

int exprCast() {
    if(consume(LPAR)) {
        if(!typeName()) tkerr(crtTk, "missing type name");
        if(!consume(RPAR)) tkerr(crtTk, "missing )");
        if(!exprCast()) tkerr(crtTk, "missing cast expression");
        return 1;
    }
    else if(exprUnary()) {
        return 1;
    }
    return 0;
}

int exprMul1() {
    if(consume(MUL) || consume(DIV)) {
        if(!exprCast()) tkerr(crtTk, "missing cast expression after (*//)");
        if(!exprMul1()) tkerr(crtTk, "missing mul1 expression");
        return 1;
    }
    return 1;
}

int exprMul() {
    if(!exprCast()) return 0;
    if(!exprMul1()) tkerr(crtTk, "missing mul1 expression");
    return 1;
}

int exprAdd1() {
    if(consume(ADD) || consume(SUB)) {
        if(!exprMul()) tkerr(crtTk, "mising mul expression after (+/-)");
        if(!exprAdd1()) tkerr(crtTk, "missing add1 expression");
        return 1;
    }
    return 1;
}

int exprAdd() {
    if(!exprMul()) return 0;
    if(!exprAdd1()) tkerr(crtTk, "missing add1 expression");
    return 1;
}

int exprRel1() {
    if(consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ)) {
        if(!exprAdd()) tkerr(crtTk, "missing add expression after (</<=/>/>=)");
        if(!exprRel1()) tkerr(crtTk, "missing rel1 expression");
        return 1;
    }
    return 1;
}

int exprRel() {
    if(!exprAdd()) return 0;
    if(!exprRel1()) tkerr(crtTk, "missing rel1 expression");
    return 1;
}

int exprEq1() {
    if(consume(EQUAL) || consume(NOTEQ)) {
        if(!exprRel()) tkerr(crtTk, "missing rel expression after (==/!=)");
        if(!exprEq1()) tkerr(crtTk, "missing eq1 expression"); 
        return 1;
    }
    return 1;
}

int exprEq() {
    if(!exprRel()) return 0;
    if(!exprEq1()) tkerr(crtTk, "missing eq1 expression");
    return 1;
}

int exprAnd1() {
    if(consume(AND)) {
        if(!exprEq()) tkerr(crtTk, "missing eq expression after &&");
        if(!exprAnd1()) tkerr(crtTk, "missing and1 expression");
        return 1;
    }
    return 1;
}

int exprAnd() {
    if(!exprEq()) return 0;
    if(!exprAnd1()) tkerr(crtTk, "missing and1 expression");
    return 1;
}

int exprOr1() {
    if(consume(OR)) {
        if(!exprAnd()) tkerr(crtTk, "missing and expression after ||");
        if(!exprOr1()) tkerr(crtTk, "missing or1 expression");
        return 1;
    }
    return 1;
}

int exprOr() {
    if(!exprAnd()) return 0;
    if(!exprOr1()) tkerr(crtTk, "missing or1 expression");
    return 1;
}

int exprAssign() {
    Token *startTk = crtTk;
    if(exprUnary()) {
        if(consume(ASSIGN)) {
            if(exprAssign()) {
                return 1;
            }
        }
        crtTk=startTk;
    }
    if(exprOr()) {
        return 1;
    }
    return 0;
}

int expr() {
    if(!exprAssign()) return 0;
    return 1;
}

int ruleIf() {
    if(!consume(IF)) return 0;
    if(!consume(LPAR)) tkerr(crtTk, "missing ( after if");
    if(!expr()) tkerr(crtTk, "invalid condition expression for if");
    if(!consume(RPAR)) tkerr(crtTk, "missing )");
    if(!stm()) tkerr(crtTk, "missing if statement");
    if(consume(ELSE)) {
        if(!stm()) tkerr(crtTk, "missing else statement");
    }
    return 1;
}

int ruleWhile() {
    if(!consume(WHILE)) return 0;
    if(!consume(LPAR)) tkerr(crtTk, "missing ( after while");
    if(!expr()) tkerr(crtTk, "invalid conditional expression for while");
    if(!consume(RPAR)) tkerr(crtTk, "missing )");
    if(!stm()) tkerr(crtTk, "missing while statement");
    return 1;
}

int ruleFor() {
    if(!consume(FOR)) return 0;
    if(!consume(LPAR)) tkerr(crtTk, "missing ( after for");
    expr();
    if(!consume(SEMICOLON)) tkerr(crtTk, "missing ; inside for condition");
    expr();
    if(!consume(SEMICOLON)) tkerr(crtTk, "missing ; inside for condition");
    expr();
    if(!consume(RPAR)) tkerr(crtTk, "missing )");
    if(!stm()) tkerr(crtTk, "missing for statement");
    return 1;
}

int ruleBreak() {
    if(!consume(BREAK)) return 0;
    if(!consume(SEMICOLON)) tkerr(crtTk, "missing ; after break");
    return 1;
}

int ruleReturn() {
    if(!consume(RETURN)) return 0;
    expr();
    if(!consume(SEMICOLON)) tkerr(crtTk, "missing ; after return");
    return 1;
}

int ruleExprLine() {
    expr();
    // if(!consume(SEMICOLON)) tkerr(crtTk, "missing ; after expression");
    if(consume(SEMICOLON)) {
        return 1;
    }
    return 0;
}

int stm() {
    Token *startTk = crtTk;
    if(stmCompound() || ruleIf() || ruleWhile() || ruleFor() || ruleBreak() || ruleReturn() || ruleExprLine()) {
        return 1;
    }
    crtTk = startTk;
    return 0;
}

int stmCompound() {
    if(!consume(LACC)) return 0;
    while(1) {
        if(declVar()) {}
        else if(stm()) {}
        else break;
    }
    if(!consume(RACC)) tkerr(crtTk, "missing ; or syntax error");
    return 1;
}

int arrayDecl() {
    if(!consume(LBRACKET)) return 0;
    expr();
    if(!consume(RBRACKET)) tkerr(crtTk, "missing ]");
    return 1;
}

int typeBase() {
    if(consume(INT) || consume(DOUBLE) || consume(CHAR)) {
        return 1;
    }
    else if(consume(STRUCT)) {
        if(!consume(ID)) tkerr(crtTk, "missing struct ID");
        return 1;
    }
    return 0;
}

int typeName() {
    if(!typeBase()) return 0;
    arrayDecl();
    return 1;
}

int funcArg() {
    if(!typeBase()) return 0;
    if(!consume(ID)) tkerr(crtTk, "missing ID for function argument");
    arrayDecl();
    return 1;
}

int declFunc() {
    Token *startTk = crtTk;
    if(typeBase()) {
        consume(MUL);
        if(consume(ID)) {
            if(consume(LPAR)) {
                if(funcArg()) {
                    while(1) {
                        if(consume(COMMA)) {
                            if(!funcArg()) tkerr(crtTk, "missing function argument");
                        }
                        else break;
                    }
                }
                if(!consume(RPAR)) tkerr(crtTk, "missing ) after function");
                if(!stmCompound()) tkerr(crtTk, "missing function statement");
                return 1;
            }
        }
    }
    else if(consume(VOID)) {
        if(!consume(ID)) tkerr(crtTk, "missing ID for function");
        if(!consume(LPAR)) tkerr(crtTk, "missing ( after function");
        if(funcArg()) {
            while(1) {
                if(consume(COMMA)) {
                    if(!funcArg()) tkerr(crtTk, "missing function argument");
                }
                else break;
            }
        }
        if(!consume(RPAR)) tkerr(crtTk, "missing ) after function");
        if(!stmCompound()) tkerr(crtTk, "missing function statement");
        return 1;
    }
    crtTk = startTk;
    return 0;
}

int declVar() {
    if(!typeBase()) return 0;
    if(!consume(ID)) tkerr(crtTk, "missing ID");
    arrayDecl();
    while(1) {
        if(consume(COMMA)) {
            if(!consume(ID)) tkerr(crtTk, "missing variable ID");
            arrayDecl();
        }
        else break;
    }
    if(!consume(SEMICOLON)) tkerr(crtTk, "missing ; after variable declaration");
    return 1;
}

int declStruct() {
    Token *startTk = crtTk;
    if(consume(STRUCT)) {
        if(consume(ID)) {
            if(consume(LACC)) {
                while(1) {
                    if(declVar()) {}
                    else break;
                }
                if(!consume(RACC)) tkerr(crtTk, "missing } after struct declaration");
                if(!consume(SEMICOLON)) tkerr(crtTk, "missing ; after struct");
                return 1;
            }
        }
    }
    crtTk=startTk;
    return 0;
}

int unit() {
    while(1) {
        if(declStruct()) {}
        else if(declFunc()) {}
        else if(declVar()) {}
        else break;
    }
    if(!consume(END)) tkerr(crtTk, "Failed to reach the END");
    return 1;
}

void init(Token *start) {
    crtTk = start;
    consumedTk = NULL;
}

void syntacticAnalysis(Token *start) {
    init(start);
    unit();
}