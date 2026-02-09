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

#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <errno.h>
#include <linux/limits.h>
#include <string.h>
#include <unistd.h>

#include "infra/path.h"

/* ============================================================================
 * path_join
 * ============================================================================
 */

Test(path_suite, join_basic)
{
    char *p = path_join("a", "b");
    cr_assert_str_eq(p, "a/b");
    free(p);
}

Test(path_suite, join_no_separator_needed)
{
    char *p = path_join("a/", "b");
    cr_assert_str_eq(p, "a/b");
    free(p);
}

Test(path_suite, join_empty_component)
{
    char *p = path_join("a", "");
    cr_assert_str_eq(p, "a/");
    free(p);
}

Test(path_suite, join_null)
{
    cr_assert_null(path_join(NULL, "b"));
    cr_assert_eq(errno, EINVAL);
}

/* ============================================================================
 * path_normalize
 * ============================================================================
 */

Test(path_suite, normalize_simple)
{
    char *p = path_normalize("a/b/c");
    cr_assert_str_eq(p, "a/b/c");
    free(p);
}

Test(path_suite, normalize_dot)
{
    char *p = path_normalize("./a/./b");
    cr_assert_str_eq(p, "a/b");
    free(p);
}

Test(path_suite, normalize_dotdot_relative)
{
    char *p = path_normalize("a/b/../c");
    cr_assert_str_eq(p, "a/c");
    free(p);
}

Test(path_suite, normalize_dotdot_absolute)
{
    char *p = path_normalize("/a/b/../c");
    cr_assert_str_eq(p, "/a/c");
    free(p);
}

Test(path_suite, normalize_root)
{
    char *p = path_normalize("/");
    cr_assert_str_eq(p, "/");
    free(p);
}

Test(path_suite, normalize_empty)
{
    char *p = path_normalize("");
    cr_assert_str_eq(p, ".");
    free(p);
}

Test(path_suite, normalize_null)
{
    cr_assert_null(path_normalize(NULL));
    cr_assert_eq(errno, EINVAL);
}

/* ============================================================================
 * path_basename
 * ============================================================================
 */

Test(path_suite, basename_file)
{
    char *b = path_basename("a/b/c.txt");
    cr_assert_str_eq(b, "c.txt");
    free(b);
}

Test(path_suite, basename_dir)
{
    char *b = path_basename("a/b/c/");
    cr_assert_str_eq(b, "c");
    free(b);
}

Test(path_suite, basename_root)
{
    char *b = path_basename("/");
    cr_assert_str_eq(b, "/");
    free(b);
}

Test(path_suite, basename_dot)
{
    char *b = path_basename(".");
    cr_assert_str_eq(b, ".");
    free(b);
}

Test(path_suite, basename_null) { cr_assert_null(path_basename(NULL)); }

/* ============================================================================
 * path_dirname
 * ============================================================================
 */

Test(path_suite, dirname_simple)
{
    char *d = path_dirname("a/b/c");
    cr_assert_str_eq(d, "a/b");
    free(d);
}

Test(path_suite, dirname_trailing_slash)
{
    char *d = path_dirname("a/b/c/");
    cr_assert_str_eq(d, "a/b");
    free(d);
}

Test(path_suite, dirname_file)
{
    char *d = path_dirname("file");
    cr_assert_str_eq(d, ".");
    free(d);
}

Test(path_suite, dirname_root)
{
    char *d = path_dirname("/");
    cr_assert_str_eq(d, "/");
    free(d);
}

Test(path_suite, dirname_absolute)
{
    char *d = path_dirname("/a");
    cr_assert_str_eq(d, "/");
    free(d);
}

Test(path_suite, dirname_empty)
{
    char *d = path_dirname("");
    cr_assert_str_eq(d, ".");
    free(d);
}

Test(path_suite, dirname_null)
{
    cr_assert_null(path_dirname(NULL));
    cr_assert_eq(errno, EINVAL);
}

/* ============================================================================
 * path_is_absolute
 * ============================================================================
 */

Test(path_suite, is_absolute_true) { cr_assert_eq(path_is_absolute("/a/b"), 1); }

Test(path_suite, is_absolute_false) { cr_assert_eq(path_is_absolute("a/b"), 0); }

Test(path_suite, is_absolute_null) { cr_assert_eq(path_is_absolute(NULL), 0); }

/* ============================================================================
 * path_absolute
 * ============================================================================
 */

Test(path_suite, absolute_relative)
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    char *p = path_absolute("test");
    cr_assert_not_null(p);
    cr_assert(strncmp(p, cwd, strlen(cwd)) == 0);
    free(p);
}

Test(path_suite, absolute_absolute)
{
    char *p = path_absolute("/a/b");
    cr_assert_str_eq(p, "/a/b");
    free(p);
}

Test(path_suite, absolute_dot)
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    char *p = path_absolute(".");
    cr_assert_str_eq(p, cwd);
    free(p);
}

Test(path_suite, absolute_null)
{
    cr_assert_null(path_absolute(NULL));
    cr_assert_eq(errno, EINVAL);
}

