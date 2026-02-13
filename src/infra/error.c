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

#include "infra/error.h" // Declaration of all error functions
#include <errno.h>       // Provides errno values for error reporting
#include <stddef.h>      // size_t for sizes and counts

static _Thread_local error_code_t last_error = ERR_OK;

typedef struct {
    const char *message;
    error_code_t code;
    int posix_errno;
} error_info_t;

static const error_info_t error_table[] = {
#define X(msg, code, err) {msg, code, err},
#include "infra/error.def"
#undef X
};

static const size_t error_table_len = sizeof(error_table) / sizeof(error_table[0]);

const char *error_string(error_code_t code)
{
    for (size_t i = 0; i < error_table_len; i++) {
        if (error_table[i].code == code) {
            return error_table[i].message;
        }
    }
    return "Unknow error";
}

void error_set(error_code_t code)
{
    last_error = code;
    for (size_t i = 0; i < error_table_len; i++) {
        if (error_table[i].code == code && error_table[i].posix_errno) {
            errno = error_table[i].posix_errno;
        }
    }
}

error_code_t error_get(void) { return last_error; }

void error_clear(void) { last_error = ERR_OK; }

