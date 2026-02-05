/*
 * JUST - A lightweight version control system
 * Copyright (C) 2026 Hugo Fabresse
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "infra/hash.h"  // Declaration of all fs functions
#include <ctype.h>       // isxdigit() to validate hexadecimal characters
#include <errno.h>       // Provides errno values for error reporting
#include <openssl/sha.h> // SHA256() for computing SHA-256 hash
#include <stdio.h>       // sprintf() for converting bytes to hex string
#include <string.h>      // strlen() and memcmp()

void hash_compute(const void *data, size_t len, hash_t *out)
{
    if (!data || !out || len == 0) {
        errno = EINVAL;
        return;
    }
    SHA256((const unsigned char *)data, len, out->data);
}

void hash_to_hex(const hash_t *hash, char *hex_str)
{
    if (!hash || !hex_str) {
        errno = EINVAL;
        return;
    }
    for (size_t i = 0; i < HASH_SIZE; i++) {
        sprintf(hex_str + (i * 2), "%02x", hash->data[i]);
    }
    hex_str[HASH_HEX_SIZE - 1] = '\0';
}

int hash_from_hex(const char *hex_str, hash_t *out)
{
    size_t len = 0;

    if (!hex_str || !out) {
        errno = EINVAL;
        return -1;
    }
    len = strlen(hex_str);
    if (len != HASH_HEX_SIZE - 1) {
        errno = EINVAL;
        return -1;
    }
    for (size_t i = 0; i < len; i++) {
        if (!isxdigit((unsigned char)hex_str[i])) {
            errno = EINVAL;
            return -1;
        }
    }
    for (size_t i = 0; i < HASH_SIZE; i++) {
        unsigned int byte;
        if (sscanf(hex_str + (i * 2), "%2x", &byte) != 1) {
            errno = EINVAL;
            return -1;
        }
        out->data[i] = (unsigned char)byte;
    }
    return 0;
}

int hash_equals(const hash_t *a, const hash_t *b)
{
    if (!a || !b) {
        return 0;
    }

    return memcmp(a->data, b->data, HASH_SIZE) == 0;
}

void hash_copy(hash_t *dest, const hash_t *src)
{
    if (!dest || !src) {
        return;
    }

    memcpy(dest->data, src->data, HASH_SIZE);
}

