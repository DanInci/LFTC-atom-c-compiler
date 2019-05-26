#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/util.h"
#include "../headers/syntactic.h"
#include "../headers/domain.h"

Token *crtTk, *consumedTk;
Symbols *symbols;
Symbol *crtFunc;
Symbol *crtStruct;
int crtDepth;

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

Symbol *addSymbol(Symbols *symbols, const char *name, int cls) {
    Symbol *s;
    if(symbols->end == symbols->after) {
        int count = symbols->after - symbols->begin;
        int n = count * 2;
        if(n == 0) {
            n = 1;
        }
        symbols->begin = (Symbol**)realloc(symbols->begin, n*sizeof(Symbols*));
        if(symbols->begin == NULL) {
            err("not enough memory");
        }
        symbols->end = symbols->begin+count;
        symbols->after = symbols->begin+n;
    }
    SAFEALLOC(s, Symbol);
    *symbols->end++=s;
    s->name=name;
    s->cls=cls;
    s->depth=crtDepth;
    return s;
}

Symbol *findSymbol(Symbols *symbols, const char *name) {
    int i, n;
    n=symbols->end-symbols->begin;
    for(i=n-1; i>=0; i--) {
        if(strcmp(symbols->begin[i]->name, name) == 0) {
            return symbols->begin[i];
        }
    }
    return NULL;
}

void deleteSymbolsAfter(Symbols *symbols, Symbol *start) {
	int i, n = symbols->end - symbols->begin;

	for (i = n - 1; i >= 0; i--) {
		if (strcmp(symbols->begin[i]->name, start->name) != 0) {
            free(symbols->begin[i]);
            symbols->begin[i] = NULL;
		}
        else {
            break;
        }
	}

    symbols->end = &symbols->begin[i+1];
}


void addVariableSymbol(Token *token, Type *t) {
	Symbol *s;
	if (crtStruct) {
		if (findSymbol(&crtStruct->members, token->text)) {
			tkerr(crtTk, "symbol redefinition: %s", token->text);
        }
		s = addSymbol(&crtStruct->members, token->text, CLS_VAR);
	}
	else if (crtFunc) {
		s = findSymbol(symbols, token->text);
		if (s && s->depth == crtDepth) {
			tkerr(crtTk, "symbol redefinition: %s", token->text);
        }
		s = addSymbol(symbols, token->text, CLS_VAR);
		s->mem = MEM_LOCAL;
	}
	else {
		if (findSymbol(symbols, token->text)) {
			tkerr(crtTk, "symbol redefinition: %s", token->text);
        }		
	    s = addSymbol(symbols, token->text, CLS_VAR);			
        s->mem = MEM_GLOBAL;
	}
	s->type = *t;
}

void initSymbols(Symbols *symbols) {
	symbols->begin = symbols->end = symbols->after = NULL;
}

