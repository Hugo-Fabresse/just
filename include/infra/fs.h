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

#ifndef JUST_INFRA_FS_H
#define JUST_INFRA_FS_H

/**
 * @file fs.h
 * @brief Filesystem utilities for just VCS
 *
 * Provides functions for creating directories, reading/writing files,
 * checking file types, and listing directory contents.
 */

#include <stddef.h> // size_t for sizes and counts
#include <stdint.h> // uint8_t for return codes

/* ==========================================================================
 * Directories
 * ========================================================================== */

/**
 * @brief Create a directory.
 *
 * @param path Path of the directory to create.
 * @return 0 on success, or errno value on failure.
 */
uint8_t fs_mkdir(const char *path);

/**
 * @brief Recursively create a directory and all its parent directories.
 *
 * @param path Path of the directory to create.
 * @return 0 on success, EEXIST if directory already exists, or errno value on other failures.
 */
uint8_t fs_mkdir_recursive(const char *path);

/* ==========================================================================
 * File IO
 * ========================================================================== */

/**
 * @brief Read the entire content of a file into memory.
 *
 * @param path Path of the file to read.
 * @param content Output pointer to allocated buffer containing file content.
 *                The caller must free() this buffer.
 * @param size Output size of the read content.
 * @return 0 on success, or errno value on failure (e.g., ENOMEM, EIO, EINVAL).
 */
uint8_t fs_read_file(const char *path, char **content, size_t *size);

/**
 * @brief Write data to a file.
 *
 * @param path Path of the file to write.
 * @param content Buffer containing the data to write.
 * @param size Number of bytes to write.
 * @return 0 on success, or errno value on failure (e.g., EIO, EINVAL).
 */
uint8_t fs_write_file(const char *path, const char *content, size_t size);

/* ==========================================================================
 * Verification / type checks
 * ========================================================================== */

/**
 * @brief Check if a file or directory exists.
 *
 * @param path Path to check.
 * @return 1 if exists, 0 if not.
 */
uint8_t fs_exists(const char *path);

/**
 * @brief Check if a path is a directory.
 *
 * @param path Path to check.
 * @return 1 if directory, 0 if not, or errno value on error.
 */
uint8_t fs_is_dir(const char *path);

/**
 * @brief Check if a path is a regular file.
 *
 * @param path Path to check.
 * @return 1 if regular file, 0 if not, or errno value on error.
 */
uint8_t fs_is_file(const char *path);

/* ==========================================================================
 * Directory listing
 * ========================================================================== */

/**
 * @brief List all entries in a directory (excluding "." and "..").
 *
 * @param path Directory path.
 * @param count Output: number of entries.
 * @return NULL on failure (errno set), otherwise a NULL-terminated array of strings.
 *         Each string and the array itself must be free()d by the caller.
 */
char **fs_list_dir(const char *path, size_t *count);

#endif // JUST_INFRA_FS_H

