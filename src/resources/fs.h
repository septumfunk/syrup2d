#pragma once
#include "../structures/result.h"
#include <stdbool.h>

typedef unsigned long long fs_size_t;

/// Load file from path.
/// Out mustn't be allocated, it will be allocated automatically.
result_t fs_load(const char *path, char **out, fs_size_t *size);
/// Load checksummed file from path.
/// Out mustn't be allocated, it will be allocated automatically.
result_t fs_load_checksum(const char *path, char **out, fs_size_t *size);

/// Save file to path.
result_t fs_save(const char *path, const char *buffer, fs_size_t size);
/// Save checksummed file to path.
result_t fs_save_checksum(const char *path, const char *buffer, fs_size_t size);

/// Get the size of a file at path.
result_t fs_size(const char *path, fs_size_t *out);

/// Check if file exists.
bool fs_exists(const char *path);
