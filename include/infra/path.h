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

#ifndef JUST_INFRA_PATH_H
#define JUST_INFRA_PATH_H

/**
 * Join two path components
 * @param base Base path
 * @param component Component to append
 * @return Newly allocated joined path (caller must free)
 */
char *path_join(const char *base, const char *component);

/**
 * Normalize a path (remove '.', '..', '//')
 * @param path Path to normalize
 * @return Newly allocated normalized path (caller must free)
 */
char *path_normalize(const char *path);

/**
 * Get basename of a path (last component)
 * @param path Full path
 * @return Pointer to basename (inside path, do not free)
 */
char *path_basename(const char *path);

/**
 * Get dirname of a path (all but last component)
 * @param path Full path
 * @return Newly allocated dirname (caller must free)
 */
char *path_dirname(const char *path);

/**
 * Check if path is absolute
 * @param path Path to check
 * @return 1 if absolute, 0 otherwise
 */
int path_is_absolute(const char *path);

/**
 * Get absolute path
 * @param path Relative or absolute path
 * @return Newly allocated absolute path (caller must free)
 */
char *path_absolute(const char *path);

#endif // JUST_INFRA_PATH_H

