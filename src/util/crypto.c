#include "crypto.h"
#include <md5.h>

uint32_t jhash(const char *buffer, uint32_t size) {
    uint32_t hash = 0;
    for (uint64_t i = 0; i < size; ++i) {
        hash += buffer[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

uint32_t jhash_str(const char *buffer) {
    uint32_t hash = 0;
    for (uint64_t i = 0; buffer[i] != '\0'; ++i) {
        hash += buffer[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

void md5_hash(uint8_t *buffer, uint32_t size, uint8_t *out) {
    MD5Context ctx;
    md5Init(&ctx);
    md5Update(&ctx, buffer, size);
    md5Finalize(&ctx);
    memcpy(out, ctx.digest, MD5_LENGTH);
}