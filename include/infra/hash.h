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

#ifndef JUST_INFRA_HASH_H
#define JUST_INFRA_HASH_H

/**
 * @file hash.h
 * @brief SHA-256 hash utilities for JUST VCS
 *
 * Provides functions to compute SHA-256 hashes, convert between
 * raw bytes and hexadecimal string representations, and parse
 * hashes from hex strings.
 */

#include <stddef.h> // size_t for sizes and counts

#define HASH_SIZE 32     // SHA-256 = 32 bytes
#define HASH_HEX_SIZE 65 // 64 caractères hex + '\0'

/**
 * @brief SHA-256 hash representation
 *
 * Stores the raw 32 bytes of a SHA-256 hash.
 */
typedef struct {
    unsigned char data[HASH_SIZE]; /**< Raw hash bytes */
} hash_t;

/**
 * @brief Compute the SHA-256 hash of a data buffer.
 *
 * @param data Pointer to input data.
 * @param len Length of the input data in bytes.
 * @param out Pointer to hash_t structure to store the resulting hash.
 *
 * This function does nothing if `data` is NULL, `out` is NULL, or `len` is 0 (errno set).
 */
void hash_compute(const void *data, size_t len, hash_t *out);

/**
 * @brief Convert a SHA-256 hash to a hexadecimal string.
 *
 * @param hash Pointer to the hash_t structure containing the hash.
 * @param hex_str Output buffer to store the hexadecimal string.
 *                Must be at least HASH_HEX_SIZE bytes.
 *
 * The resulting string is null-terminated and contains 64 hex characters.
 * This function does nothing if `hash` or `hex_str` is NULL (errno set).
 */
void hash_to_hex(const hash_t *hash, char *hex_str);

/**
 * @brief Parse a SHA-256 hash from a hexadecimal string.
 *
 * @param hex_str Input hexadecimal string (64 characters, null-terminated).
 * @param out Pointer to hash_t structure to store the resulting hash.
 * @return 0 on success, -1 on failure (invalid length or non-hex characters).
 *
 * Converts a valid hexadecimal string into a SHA-256 hash.
 * Fails if `hex_str` or `out` is NULL, if the string length is not 64,
 * or if the string contains invalid characters.
 */
int hash_from_hex(const char *hex_str, hash_t *out);

/**
 * Compare deux hashs
 * @param a Premier hash
 * @param b Deuxième hash
 * @return 1 si égaux, 0 sinon
 */
int hash_equals(const hash_t *a, const hash_t *b);

/**
 * Copie un hash
 * @param dest Destination
 * @param src Source
 */
void hash_copy(hash_t *dest, const hash_t *src);

#endif // JUST_INFRA_HASH_H

