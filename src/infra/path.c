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

#include "infra/path.h"
#include <errno.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *path_join(const char *base, const char *component)
{
    size_t base_len = 0;
    size_t component_len = 0;
    size_t total_len = 0;
    unsigned char need_separator = 0;
    char *result;

    if (!base || !component) {
        errno = EINVAL;
        return NULL;
    }
    base_len = strlen(base);
    component_len = strlen(component);
    need_separator = (base_len > 0 && base[base_len - 1] != '/');
    total_len = base_len + component_len + (need_separator ? 1 : 0) + 1;
    result = malloc(sizeof(*result) * total_len);
    if (result == NULL) {
        return NULL;
    }
    strcpy(result, base);
    if (need_separator) {
        strcat(result, "/");
    }
    strcat(result, component);
    return result;
}

char *path_normalize(const char *path)
{
    size_t depth = 0;
    char *tmp;
    char *token;
    char *stack[PATH_MAX];
    size_t stack_len = 0;
    char *result;
    char *p_result;
    size_t is_absolute = 0;

    if (!path) {
        errno = EINVAL;
        return NULL;
    }
    is_absolute = (path[0] == '/');
    tmp = strdup(path);
    token = strtok(tmp, "/");
    while (token) {
        if (strcmp(token, ".") == 0) {
        } else if (strcmp(token, "..") == 0) {
            if (depth > 0 && strcmp(stack[depth - 1], "..") != 0) {
                depth--;
            } else if (!is_absolute) {
                stack[depth++] = token;
            }
        } else {
            stack[depth++] = token;
        }
        token = strtok(NULL, "/");
    }
    if (depth == 0) {
        free(tmp);
        return strdup(is_absolute ? "/" : ".");
    }
    stack_len = is_absolute ? 1 : 0;
    for (size_t i = 0; i < depth; i++) {
        stack_len += strlen(stack[i]) + (i + 1 < depth ? 1 : 0);
    }
    result = malloc(sizeof(*result) * (stack_len + 1));
    if (!result) {
        free(tmp);
        return NULL;
    }
    p_result = result;
    if (is_absolute) {
        *p_result++ = '/';
    }
    for (size_t i = 0; i < depth; i++) {
        size_t len = strlen(stack[i]);

        memcpy(p_result, stack[i], len);
        p_result += len;
        if (i + 1 < depth) {
            *p_result++ = '/';
        }
    }
    *p_result = '\0';
    free(tmp);
    return result;
}

char *path_basename(const char *path)
{
    char *norm;
    const char *base;
    char *result;

    if (!path) {
        return NULL;
    }
    if (*path == '\0') {
        return strdup("");
    }
    norm = path_normalize(path);
    if (!norm) {
        return NULL;
    }
    base = strrchr(norm, '/');
    if (base) {
        base++;
    } else {
        base = norm;
    }
    if (*norm == '/' && *(norm + 1) == '\0') {
        result = strdup(norm);
        free(norm);
        return result;
    }
    result = strdup(base);
    if (!result) {
        return NULL;
    }
    free(norm);
    return result;
}

char *path_dirname(const char *path)
{
    char *tmp;
    char *end;
    char *slash;
    size_t len;

    if (!path) {
        errno = EINVAL;
        return NULL;
    }
    if (*path == '\0') {
        return strdup(".");
    }
    len = strlen(path);
    tmp = malloc(sizeof(*tmp) * (len + 1));
    if (!tmp) {
        errno = ENOMEM;
        return NULL;
    }
    strcpy(tmp, path);
    end = tmp + len - 1;
    while (end > tmp && *end == '/') {
        *end-- = '\0';
    }
    if (tmp[0] == '/' && tmp[1] == '\0') {
        return tmp;
    }
    slash = strrchr(tmp, '/');
    if (!slash) {
        free(tmp);
        return strdup(".");
    }
    while (slash > tmp && *slash == '/') {
        slash--;
    }
    *(slash + 1) = '\0';
    if (tmp[0] == '\0') {
        tmp[0] = '/';
        tmp[1] = '\0';
    }
    return tmp;
}

int path_is_absolute(const char *path)
{
    if (!path) {
        return 0;
    }
    return path[0] == '/';
}

char *path_absolute(const char *path)
{
    char cwd[PATH_MAX];
    char *joined;
    char *normalized;

    if (!path) {
        errno = EINVAL;
        return NULL;
    }
    if (path_is_absolute(path)) {
        normalized = path_normalize(path);
        return normalized;
    }
    if (!getcwd(cwd, sizeof(cwd))) {
        return NULL;
    }
    joined = path_join(cwd, path);
    if (!joined) {
        return NULL;
    }
    normalized = path_normalize(joined);
    free(joined);
    return normalized;
}

