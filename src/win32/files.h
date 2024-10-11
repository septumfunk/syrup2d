//? septumfunk 2024
#pragma once
#include "../data/result.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

/// Return all objects in a directory as an array of strings.
/// Do not allocate out.
/// Returns null if path is not found.
char **win_lsdir(const char *path, int *count);