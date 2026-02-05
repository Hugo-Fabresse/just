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
#include <string.h>
#include "infra/hash.h"

// =============================================================================
// hash_compute Tests
// =============================================================================

Test(hash_suite, compute_basic)
{
    const char *data = "Hello, JUST!";
    hash_t hash;

    hash_compute(data, strlen(data), &hash);

    // Verify hash is not empty
    int is_zero = 1;
    for (size_t i = 0; i < HASH_SIZE; i++) {
        if (hash.data[i] != 0) {
            is_zero = 0;
            break;
        }
    }
    cr_assert_eq(is_zero, 0, "Hash should not be all zeros");
}

Test(hash_suite, compute_deterministic)
{
    const char *data = "test data";
    hash_t hash1, hash2;

    hash_compute(data, strlen(data), &hash1);
    hash_compute(data, strlen(data), &hash2);

    cr_assert(hash_equals(&hash1, &hash2), "Same input should produce same hash");
}

Test(hash_suite, compute_large_data)
{
    // Test with large data
    char large_data[10000];
    memset(large_data, 'A', sizeof(large_data));

    hash_t hash;
    hash_compute(large_data, sizeof(large_data), &hash);

    // Verify hash is not empty
    int is_zero = 1;
    for (size_t i = 0; i < HASH_SIZE; i++) {
        if (hash.data[i] != 0) {
            is_zero = 0;
            break;
        }
    }
    cr_assert_eq(is_zero, 0, "Large data should produce non-zero hash");
}

Test(hash_suite, compute_null_data)
{
    hash_t hash;

    // Should not crash with NULL data
    hash_compute(NULL, 0, &hash);
}

Test(hash_suite, compute_null_with_nonzero_length)
{
    hash_t hash;

    // Should not crash with NULL and len > 0
    hash_compute(NULL, 100, &hash);
}

Test(hash_suite, compute_null_output)
{
    const char *data = "test";

    // Should not crash with NULL output
    hash_compute(data, 4, NULL);
}

// =============================================================================
// hash_to_hex Tests
// =============================================================================

Test(hash_suite, to_hex_conversion)
{
    hash_t hash = {0};
    // Known hash for "hello"
    const char *expected = "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824";

    const char *data = "hello";
    hash_compute(data, strlen(data), &hash);

    char hex[HASH_HEX_SIZE];
    hash_to_hex(&hash, hex);

    cr_assert_str_eq(hex, expected, "Hash hex representation should match");
}

Test(hash_suite, to_hex_all_bytes)
{
    hash_t hash;

    // Fill hash with known values
    for (size_t i = 0; i < HASH_SIZE; i++) {
        hash.data[i] = (unsigned char)i;
    }

    char hex[HASH_HEX_SIZE];
    hash_to_hex(&hash, hex);

    // Verify all bytes are converted correctly
    cr_assert_str_eq(hex, "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f",
                     "All bytes should be converted correctly");
}

Test(hash_suite, to_hex_high_bytes)
{
    hash_t hash;

    // Test with high bytes (>= 0x80)
    for (size_t i = 0; i < HASH_SIZE; i++) {
        hash.data[i] = 0xff - (unsigned char)i;
    }

    char hex[HASH_HEX_SIZE];
    hash_to_hex(&hash, hex);

    // Verify format
    cr_assert_eq(strlen(hex), 64, "Hex string should be 64 chars");
    cr_assert_eq(hex[0], 'f', "First char should be 'f'");
    cr_assert_eq(hex[1], 'f', "Second char should be 'f'");
}

Test(hash_suite, to_hex_null_hash)
{
    char hex[HASH_HEX_SIZE];

    // Should not crash
    hash_to_hex(NULL, hex);
}

Test(hash_suite, to_hex_null_output)
{
    hash_t hash;
    hash_compute("test", 4, &hash);

    // Should not crash
    hash_to_hex(&hash, NULL);
}

// =============================================================================
// hash_from_hex Tests
// =============================================================================

Test(hash_suite, from_hex_valid)
{
    const char *hex_str = "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824";
    hash_t hash;

    int result = hash_from_hex(hex_str, &hash);

    cr_assert_eq(result, 0, "Valid hex string should parse successfully");
    cr_assert_eq(hash.data[0], 0x2c, "First byte should be 0x2c");
    cr_assert_eq(hash.data[31], 0x24, "Last byte should be 0x24");
}

