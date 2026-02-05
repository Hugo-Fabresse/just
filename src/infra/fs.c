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

#include "infra/fs.h"     // Declaration of all fs functions
#include <dirent.h>       // DIR, opendir, readdir for directory listing
#include <errno.h>        // Provides errno values for error reporting
#include <libgen.h>       // dirname for recursive directory creation
#include <linux/limits.h> // PATH_MAX for maximum path length
#include <stdio.h>        // FILE*, fopen/fread/fwrite, fprintf for errors
#include <stdlib.h>       // malloc, free
#include <string.h>       // strcmp, strncpy, strdup
#include <sys/stat.h>     // mkdir and stat structures

// Creating directories
uint8_t fs_mkdir(const char *path)
{
    if (mkdir(path, 0755) != 0) {
        fprintf(stderr, "mkdir '%s': %s\n", path, strerror(errno));
        return errno;
    }
    return 0;
}

uint8_t fs_mkdir_recursive(const char *path)
{
    char tmp[PATH_MAX];
    const char *parent = NULL;

    if (!path) {
        return EINVAL;
    }
    strncpy(tmp, path, PATH_MAX);
    tmp[PATH_MAX - 1] = '\0';
    parent = dirname(tmp);
    if (parent && strcmp(parent, ".") != 0 && strcmp(parent, "/") != 0) {
        uint8_t res = fs_mkdir_recursive(parent);
        if (res != 0 && res != EEXIST) {
            return res;
        }
    }
    if (mkdir(path, 0755) != 0) {
        if (errno != EEXIST) {
            fprintf(stderr, "mkdir '%s' failed: %s\n", path, strerror(errno));
            return errno;
        }
    }
    return 0;
}

// Reading/writing files
uint8_t fs_read_file(const char *path, char **content, size_t *size)
{
    FILE *file = NULL;
    long len = 0;
    char *buffer = NULL;

    if (!path || !content || !size) {
        return EINVAL;
    }
    file = fopen(path, "rb");
    if (!file) {
        return errno;
    }
    fseek(file, 0, SEEK_END);
    len = ftell(file);
    rewind(file);
    buffer = malloc((sizeof(*buffer)) * (len + 1));
    if (!buffer) {
        fclose(file);
        return ENOMEM;
    }
    if (fread(buffer, 1, len, file) != (size_t)len) {
        free(buffer);
        fclose(file);
        return EIO;
    }
    buffer[len] = '\0';
    fclose(file);
    *content = buffer;
    *size = len;
    return 0;
}

uint8_t fs_write_file(const char *path, const char *content, size_t size)
{
    FILE *file = NULL;

    file = fopen(path, "wb");
    if (!file) {
        return errno;
    }
    if (fwrite(content, 1, size, file) != size) {
        fclose(file);
        return EIO;
    }
    fclose(file);
    return 0;
}

// verification
uint8_t fs_exists(const char *path)
{
    struct stat st;

    if (!path || !*path) {
        return EINVAL;
    }
    return (stat(path, &st) == 0) ? 1 : 0;
}

uint8_t fs_is_dir(const char *path)
{
    struct stat st;

    if (!path) {
        return EINVAL;
    }
    if (stat(path, &st) != 0) {
        return errno;
    }
    return S_ISDIR(st.st_mode);
}

uint8_t fs_is_file(const char *path)
{
    struct stat st;

    if (!path) {
        return EINVAL;
    }
    if (stat(path, &st) != 0) {
        return errno;
    }
    return S_ISREG(st.st_mode);
}

// Listing
char **fs_list_dir(const char *path, size_t *count)
{
    DIR *dir = NULL;
    const struct dirent *entry;
    size_t n = 0;
    char **list;

    if (!path || !count) {
        errno = EINVAL;
        return NULL;
    }
    dir = opendir(path);
    if (dir == NULL) {
        return NULL;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        n++;
    }
    closedir(dir);
    list = malloc(sizeof(*list) * (n + 1));
    if (!list) {
        errno = ENOMEM;
        return NULL;
    }
    list[n] = NULL;
    dir = opendir(path);
    if (dir == NULL) {
        free(list);
        return NULL;
    }
    n = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        list[n] = strdup(entry->d_name);
        if (list[n] == NULL) {
            errno = ENOMEM;
            for (size_t i = 0; i < n; i++) {
                free(list[i]);
            }
            free(list);
            closedir(dir);
            return NULL;
        }
        n++;
    }
    closedir(dir);
    *count = n;
    return list;
}

