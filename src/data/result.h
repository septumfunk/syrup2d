//? septumfunk 2024
#pragma once
#include "../win32/msgbox.h"

/// Error structure for passing around the program.
typedef struct result_t {
    bool is_error;
    const char *type;
    const char *description;
} result_t;

/// Construct an error for returning.
/// Please only use this with string literals.
result_t error(const char *type, const char *description);
/// Check if an error is of a certain type.
bool error_is(result_t error, const char *type);
/// Construct a default "No Error" error.
result_t no_error(void);
/// Panic if an result_t is a valid error
void panic(result_t result);

#define ERROR_FORMAT_FATAL "\
Type: %s\n\
Location: %s:%d\n\
Description: %s\
"
#define error_fatal(result) msgbox_error("Fatal Error", ERROR_FORMAT_FATAL, result.type, __FILE__, __LINE__, result.description)

#define ERROR_FORMAT_WARN "\
Type: %s\n\
Location: %s:%d\n\
Description: %s\
"
#define error_warn(result) msgbox_warn("Warning", ERROR_FORMAT_WARN, result.type, __FILE__, __LINE__, result.description)