Test(hash_suite, from_hex_exact_length)
{
    // Test with exactly 64 characters
    const char *hex_str = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
    hash_t hash;

    int result = hash_from_hex(hex_str, &hash);

    cr_assert_eq(result, 0, "Exactly 64 chars should succeed");
    cr_assert_eq(hash.data[0], 0x01, "First byte should be 0x01");
    cr_assert_eq(hash.data[1], 0x23, "Second byte should be 0x23");
}

Test(hash_suite, from_hex_uppercase)
{
    // Test with uppercase letters
    const char *hex_str = "2CF24DBA5FB0A30E26E83B2AC5B9E29E1B161E5C1FA7425E73043362938B9824";
    hash_t hash;

    int result = hash_from_hex(hex_str, &hash);

    cr_assert_eq(result, 0, "Uppercase hex should be valid");
    cr_assert_eq(hash.data[0], 0x2c, "First byte should be 0x2c");
}

Test(hash_suite, from_hex_mixed_case)
{
    // Test with mixed upper and lowercase
    const char *hex_str = "2Cf24DbA5Fb0A30e26E83b2Ac5B9e29E1b161E5c1Fa7425E73043362938b9824";
    hash_t hash;

    int result = hash_from_hex(hex_str, &hash);

    cr_assert_eq(result, 0, "Mixed case hex should be valid");
}

Test(hash_suite, from_hex_all_zeros)
{
    const char *hex_str = "0000000000000000000000000000000000000000000000000000000000000000";
    hash_t hash;

    int result = hash_from_hex(hex_str, &hash);

    cr_assert_eq(result, 0, "All zeros should be valid");
    for (size_t i = 0; i < HASH_SIZE; i++) {
        cr_assert_eq(hash.data[i], 0, "All bytes should be zero");
    }
}

Test(hash_suite, from_hex_all_ones)
{
    const char *hex_str = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
    hash_t hash;

    int result = hash_from_hex(hex_str, &hash);

    cr_assert_eq(result, 0, "All 0xFF should be valid");
    for (size_t i = 0; i < HASH_SIZE; i++) {
        cr_assert_eq(hash.data[i], 0xff, "All bytes should be 0xFF");
    }
}

Test(hash_suite, from_hex_invalid_length_too_short)
{
    const char *hex_str = "2cf24dba5fb0a30e"; // Too short
    hash_t hash;

    int result = hash_from_hex(hex_str, &hash);

    cr_assert_eq(result, -1, "Invalid length should fail");
}

Test(hash_suite, from_hex_invalid_length_too_long)
{
    // Test with more than 64 characters
    const char *hex_str = "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824EXTRA";
    hash_t hash;

    int result = hash_from_hex(hex_str, &hash);

    cr_assert_eq(result, -1, "Too long string should fail");
}

Test(hash_suite, from_hex_invalid_chars_all)
{
    const char *hex_str = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
    hash_t hash;

    int result = hash_from_hex(hex_str, &hash);

    cr_assert_eq(result, -1, "Invalid characters should fail");
}

Test(hash_suite, from_hex_invalid_at_start)
{
    // Invalid character at start
    const char *hex_str = "xyz4dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824";
    hash_t hash;

    int result = hash_from_hex(hex_str, &hash);

    cr_assert_eq(result, -1, "Invalid char at start should fail");
}

Test(hash_suite, from_hex_invalid_at_middle)
{
    // Invalid character in middle
    const char *hex_str = "2cf24dba5fb0a30e26e83b2ac5b9e29e1bXYZ5c1fa7425e73043362938b9824";
    hash_t hash;

    int result = hash_from_hex(hex_str, &hash);

    cr_assert_eq(result, -1, "Invalid char in middle should fail");
}

Test(hash_suite, from_hex_invalid_at_end)
{
    // Invalid character at end
    const char *hex_str = "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b98XY";
    hash_t hash;

    int result = hash_from_hex(hex_str, &hash);

    cr_assert_eq(result, -1, "Invalid char at end should fail");
}

Test(hash_suite, from_hex_null_input)
{
    hash_t hash;

    int result = hash_from_hex(NULL, &hash);

    cr_assert_eq(result, -1, "NULL input should return error");
}

Test(hash_suite, from_hex_null_output)
{
    int result = hash_from_hex("2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824", NULL);

    cr_assert_eq(result, -1, "NULL output should return error");
}

// =============================================================================
// hash_equals Tests
// =============================================================================

Test(hash_suite, equals_identical)
{
    hash_t hash1, hash2;

    hash_compute("identical", 9, &hash1);
    hash_compute("identical", 9, &hash2);

    cr_assert(hash_equals(&hash1, &hash2), "Identical hashes should be equal");
}

