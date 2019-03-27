#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "headers/util.h"
#include "headers/lexical.h"
#include "headers/syntactic.h"

#define INPUT_FOLDER "tests"
#define BUFFER_SIZE 10000

void clean(Token *start) {
    Token *aux, *q = start;
    while(q) {
        aux = q;
        q = q->next;
        if (aux->code == ID || aux->code == CT_STRING) {
            free(aux->text);
        }
        free(aux);
    }
}

void readFromFile(const char *filePath, char *buffer) {
    FILE *f;
    int n;
    f = fopen(filePath, "rt");
    if(!f) {
        err("Failed to open input file %s", *filePath);
    }
    n = fread(buffer, 1, BUFFER_SIZE, f);
    buffer[n] = '\0';
    fclose(f);
}

void writeToFile(const char *filePath, Token *start) {
    FILE *g;
    Token *q;
    g = fopen(filePath, "wt");
    if(!g) {
        err("Failed to open output file %s", *filePath);
    }
    q=start;
    while(q) {
        fprintf(g, "%d: %s\n", q->line, toString(q));
        q=q->next;
    }
    fclose(g);
}

void compile(char *inputFilePath, char *outputFilePath) {
    char inBuffer[BUFFER_SIZE + 1];
    Token *tokens;

    readFromFile(inputFilePath, inBuffer);
    
    tokens = lexicalAnalysis(inBuffer);
    syntacticAnalysis(tokens);

    writeToFile(outputFilePath, tokens);
    
    clean(tokens);
}

int main(int argv, char *argc[]) {
    if(argv > 1) { // compile file given as argument
        char *outputFilePath = getOutputFilePath(argc[1]);
        compile(argc[1], outputFilePath);
        free(outputFilePath);
    }
    else { // compile files from tests folder
        DIR *dir;
        struct dirent *ent;
        char *inputFilePath, *outputFilePath;

        dir = opendir(INPUT_FOLDER);
        if(!dir) {
            err("Usage: %s <file> or <%s> folder required", argc[0], INPUT_FOLDER);
        }
       
        while ((ent = readdir(dir)) != NULL) {
            if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0 || strstr(ent->d_name, ".out") > 0 ) {
                continue;
            }
    
            printf("Compiling %s ...\n", ent->d_name);
            
            inputFilePath = (char *) malloc((strlen(INPUT_FOLDER) + strlen(ent->d_name) + 2) * sizeof(char));
            sprintf(inputFilePath, "%s/%s", INPUT_FOLDER, ent->d_name);
            outputFilePath = getOutputFilePath(inputFilePath);
            
            compile(inputFilePath, outputFilePath);
            
            free(inputFilePath);
            free(outputFilePath);
        }
        closedir(dir);
    }
    return 0;
}