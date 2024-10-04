//? septumfunk 2024
#pragma once
#include <stdbool.h>

/// Errors that can be returned by filesystem functions
typedef enum fs_err_e {
    FS_ERR_NONE, /// No issue processing file.
    FS_ERR_OPEN, /// Failed to fopen file.
    FS_ERR_NOT_FOUND, /// Failed to locate file.
    FS_ERR_READ, /// Failed to read from file.
    FS_ERR_WRITE, /// Failed to write to file.
} fs_err_e;

typedef unsigned long long fs_size_t;

/// Load file from path.
/// Out mustn't be allocated, it will be allocated automatically.
fs_err_e fs_read(const char *path, char **out);

/// Save file to path.
fs_err_e fs_write(const char *path, const char *buffer, fs_size_t size);

/// Get the size of a file at path.
fs_err_e fs_size(const char *path, fs_size_t *out);

/// Check if file exists.
bool fs_exists(const char *path);
