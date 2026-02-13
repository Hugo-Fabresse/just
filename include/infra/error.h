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

#ifndef JUST_INFRA_ERROR_H
#define JUST_INFRA_ERROR_H

/**
 * @file error.h
 * @brief Error handling utilities for JUST VCS
 *
 * Defines all error codes used in JUST, provides thread-local storage
 * for the last error, and functions to get human-readable strings,
 * set, get, and clear errors. Supports automatic mapping to POSIX errno
 * where applicable.
 */

/**
 * @brief Error codes for the JUST version control system.
 *
 * This enum defines all the possible error codes used in JUST.
 * Each code represents a specific error condition.
 */
typedef enum {
#define X(msg, code, err) code,
#include "infra/error.def"
#undef X
} error_code_t;

/**
 * @brief Get a human-readable string for an error code.
 *
 * @param code The error code to convert.
 * @return A static string describing the error. Do not free.
 */
const char *error_string(error_code_t code);

/**
 * @brief Set the current error code.
 *
 * This sets the thread-local last error and updates errno if
 * a POSIX equivalent is defined.
 *
 * @param code The error code to set.
 */
void error_set(error_code_t code);

/**
 * @brief Retrieve the current thread-local error code.
 *
 * @return The last error code set with error_set(), or ERR_OK
 *         if no error has occurred.
 */
error_code_t error_get(void);

/**
 * @brief Retrieve the current thread-local error code.
 *
 * @return The last error code set with error_set(), or ERR_OK
 *         if no error has occurred.
 */
void error_clear(void);

#endif // JUST_INFRA_ERROR_H

