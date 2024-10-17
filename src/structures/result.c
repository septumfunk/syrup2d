#include "result.h"
#include "../util/stringext.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

result_t result_error(const char *type, const char *description, ...) {
    va_list arglist;

    va_start(arglist, description);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, description, arglist);
    va_end(arglist);

    char *text = calloc(1, size + 1);
    va_start(arglist, description);
    vsnprintf(text, size + 1, description, arglist);
    va_end(arglist);

    return (result_t) {
        .is_error = true,
        .type = strdup(type),
        .description = text,
    };
}

result_t result_no_error(void) {
    return (result_t) {
        .is_error = false,
        .type = NULL,
        .description = NULL,
    };
}

void result_discard(result_t result) {
    free(result.type);
    free(result.description);
}

bool result_match(result_t result, const char *type) {
    return bstrcmp(result.type, type);
}