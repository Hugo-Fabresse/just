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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "infra/fs.h"

// Helper to create unique test directory name
static char test_dir[256];
static char test_file[256];

void setup(void)
{
    // Create TRULY unique directory name using PID and address
    snprintf(test_dir, sizeof(test_dir), "test_dir_%d_%p", getpid(), (void *)&test_dir);
    snprintf(test_file, sizeof(test_file), "%s/test_file.txt", test_dir);

    // Only remove THIS specific test directory
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "rm -rf %s 2>/dev/null", test_dir);
    system(cmd);

    fs_mkdir(test_dir);
}

void teardown(void)
{
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "rm -rf %s 2>/dev/null", test_dir);
    system(cmd);
}

// Cleanup all test directories at the very end
void final_cleanup(void)
{
    system("rm -rf test_dir_* recursive_dir_* nested_test* existing_dir_* deep_test* 2>/dev/null");
}

// =============================================================================
// fs_mkdir Tests
// =============================================================================

Test(fs_suite, mkdir_success, .init = setup, .fini = teardown)
{
    char new_dir[256];
    snprintf(new_dir, sizeof(new_dir), "%s_new", test_dir);
    cr_assert_eq(fs_mkdir(new_dir), 0, "mkdir should succeed");
    cr_assert_eq(fs_is_dir(new_dir), 1, "Directory should exist");

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "rm -rf %s 2>/dev/null", new_dir);
    system(cmd);
}

Test(fs_suite, mkdir_already_exists, .init = setup, .fini = teardown)
{
    cr_assert_neq(fs_mkdir(test_dir), 0, "mkdir on existing directory should fail");
}

Test(fs_suite, mkdir_null_path) { cr_assert_neq(fs_mkdir(NULL), 0, "mkdir with NULL should fail"); }

// =============================================================================
// fs_mkdir_recursive Tests
// =============================================================================

Test(fs_suite, mkdir_recursive_simple, .fini = final_cleanup)
{
    char dir[256];
    snprintf(dir, sizeof(dir), "recursive_dir_test1_%d", getpid());

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "rm -rf %s 2>/dev/null", dir);
    system(cmd);

    cr_assert_eq(fs_mkdir_recursive(dir), 0, "Should create simple directory");
    cr_assert_eq(fs_is_dir(dir), 1, "Directory should exist");

    system(cmd);
}

Test(fs_suite, mkdir_recursive_nested, .fini = final_cleanup)
{
    char base[256];
    char dir[256];
    snprintf(base, sizeof(base), "nested_test2_%d", getpid());
    snprintf(dir, sizeof(dir), "%s/dir/structure", base);

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "rm -rf %s 2>/dev/null", base);
    system(cmd);

    cr_assert_eq(fs_mkdir_recursive(dir), 0, "Should create nested directories");
    cr_assert_eq(fs_is_dir(base), 1, "Parent should exist");

    char intermediate[256];
    snprintf(intermediate, sizeof(intermediate), "%s/dir", base);
    cr_assert_eq(fs_is_dir(intermediate), 1, "Intermediate should exist");
    cr_assert_eq(fs_is_dir(dir), 1, "Deepest should exist");

    system(cmd);
}

Test(fs_suite, mkdir_recursive_already_exists, .fini = final_cleanup)
{
    char dir[256];
    snprintf(dir, sizeof(dir), "existing_dir_test3_%d", getpid());

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "rm -rf %s 2>/dev/null", dir);
    system(cmd);

    cr_assert_eq(fs_mkdir_recursive(dir), 0, "First creation should succeed");
    cr_assert_eq(fs_mkdir_recursive(dir), 0, "Second creation should succeed");

    system(cmd);
}

Test(fs_suite, mkdir_recursive_partial_exists, .init = setup, .fini = teardown)
{
    char level1[256];
    snprintf(level1, sizeof(level1), "%s/level1", test_dir);
    fs_mkdir(level1);

    char deep_path[256];
    snprintf(deep_path, sizeof(deep_path), "%s/level1/level2/level3", test_dir);

    cr_assert_eq(fs_mkdir_recursive(deep_path), 0, "Should handle partial existing path");
    cr_assert_eq(fs_is_dir(deep_path), 1, "Deep path should exist");
}

Test(fs_suite, mkdir_recursive_deep, .fini = final_cleanup)
{
    char base[256];
    char dir[256];
    snprintf(base, sizeof(base), "deep_test4_%d", getpid());
    snprintf(dir, sizeof(dir), "%s/level1/level2/level3", base);

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "rm -rf %s 2>/dev/null", base);
    system(cmd);

    cr_assert_eq(fs_mkdir_recursive(dir), 0, "Should create deep path");
    cr_assert_eq(fs_is_dir(dir), 1, "Deep directory should exist");

    system(cmd);
}

