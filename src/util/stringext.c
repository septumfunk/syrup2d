#include "stringext.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *format(const char *format, ...) {
    va_list arglist;

    va_start(arglist, format);
    unsigned long long size = (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *buf = calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(buf, size + 1, format, arglist);
    va_end(arglist);

    return buf;
}

bool bstrcmp(const char *first, const char *second) { return strcmp(first, second) == 0; }