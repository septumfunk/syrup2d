//? septumfunk 2024
#include "fs.h"
#include "crypto.h"
#include "result.h"
#include "md5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

result_t fs_load(const char *path, char **out, fs_size_t *size) {
    if (!fs_exists(path))
        return error("FileNotFoundError", "The requested file was not found on the filesystem.");
    FILE *f = fopen(path, "r");
    if (!f)
        return error("FileOpenError", "Call to function fopen failed.");

    // Stat size
    fseek(f, 0, SEEK_END);
    *size = (fs_size_t)ftell(f);
    fseek(f, 0, SEEK_SET);

    // Allocate buffer
    *out = calloc(1, *size);

    // Read into buffer
    if (fread(*out, *size, 1, f) < 0)
        return error("FileReadError", "The requested file was unable to be read. Please ensure assets are not corrupt.");
    fclose(f);

    return no_error();
}

result_t fs_load_checksum(const char *path, char **out, fs_size_t *size) {
    char *buffer;
    result_t res = fs_load(path, &buffer, size);
    if (res.is_error)
        return res;

    *size -= MD5_LENGTH;
    char *file = calloc(1, *size);
    memcpy(file, buffer + MD5_LENGTH, *size);

    uint8_t checksum[MD5_LENGTH];
    md5_hash((uint8_t *)file, *size, checksum);
    if (memcmp(checksum, buffer, MD5_LENGTH) != 0) {
        free(buffer);
        free(file);
        return error("ChecksumCorruptError", "Checksum check failed against requested file. It is likely corrupt.");
    }

    *out = file;
    free(buffer);
    return no_error();
}

result_t fs_save(const char *path, const char *buffer, fs_size_t size) {
    FILE *f = fopen(path, "w");
    if (!f)
        return error("FileOpenError", "Call to function fopen failed.");

    if (fwrite(buffer, size, 1, f) < 0)
        return error("FileWriteError", "The requested file was unable to be written to. Please ensure assets are not corrupt.");
    fclose(f);

    return no_error();
}

result_t fs_save_checksum(const char *path, const char *buffer, fs_size_t size) {
    char *f_buffer = malloc(MD5_LENGTH + size);
    md5_hash((uint8_t *)buffer, size, (uint8_t *)f_buffer);
    result_t res = fs_save(path, f_buffer, MD5_LENGTH + size);
    free(f_buffer);
    return res;
}

result_t fs_size(const char *path, fs_size_t *out) {
    FILE *f = fopen(path, "r");
    if (!f)
        return error("FileOpenError", "Call to function fopen failed.");;

    fseek(f, 0, SEEK_END);
    *out = (fs_size_t)ftell(f);
    fclose(f);

    return no_error();
}

bool fs_exists(const char *path) {
    fs_size_t size;
    if (fs_size(path, &size).is_error || size < 0)
        return false;
    return true;
}