Test(hash_suite, equals_different)
{
    hash_t hash1, hash2;

    hash_compute("data1", 5, &hash1);
    hash_compute("data2", 5, &hash2);

    cr_assert_not(hash_equals(&hash1, &hash2), "Different data should produce different hashes");
}

Test(hash_suite, equals_differ_at_start)
{
    hash_t hash1, hash2;

    memset(&hash1, 0, sizeof(hash1));
    memset(&hash2, 0, sizeof(hash2));

    hash1.data[0] = 0x01;
    hash2.data[0] = 0x02;

    cr_assert_not(hash_equals(&hash1, &hash2), "Hashes differing at start should not be equal");
}

Test(hash_suite, equals_differ_at_end)
{
    hash_t hash1, hash2;

    memset(&hash1, 0xff, sizeof(hash1));
    memset(&hash2, 0xff, sizeof(hash2));

    hash1.data[HASH_SIZE - 1] = 0xaa;
    hash2.data[HASH_SIZE - 1] = 0xbb;

    cr_assert_not(hash_equals(&hash1, &hash2), "Hashes differing at end should not be equal");
}

Test(hash_suite, equals_both_null)
{
    int result = hash_equals(NULL, NULL);
    cr_assert_eq(result, 0, "Both NULL should return 0");
}

Test(hash_suite, equals_first_null)
{
    hash_t hash;
    hash_compute("test", 4, &hash);

    int result = hash_equals(NULL, &hash);
    cr_assert_eq(result, 0, "First NULL should return 0");
}

Test(hash_suite, equals_second_null)
{
    hash_t hash;
    hash_compute("test", 4, &hash);

    int result = hash_equals(&hash, NULL);
    cr_assert_eq(result, 0, "Second NULL should return 0");
}

// =============================================================================
// hash_copy Tests
// =============================================================================

Test(hash_suite, copy_basic)
{
    hash_t src, dest;

    hash_compute("test", 4, &src);
    hash_copy(&dest, &src);

    cr_assert(hash_equals(&src, &dest), "Copied hash should be equal");
}

Test(hash_suite, copy_full_hash)
{
    hash_t src, dest;

    // Create hash with varied data
    for (size_t i = 0; i < HASH_SIZE; i++) {
        src.data[i] = (unsigned char)(i * 7 % 256);
    }

    hash_copy(&dest, &src);

    // Verify all bytes are copied
    for (size_t i = 0; i < HASH_SIZE; i++) {
        cr_assert_eq(dest.data[i], src.data[i], "Byte %zu should be copied", i);
    }
}

Test(hash_suite, copy_both_null)
{
    // Should not crash
    hash_copy(NULL, NULL);
}

Test(hash_suite, copy_dest_null)
{
    hash_t src;
    hash_compute("test", 4, &src);

    // Should not crash
    hash_copy(NULL, &src);
}

Test(hash_suite, copy_src_null)
{
    hash_t dest;

    // Should not crash
    hash_copy(&dest, NULL);
}

// =============================================================================
// Integration Tests
// =============================================================================

Test(hash_suite, roundtrip)
{
    const char *data = "roundtrip test";
    hash_t hash1, hash2;

    hash_compute(data, strlen(data), &hash1);

    char hex[HASH_HEX_SIZE];
    hash_to_hex(&hash1, hex);

    hash_from_hex(hex, &hash2);

    cr_assert(hash_equals(&hash1, &hash2), "Roundtrip should preserve hash");
}

Test(hash_suite, multiple_roundtrips)
{
    const char *data = "test data for multiple roundtrips";
    hash_t hash1, hash2, hash3;

    // First roundtrip
    hash_compute(data, strlen(data), &hash1);
    char hex1[HASH_HEX_SIZE];
    hash_to_hex(&hash1, hex1);
    hash_from_hex(hex1, &hash2);

    // Second roundtrip
    char hex2[HASH_HEX_SIZE];
    hash_to_hex(&hash2, hex2);
    hash_from_hex(hex2, &hash3);

    // All should be equal
    cr_assert(hash_equals(&hash1, &hash2), "First roundtrip should preserve hash");
    cr_assert(hash_equals(&hash2, &hash3), "Second roundtrip should preserve hash");
    cr_assert(hash_equals(&hash1, &hash3), "All hashes should be equal");
    cr_assert_str_eq(hex1, hex2, "Hex representations should match");
}

