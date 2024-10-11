//? septumfunk 2024
#pragma once
#include <stdint.h>
#include <stdbool.h>

#define MD5_LENGTH 16

/// Jenkin's Hash Function
uint32_t jhash(const char *buffer, uint32_t size);
/// Jenkin's Hash Function compatible with a null terminated string
uint32_t jhash_str(const char *buffer);

/// MD5 Hash Function.
/// Please allocate out before calling the function.
void md5_hash(uint8_t *buffer, uint32_t size, uint8_t *out);