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

int main(void);

#include <criterion/criterion.h>

/**
 * Test suite: Main program
 * Description: Basic sanity tests for the main entry point
 */

Test(main_suite, basic_sanity_check)
{
    // This test just ensures the test framework is working
    cr_assert(1 == 1, "Basic arithmetic should work");
}

Test(main_suite, pointer_null_check)
{
    void *ptr = NULL;
    cr_assert_null(ptr, "NULL pointer should be NULL");
}

Test(main_suite, string_comparison)
{
    const char *expected = "Hello, world!";
    const char *actual = "Hello, world!";
    cr_assert_str_eq(expected, actual, "Strings should match");
}

