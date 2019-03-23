#ifndef UTIL
#define UTIL

void err(const char *fmt, ...);

char *createString(const char *startCh, const char *endCh);

char *getOutputFilePath(const char *inputFilePath);

char *convertToEscaped(const char *str);

char *convertFromEscaped(const char *startCh, const char *endCh);

#endif