void printSymbols(Symbols *symbols) {
	int i;
	int n = symbols->end - symbols->begin;
		
	for (i = 0; i < n; i++) {
		printf("%s\n", symbolToString(symbols->begin[i]));
	}
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
        if(!consume(ID)) tkerr(crtTk, "missing ID");
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
    Type t;
    if(consume(LPAR)) {
        if(!typeName(&t)) tkerr(crtTk, "missing type name");
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
    Symbol *start = symbols->end[-1];
    if(!consume(LACC)) return 0;
    crtDepth++;
    while(1) {
        if(declVar()) {}
        else if(stm()) {}
        else break;
    }
    if(!consume(RACC)) tkerr(crtTk, "missing ; or syntax error");
    deleteSymbolsAfter(symbols, start);
    crtDepth--;
    return 1;
}

int arrayDecl(Type *ret) {
    if(!consume(LBRACKET)) return 0;
    expr();
    ret->nElements = 0;
    if(!consume(RBRACKET)) tkerr(crtTk, "missing ]");
    return 1;
}

int typeBase(Type *ret) {
    Token *startTk = crtTk;
	Token *tkName;
    
    if (consume(INT)) {
   	    ret->typeBase = TB_INT;
	    return 1;
	}
	else if (consume(DOUBLE)) {
		ret->typeBase = TB_DOUBLE;
	    return 1;
	}
	else if (consume(CHAR)) {
		ret->typeBase = TB_CHAR;
	    return 1;
	}
    else if(consume(STRUCT)) {
        if(!consume(ID)) tkerr(crtTk, "missing struct ID");
        tkName = consumedTk;
	    Symbol *s = findSymbol(symbols, tkName->text);
	        	
	    if (s == NULL) {
	        tkerr(crtTk, "undefined symbol: %s", tkName->text);
        }
	    		
	    if (s->cls != CLS_STRUCT) {
	    	tkerr(crtTk, "%s is not a struct", tkName->text);
        }
	    			
		ret->typeBase = TB_STRUCT;
		ret->s = s;
        return 1;
    }
    crtTk = startTk;
    return 0;
}

int typeName(Type *ret) {
    if(!typeBase(ret)) return 0;
    if(arrayDecl(ret) != 1) {
        ret->nElements = -1;
    }
    return 1;
}

int funcArg() {
    Type t;
    if(!typeBase(&t)) return 0;
    if(!consume(ID)) tkerr(crtTk, "missing ID for function argument");
    Token *idToken = consumedTk;
    if(!arrayDecl(&t)) {
        t.nElements = -1;
    }

    Symbol *s = addSymbol(symbols, idToken->text, CLS_VAR);
	s->mem = MEM_ARG;
	s->type = t;

	s = addSymbol(&crtFunc->args, idToken->text, CLS_VAR);
	s->mem = MEM_ARG;
    s->type = t;

    return 1;
}

int declFunc() {
    Token *startTk = crtTk;
    Type t;
    if(typeBase(&t)) {
        if(consume(MUL)) {
            t.nElements = 0;
        }
        else {
            t.nElements = -1;
        }
        if(consume(ID)) {
            Token *idToken = consumedTk;
            if(consume(LPAR)) {
                if (findSymbol(symbols, idToken->text)) {
	        		tkerr(crtTk, "symbol redefinition: %s", idToken->text);
                }
	        	crtFunc = addSymbol(symbols, idToken->text, CLS_FUNC);
	        	initSymbols(&crtFunc->args);
	        	crtFunc->type = t;
	        	crtDepth++;
                if(funcArg()) {
                    while(1) {
                        if(consume(COMMA)) {
                            if(!funcArg()) tkerr(crtTk, "missing function argument");
                        }
                        else break;
                    }
                }
                if(!consume(RPAR)) tkerr(crtTk, "missing ) after function");
                crtDepth--;
                if(!stmCompound()) tkerr(crtTk, "missing function statement");
                deleteSymbolsAfter(symbols, crtFunc);
	            crtFunc = NULL;
                return 1;
            }
        }
    }
    else if(consume(VOID)) {
        t.typeBase = TB_VOID;
        if(!consume(ID)) tkerr(crtTk, "missing ID for function");
        Token *idToken = consumedTk;
        if(!consume(LPAR)) tkerr(crtTk, "missing ( after function");
        if (findSymbol(symbols, idToken->text)) {
	        tkerr(crtTk, "symbol redefinition: %s", idToken->text);
        }
	        		
	    crtFunc = addSymbol(symbols, idToken->text, CLS_FUNC);
	    initSymbols(&crtFunc->args);
	        	
	    crtFunc->type = t;
    	crtDepth++;
        if(funcArg()) {
            while(1) {
                if(consume(COMMA)) {
                    if(!funcArg()) tkerr(crtTk, "missing function argument");
                }
                else break;
            }
        }
        if(!consume(RPAR)) tkerr(crtTk, "missing ) after function");
        crtDepth--;
        if(!stmCompound()) tkerr(crtTk, "missing function statement");
        deleteSymbolsAfter(symbols, crtFunc);
	    crtFunc = NULL;
        return 1;
    }
    crtTk = startTk;
    return 0;
}

int declVar() {
    Type t;
    if(!typeBase(&t)) return 0;
    if(!consume(ID)) tkerr(crtTk, "missing ID");
    Token *idToken = consumedTk;
    if(arrayDecl(&t) != 1) {
        t.nElements = -1;
    }
    addVariableSymbol(idToken, &t);
    while(1) {
        if(consume(COMMA)) {
            if(!consume(ID)) tkerr(crtTk, "missing variable ID");
            Token *idToken = consumedTk;
            if(arrayDecl(&t) != 1) {
                t.nElements = -1;
            }
            addVariableSymbol(idToken, &t);
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
            Token *idToken = consumedTk;
            if(consume(LACC)) {
                if (findSymbol(symbols, idToken->text)) {
                    tkerr(crtTk, "symbol redefinition: %s", idToken->text);
                }
                crtStruct = addSymbol(symbols, idToken->text, CLS_STRUCT);
		        initSymbols(&crtStruct->members);
                while(1) {
                    if(declVar()) {}
                    else break;
                }
                if(!consume(RACC)) tkerr(crtTk, "missing } after struct declaration");
                if(!consume(SEMICOLON)) tkerr(crtTk, "missing ; after struct");
                crtStruct = NULL;
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

void initSyntactic(Token *start) {
    crtTk = start;
    consumedTk = NULL;
    crtDepth = 0;
    SAFEALLOC(symbols, Symbols);
    initSymbols(symbols);
}

void syntacticAnalysis(Token *start) {
    initSyntactic(start);
    unit();
    printSymbols(symbols);
}