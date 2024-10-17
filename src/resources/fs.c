#include "fs.h"
#include "../util/crypto.h"
#include "../structures/result.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

result_t fs_load(const char *path, char **out, fs_size_t *size) {
    if (!fs_exists(path))
        return result_error("FileNotFoundError", "Requested file '%s' could not be found.", path);
    FILE *f = fopen(path, "rb");
    if (!f)
        return result_error("FileOpenError", "Call to fopen on requested file '%s' failed.", path);

    // Stat size
    fseek(f, 0, SEEK_END);
    *size = (fs_size_t)ftell(f);
    fseek(f, 0, SEEK_SET);

    // Allocate buffer
    *out = calloc(1, *size + 1);

    // Read into buffer
    if (fread(*out, *size, 1, f) < 0)
        return result_error("FileReadError", "The requested file '%s' was unable to be read.", path);
    fclose(f);

    return result_no_error();
}

result_t fs_load_checksum(const char *path, char **out, fs_size_t *size) {
    *size = 0;
    char *buffer;
    result_t res = fs_load(path, &buffer, size);
    if (res.is_error)
        return res;
    if (*size <= MD5_LENGTH)
        return result_error("ChecksumCorruptError", "Checksum for requested file '%s' is invalid.", path);

    *size -= MD5_LENGTH;
    char *file = calloc(1, *size);
    memcpy(file, buffer + MD5_LENGTH, *size);

    uint8_t checksum[MD5_LENGTH];
    md5_hash((uint8_t *)file, *size, checksum);
    if (memcmp(checksum, buffer, MD5_LENGTH) != 0) {
        free(buffer);
        free(file);
        return result_error("ChecksumCorruptError", "Checksum for requested file '%s' does not match. The file is corrupt!", path);
    }

    *out = file;
    free(buffer);
    return result_no_error();
}

result_t fs_save(const char *path, const char *buffer, fs_size_t size) {
    FILE *f = fopen(path, "wb");
    if (!f)
        return result_error("FileOpenError", "Call to fopen on requested file '%s' failed.", path);

    if (fwrite(buffer, size, 1, f) < 0)
        return result_error("FileWriteError", "The requested file '%s' was unable to be written to.", path);
    fclose(f);

    return result_no_error();
}

result_t fs_save_checksum(const char *path, const char *buffer, fs_size_t size) {
    char *f_buffer = malloc(MD5_LENGTH + size);
    md5_hash((uint8_t *)buffer, size, (uint8_t *)f_buffer);
    memcpy(f_buffer + MD5_LENGTH, buffer, size);
    result_t res = fs_save(path, f_buffer, MD5_LENGTH + size);
    free(f_buffer);
    return res;
}

result_t fs_size(const char *path, fs_size_t *out) {
    FILE *f = fopen(path, "rb");
    if (!f)
        return result_error("FileOpenError", "Call to fopen on requested file '%s' failed.", path);

    fseek(f, 0, SEEK_END);
    *out = (fs_size_t)ftell(f);
    fclose(f);

    return result_no_error();
}

bool fs_exists(const char *path) {
    fs_size_t size;
    if (fs_size(path, &size).is_error || size < 0)
        return false;
    return true;
}