Test(fs_suite, mkdir_recursive_null_path)
{
    cr_assert_eq(fs_mkdir_recursive(NULL), EINVAL, "NULL path should return EINVAL");
}

Test(fs_suite, mkdir_recursive_current_dir)
{
    cr_assert_eq(fs_mkdir_recursive("."), 0, "Current directory '.' should succeed");
}

Test(fs_suite, mkdir_recursive_root) { cr_assert_eq(fs_mkdir_recursive("/"), 0, "Root directory '/' should succeed"); }

// =============================================================================
// fs_exists Tests
// =============================================================================

Test(fs_suite, exists_directory, .init = setup, .fini = teardown)
{
    cr_assert_eq(fs_exists(test_dir), 1, "Directory should exist");
}

Test(fs_suite, exists_file, .init = setup, .fini = teardown)
{
    fs_write_file(test_file, "test", 4);
    cr_assert_eq(fs_exists(test_file), 1, "File should exist");
}

Test(fs_suite, exists_non_existent)
{
    cr_assert_eq(fs_exists("non_existent_xyz123"), 0, "Non-existent path should not exist");
}

Test(fs_suite, exists_null_path) { cr_assert_eq(fs_exists(NULL), EINVAL, "NULL path should return EINVAL"); }

// =============================================================================
// fs_is_dir Tests
// =============================================================================

Test(fs_suite, is_dir_valid, .init = setup, .fini = teardown)
{
    cr_assert_eq(fs_is_dir(test_dir), 1, "Should identify directory");
}

Test(fs_suite, is_dir_on_file, .init = setup, .fini = teardown)
{
    fs_write_file(test_file, "test", 4);
    cr_assert_eq(fs_is_dir(test_file), 0, "Should not identify file as directory");
}

Test(fs_suite, is_dir_non_existent)
{
    uint8_t result = fs_is_dir("non_existent_dir_xyz");
    cr_assert_neq(result, 1, "Non-existent should not be directory");
}

Test(fs_suite, is_dir_null_path) { cr_assert_eq(fs_is_dir(NULL), EINVAL, "NULL should return EINVAL"); }

Test(fs_suite, is_dir_root) { cr_assert_eq(fs_is_dir("/"), 1, "Root should be directory"); }

Test(fs_suite, is_dir_current) { cr_assert_eq(fs_is_dir("."), 1, "Current dir should be directory"); }

// =============================================================================
// fs_is_file Tests
// =============================================================================

Test(fs_suite, is_file_valid, .init = setup, .fini = teardown)
{
    fs_write_file(test_file, "test", 4);
    cr_assert_eq(fs_is_file(test_file), 1, "Should identify file");
}

Test(fs_suite, is_file_on_directory, .init = setup, .fini = teardown)
{
    cr_assert_eq(fs_is_file(test_dir), 0, "Should not identify directory as file");
}

Test(fs_suite, is_file_non_existent)
{
    uint8_t result = fs_is_file("non_existent_file_xyz.txt");
    cr_assert_neq(result, 1, "Non-existent should not be file");
}

Test(fs_suite, is_file_null_path) { cr_assert_eq(fs_is_file(NULL), EINVAL, "NULL should return EINVAL"); }

Test(fs_suite, is_file_special_device) { cr_assert_eq(fs_is_file("/dev/null"), 0, "/dev/null is not regular file"); }

// =============================================================================
// fs_write_file Tests
// =============================================================================

Test(fs_suite, write_file_basic, .init = setup, .fini = teardown)
{
    const char *content = "Hello, JUST!";
    size_t size = strlen(content);

    cr_assert_eq(fs_write_file(test_file, content, size), 0, "Write should succeed");

    FILE *f = fopen(test_file, "rb");
    cr_assert_not_null(f, "File should exist");

    char buffer[256];
    size_t read_size = fread(buffer, 1, sizeof(buffer), f);
    buffer[read_size] = '\0';
    fclose(f);

    cr_assert_eq(read_size, size, "Size should match");
    cr_assert_str_eq(buffer, content, "Content should match");
}

Test(fs_suite, write_file_empty, .init = setup, .fini = teardown)
{
    cr_assert_eq(fs_write_file(test_file, "", 0), 0, "Empty write should succeed");

    FILE *f = fopen(test_file, "rb");
    cr_assert_not_null(f, "Empty file should exist");

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);

    cr_assert_eq(size, 0, "File should be empty");
}

