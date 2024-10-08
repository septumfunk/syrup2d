//? septumfunk 2024
#include "fs.h"
#include <stdio.h>
#include <stdlib.h>

fs_err_e fs_read(const char *path, char **out) {
    FILE *f = fopen(path, "r");
    if (!f)
        return FS_ERR_OPEN;

    // Stat size
    fseek(f, 0, SEEK_END);
    fs_size_t size = (fs_size_t)ftell(f);
    fseek(f, 0, SEEK_SET);

    // Allocate buffer
    *out = calloc(1, size);

    // Read into buffer
    if (fread(*out, size, 1, f) < 0)
        return FS_ERR_READ;
    fclose(f);

    return FS_ERR_NONE;
}

fs_err_e fs_write(const char *path, const char *buffer, fs_size_t size) {
    FILE *f = fopen(path, "w");
    if (!f)
        return FS_ERR_OPEN;

    if (fwrite(buffer, size, 1, f) < 0)
        return FS_ERR_WRITE;
    fclose(f);

    return FS_ERR_NONE;
}

fs_err_e fs_size(const char *path, fs_size_t *out) {
    FILE *f = fopen(path, "r");
    if (!f)
        return FS_ERR_OPEN;

    fseek(f, 0, SEEK_END);
    *out = (fs_size_t)ftell(f);
    fclose(f);

    return FS_ERR_NONE;
}

bool fs_exists(const char *path) {
    fs_size_t size;
    if (fs_size(path, &size) || size < 0)
        return false;
    return true;
}
