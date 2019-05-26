#ifndef UTIL
#define UTIL

#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL) err("not enough memory");

void err(const char *fmt, ...);

char *createString(const char *startCh, const char *endCh);

char *getOutputFilePath(const char *inputFilePath);

char *convertToEscaped(const char *str);

char *convertFromEscaped(const char *startCh, const char *endCh);

#endif