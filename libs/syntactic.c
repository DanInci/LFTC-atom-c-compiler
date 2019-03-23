
#include "../headers/util.h"
#include "../headers/syntactic.h"

Token *crtTk, *consumedToken;

int consume(int code) {
    if(crtTk->code == code) {
        consumedToken=crtTk;
        return 1;
    }
    return 0;
}

void syntacticAnalysis(Token *start) {
    
}