Test(fs_suite, write_file_binary, .init = setup, .fini = teardown)
{
    unsigned char data[] = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0x00, 0x42};
    size_t size = sizeof(data);

    cr_assert_eq(fs_write_file(test_file, (const char *)data, size), 0, "Binary write should succeed");

    FILE *f = fopen(test_file, "rb");
    cr_assert_not_null(f, "File should exist");

    unsigned char buffer[256];
    size_t read_size = fread(buffer, 1, sizeof(buffer), f);
    fclose(f);

    cr_assert_eq(read_size, size, "Size should match");
    cr_assert_eq(memcmp(buffer, data, size), 0, "Binary data should match");
}

Test(fs_suite, write_file_with_nulls, .init = setup, .fini = teardown)
{
    unsigned char data[] = {0x48, 0x00, 0x65, 0x00, 0x6C, 0x00};
    size_t size = sizeof(data);

    cr_assert_eq(fs_write_file(test_file, (const char *)data, size), 0, "Write with nulls should succeed");

    char *read_buf = NULL;
    size_t read_size = 0;
    cr_assert_eq(fs_read_file(test_file, &read_buf, &read_size), 0, "Read should succeed");
    cr_assert_eq(read_size, size, "Size should match");
    cr_assert_eq(memcmp(read_buf, data, size), 0, "Data with nulls should match");
    free(read_buf);
}

Test(fs_suite, write_file_overwrite, .init = setup, .fini = teardown)
{
    const char *content1 = "First content here";
    const char *content2 = "Second";

    cr_assert_eq(fs_write_file(test_file, content1, strlen(content1)), 0, "First write should succeed");
    cr_assert_eq(fs_write_file(test_file, content2, strlen(content2)), 0, "Overwrite should succeed");

    char *read_buf = NULL;
    size_t read_size = 0;
    cr_assert_eq(fs_read_file(test_file, &read_buf, &read_size), 0, "Read should succeed");
    cr_assert_eq(read_size, strlen(content2), "Size should match second");
    cr_assert_str_eq(read_buf, content2, "Should contain only second content");
    free(read_buf);
}

Test(fs_suite, write_file_newlines, .init = setup, .fini = teardown)
{
    const char *content = "Line 1\nLine 2\nLine 3\n";
    size_t size = strlen(content);

    cr_assert_eq(fs_write_file(test_file, content, size), 0, "Write with newlines should succeed");

    char *read_buf = NULL;
    size_t read_size = 0;
    cr_assert_eq(fs_read_file(test_file, &read_buf, &read_size), 0, "Read should succeed");
    cr_assert_str_eq(read_buf, content, "Content with newlines should match");
    free(read_buf);
}

Test(fs_suite, write_file_large, .init = setup, .fini = teardown)
{
    size_t size = 1024 * 1024; // 1MB
    char *large = malloc(size);
    cr_assert_not_null(large, "Should allocate");

    for (size_t i = 0; i < size; i++) {
        large[i] = (char)(i % 256);
    }

    cr_assert_eq(fs_write_file(test_file, large, size), 0, "Large write should succeed");

    FILE *f = fopen(test_file, "rb");
    cr_assert_not_null(f, "File should exist");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fclose(f);

    cr_assert_eq((size_t)fsize, size, "Size should match");
    free(large);
}

Test(fs_suite, write_file_invalid_path)
{
    const char *invalid = "/root/no_permission/file.txt";
    uint8_t result = fs_write_file(invalid, "test", 4);
    cr_assert_neq(result, 0, "Invalid path should fail");
}

Test(fs_suite, write_file_null_path)
{
    uint8_t result = fs_write_file(NULL, "test", 4);
    cr_assert_neq(result, 0, "NULL path should fail");
}

// =============================================================================
// fs_read_file Tests
// =============================================================================

Test(fs_suite, read_file_basic, .init = setup, .fini = teardown)
{
    const char *content = "Hello, read test!";
    size_t size = strlen(content);

    fs_write_file(test_file, content, size);

    char *read_buf = NULL;
    size_t read_size = 0;

    cr_assert_eq(fs_read_file(test_file, &read_buf, &read_size), 0, "Read should succeed");
    cr_assert_eq(read_size, size, "Size should match");
    cr_assert_str_eq(read_buf, content, "Content should match");
    free(read_buf);
}

Test(fs_suite, read_file_non_existent)
{
    char *read_buf = NULL;
    size_t read_size = 0;

    cr_assert_neq(fs_read_file("non_existent_xyz.txt", &read_buf, &read_size), 0, "Should fail");
}

