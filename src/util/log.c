#include "log.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

void log_info(const char *format, ...) {
    va_list arglist;

    va_start(arglist, format);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *text = calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(text, size + 1, format, arglist);
    va_end(arglist);

    printf(LOG_INFO_FORMAT, text);
    free(text);
}

void log_warn(const char *format, ...) {
    va_list arglist;

    va_start(arglist, format);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *text = calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(text, size + 1, format, arglist);
    va_end(arglist);

    printf(LOG_WARN_FORMAT, text);
    free(text);
}

void log_error(const char *format, ...) {
    va_list arglist;

    va_start(arglist, format);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *text = calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(text, size + 1, format, arglist);
    va_end(arglist);

    printf(LOG_ERROR_FORMAT, text);
    free(text);
}

void log_header(const char *format, ...) {
    va_list arglist;

    va_start(arglist, format);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *text = calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(text, size + 1, format, arglist);
    va_end(arglist);

    printf(LOG_HEADER_FORMAT, text);
    free(text);
}
