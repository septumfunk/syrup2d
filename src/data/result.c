//? septumfunk 2024
#include "result.h"
#include "stringext.h"
#include <stdlib.h>

result_t error(const char *type, const char *description) {
    return (result_t) {
        true,
        type,
        description,
    };
}

bool error_is(result_t error, const char *type) {
    return bstrcmp(error.type, type);
}

result_t no_error(void) {
    return (result_t) {
        false,
        "None",
        "No error occurred.",
    };
}

void panic(result_t result) {
    if (result.is_error) {
        error_fatal(result);
        exit(1);
    }
}