Test(fs_suite, read_file_null_path)
{
    char *read_buf = NULL;
    size_t read_size = 0;

    cr_assert_eq(fs_read_file(NULL, &read_buf, &read_size), EINVAL, "NULL path should return EINVAL");
}

Test(fs_suite, read_file_null_content, .init = setup, .fini = teardown)
{
    size_t read_size = 0;
    cr_assert_eq(fs_read_file(test_file, NULL, &read_size), EINVAL, "NULL content should return EINVAL");
}

Test(fs_suite, read_file_null_size, .init = setup, .fini = teardown)
{
    char *read_buf = NULL;
    cr_assert_eq(fs_read_file(test_file, &read_buf, NULL), EINVAL, "NULL size should return EINVAL");
}

Test(fs_suite, write_read_roundtrip, .init = setup, .fini = teardown)
{
    const char *content = "Roundtrip !@#$%^&*()";
    size_t size = strlen(content);

    cr_assert_eq(fs_write_file(test_file, content, size), 0, "Write should succeed");

    char *read_buf = NULL;
    size_t read_size = 0;
    cr_assert_eq(fs_read_file(test_file, &read_buf, &read_size), 0, "Read should succeed");
    cr_assert_eq(read_size, size, "Size should match");
    cr_assert_str_eq(read_buf, content, "Content should match");
    free(read_buf);
}

// =============================================================================
// fs_list_dir Tests
// =============================================================================

Test(fs_suite, list_dir_with_files, .init = setup, .fini = teardown)
{
    fs_write_file(test_file, "test1", 5);

    char file2[256];
    snprintf(file2, sizeof(file2), "%s/file2.txt", test_dir);
    fs_write_file(file2, "test2", 5);

    char file3[256];
    snprintf(file3, sizeof(file3), "%s/file3.txt", test_dir);
    fs_write_file(file3, "test3", 5);

    size_t count = 0;
    char **entries = fs_list_dir(test_dir, &count);

    cr_assert_not_null(entries, "Should return entries");
    cr_assert_eq(count, 3, "Should have 3 files");

    for (size_t i = 0; i < count; i++) {
        free(entries[i]);
    }
    free(entries);
}

Test(fs_suite, list_dir_with_subdirs, .init = setup, .fini = teardown)
{
    fs_write_file(test_file, "test", 4);

    char subdir[256];
    snprintf(subdir, sizeof(subdir), "%s/subdir", test_dir);
    fs_mkdir(subdir);

    char file_in_sub[256];
    snprintf(file_in_sub, sizeof(file_in_sub), "%s/file.txt", subdir);
    fs_write_file(file_in_sub, "test", 4);

    size_t count = 0;
    char **entries = fs_list_dir(test_dir, &count);

    cr_assert_not_null(entries, "Should return entries");
    cr_assert_eq(count, 2, "Should have 2 entries (file + subdir)");

    for (size_t i = 0; i < count; i++) {
        free(entries[i]);
    }
    free(entries);
}

Test(fs_suite, list_dir_empty, .init = setup, .fini = teardown)
{
    size_t count = 0;
    char **entries = fs_list_dir(test_dir, &count);

    cr_assert_not_null(entries, "Should return array for empty dir");
    cr_assert_eq(count, 0, "Should have 0 entries");

    free(entries);
}

Test(fs_suite, list_dir_no_dots, .init = setup, .fini = teardown)
{
    fs_write_file(test_file, "test", 4);

    char file2[256];
    snprintf(file2, sizeof(file2), "%s/file2.txt", test_dir);
    fs_write_file(file2, "test", 4);

    size_t count = 0;
    char **entries = fs_list_dir(test_dir, &count);

    cr_assert_not_null(entries, "Should return entries");

    for (size_t i = 0; i < count; i++) {
        cr_assert_neq(strcmp(entries[i], "."), 0, "Should not include '.'");
        cr_assert_neq(strcmp(entries[i], ".."), 0, "Should not include '..'");
        free(entries[i]);
    }
    free(entries);
}

Test(fs_suite, list_dir_non_existent)
{
    size_t count = 0;
    char **entries = fs_list_dir("non_existent_xyz", &count);

    cr_assert_null(entries, "Should return NULL");
}

Test(fs_suite, list_dir_null_path)
{
    size_t count = 0;
    char **entries = fs_list_dir(NULL, &count);

    cr_assert_null(entries, "Should return NULL");
    cr_assert_eq(errno, EINVAL, "errno should be EINVAL");
}

Test(fs_suite, list_dir_null_count)
{
    char **entries = fs_list_dir(test_dir, NULL);

    cr_assert_null(entries, "Should return NULL");
    cr_assert_eq(errno, EINVAL, "errno should be EINVAL");
}

