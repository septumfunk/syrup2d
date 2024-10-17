#pragma once
#include <stdbool.h>

/// Allocates and formats a string automatically.
/// Returns a dynamically allocated string, be sure to free it.
char *format(const char *format, ...);

/// Strcmp but it returns a bool.
bool bstrcmp(const char *first, const char *second);