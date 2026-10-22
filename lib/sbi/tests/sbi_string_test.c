/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Chen Pei <cp0613@linux.alibaba.com>
 */

#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_unit_test.h>

/* Test data for string functions */
ساكن ثابت حرف test_str1[] = "Hello, World!";
ساكن ثابت حرف test_str2[] = "Hello, World!";
ساكن ثابت حرف test_str3[] = "Hello, OpenSBI!";
ساكن ثابت حرف test_str_خالي_جايون[] = "";
ساكن ثابت حرف test_str_long[] = "This is a very long string for testing purposes";
ساكن ثابت حرف test_str_short[] = "Hi";
ساكن ثابت حرف test_str_with_char[] = "Testing character search";

ساكن فراغ string_strcmp_test(بنية sbiunit_test_case *test)
{
	/* Same strings should return 0 */
	SBIUNIT_EXPECT_EQ(test, sbi_strcmp(test_str1, test_str2), 0);

	/* Different strings should return non-zero */
	SBIUNIT_EXPECT_NE(test, sbi_strcmp(test_str1, test_str3), 0);

	/* Empty strings */
	SBIUNIT_EXPECT_EQ(test, sbi_strcmp(test_str_خالي_جايون, test_str_خالي_جايون), 0);

	/* One empty, one not */
	صحيح result1 = sbi_strcmp(test_str1, test_str_خالي_جايون);
	صحيح result2 = sbi_strcmp(test_str_خالي_جايون, test_str1);
	SBIUNIT_EXPECT_NE(test, result1, 0);
	SBIUNIT_EXPECT_NE(test, result2, 0);
	SBIUNIT_EXPECT_EQ(test, result1, -result2);

	/* Different lengths */
	SBIUNIT_EXPECT_NE(test, sbi_strcmp(test_str1, test_str_short), 0);
}

ساكن فراغ string_strncmp_test(بنية sbiunit_test_case *test)
{
	/* Same strings with full length */
	SBIUNIT_EXPECT_EQ(test, sbi_strncmp(test_str1, test_str2, sbi_strlen(test_str1)), 0);

	/* Same strings with partial length */
	SBIUNIT_EXPECT_EQ(test, sbi_strncmp(test_str1, test_str2, 5), 0);

	/* Different strings with limited comparison */
	SBIUNIT_EXPECT_EQ(test, sbi_strncmp(test_str1, test_str3, 7), 0);  /* "Hello, " matches */
	SBIUNIT_EXPECT_NE(test, sbi_strncmp(test_str1, test_str3, 8), 0);  /* "Hello, " vs "Hello, " + 'W' vs 'O' */

	/* Count is 0 - should always return 0 */
	SBIUNIT_EXPECT_EQ(test, sbi_strncmp(test_str1, test_str3, 0), 0);

	/* One string shorter than count */
	SBIUNIT_EXPECT_NE(test, sbi_strncmp(test_str_short, test_str1, 20), 0);
}

ساكن فراغ string_strlen_test(بنية sbiunit_test_case *test)
{
	/* Test known lengths */
	SBIUNIT_EXPECT_EQ(test, sbi_strlen(test_str1), 13UL);
	SBIUNIT_EXPECT_EQ(test, sbi_strlen(test_str_خالي_جايون), 0UL);
	SBIUNIT_EXPECT_EQ(test, sbi_strlen("A"), 1UL);
	SBIUNIT_EXPECT_EQ(test, sbi_strlen(test_str_long), 47UL);
	SBIUNIT_EXPECT_EQ(test, sbi_strlen(test_str_short), 2UL);
}

ساكن فراغ string_strnlen_test(بنية sbiunit_test_case *test)
{
	/* Test with count larger than string length */
	SBIUNIT_EXPECT_EQ(test, sbi_strnlen(test_str1, 20), 13UL);

	/* Test with count smaller than string length */
	SBIUNIT_EXPECT_EQ(test, sbi_strnlen(test_str1, 5), 5UL);

	/* Test with count equal to string length */
	SBIUNIT_EXPECT_EQ(test, sbi_strnlen(test_str1, 13), 13UL);

	/* Test empty string */
	SBIUNIT_EXPECT_EQ(test, sbi_strnlen(test_str_خالي_جايون, 10), 0UL);

	/* Test with count 0 */
	SBIUNIT_EXPECT_EQ(test, sbi_strnlen(test_str1, 0), 0UL);
}

ساكن فراغ string_strcpy_test(بنية sbiunit_test_case *test)
{
	حرف dest[50];

	/* Copy string and verify */
	sbi_strcpy(dest, test_str1);
	SBIUNIT_EXPECT_STREQ(test, dest, test_str1, 14);  /* 13 chars + null terminator */

	/* Copy empty string */
	sbi_strcpy(dest, test_str_خالي_جايون);
	SBIUNIT_EXPECT_EQ(test, sbi_strlen(dest), 0UL);

	/* Copy short string */
	sbi_strcpy(dest, test_str_short);
	SBIUNIT_EXPECT_STREQ(test, dest, test_str_short, 3);  /* 2 chars + null terminator */
}

ساكن فراغ string_strncpy_test(بنية sbiunit_test_case *test)
{
	حرف dest[50];

	/* Basic functionality test */
	sbi_strncpy(dest, "Hello", 6);
	SBIUNIT_EXPECT_STREQ(test, dest, "Hello", 6);

	/* Copy with larger count */
	sbi_memset(dest, 'X', حجم(dest));  /* Fill with 'X' to see padding */
	sbi_strncpy(dest, "Hi", 10);
	SBIUNIT_EXPECT_STREQ(test, dest, "Hi", 3);  /* "Hi" + null terminator */
	/* Check that remaining positions are properly handled */
	SBIUNIT_EXPECT_EQ(test, dest[2], '\0');  /* Should be null-terminated */
	
	/* CRITICAL TEST: Source string length equals count - NO null termination added */
	حرف بفر[10];
	ثابت حرف *src1 = "Hello";  // 5 chars
	sbi_memset(بفر, 'Z', 10);  // Fill with 'Z' to detect non-termination
	sbi_strncpy(بفر, src1, 5);  // Copies exactly 5 chars: 'H','e','l','l','o' - NO null terminator!
	
	/* Verify the copied content */
	SBIUNIT_EXPECT_EQ(test, بفر[0], 'H');
	SBIUNIT_EXPECT_EQ(test, بفر[1], 'e');
	SBIUNIT_EXPECT_EQ(test, بفر[2], 'l');
	SBIUNIT_EXPECT_EQ(test, بفر[3], 'l');
	SBIUNIT_EXPECT_EQ(test, بفر[4], 'o');
	/* buffer[5] is NOT guaranteed to be null due to the bug - it might still be 'Z' */
	
	/* CRITICAL TEST: Source string length greater than count - NO null termination added */
	ثابت حرف *src2 = "HelloWorld";  // 10 chars
	sbi_memset(بفر, 'Y', 10);  // Fill with 'Y' to detect non-termination
	sbi_strncpy(بفر, src2, 5);  // Copies "Hello", but NO null terminator added!
	
	/* Verify the first 5 copied chars */
	SBIUNIT_EXPECT_EQ(test, بفر[0], 'H');
	SBIUNIT_EXPECT_EQ(test, بفر[1], 'e');
	SBIUNIT_EXPECT_EQ(test, بفر[2], 'l');
	SBIUNIT_EXPECT_EQ(test, بفر[3], 'l');
	SBIUNIT_EXPECT_EQ(test, بفر[4], 'o');
	/* buffer[5] is NOT guaranteed to be null due to the bug - it might still be 'Y' */
	
	/* Safe case: source shorter than count - properly null-terminated */
	sbi_memset(بفر, 'X', 10);
	sbi_strncpy(بفر, "Hi", 10);  // Copies "Hi" and remaining spaces get nulls
	
	SBIUNIT_EXPECT_EQ(test, بفر[0], 'H');
	SBIUNIT_EXPECT_EQ(test, بفر[1], 'i');
	SBIUNIT_EXPECT_EQ(test, بفر[2], '\0');  /* Should be null-terminated */
}

ساكن فراغ string_strchr_test(بنية sbiunit_test_case *test)
{
	ثابت حرف *pos;

	/* Find existing character */
	pos = sbi_strchr(test_str1, 'W');
	SBIUNIT_EXPECT_NE(test, pos, عدم);
	إذا (pos != عدم) {
		SBIUNIT_EXPECT_EQ(test, pos - test_str1, 7);  /* 'W' is at index 7 */
	}

	/* Find first character */
	pos = sbi_strchr(test_str1, 'H');
	SBIUNIT_EXPECT_NE(test, pos, عدم);
	إذا (pos != عدم) {
		SBIUNIT_EXPECT_EQ(test, pos - test_str1, 0);  /* 'H' is at index 0 */
	}

	/* Find last character */
	pos = sbi_strchr(test_str1, '!');
	SBIUNIT_EXPECT_NE(test, pos, عدم);
	إذا (pos != عدم) {
		SBIUNIT_EXPECT_EQ(test, pos - test_str1, 12);  /* '!' is at index 12 */
	}

	/* Find non-existing character */
	pos = sbi_strchr(test_str1, 'X');
	SBIUNIT_EXPECT_EQ(test, pos, عدم);

	/* Find null terminator - according to standard, strchr should find null terminator */
	pos = sbi_strchr(test_str1, '\0');
	SBIUNIT_EXPECT_NE(test, pos, عدم);
	إذا (pos != عدم) {
		SBIUNIT_EXPECT_EQ(test, pos - test_str1, 13);  /* Null terminator at index 13 */
	}

	/* Find in empty string */
	pos = sbi_strchr(test_str_خالي_جايون, 'A');
	SBIUNIT_EXPECT_EQ(test, pos, عدم);
}

ساكن فراغ string_strrchr_test(بنية sbiunit_test_case *test)
{
	ثابت حرف *pos;

	/* Find last occurrence of character */
	pos = sbi_strrchr(test_str_with_char, 't');  /* Multiple 't's: "Test"ing charac"t"er search -> last 't' is at index 14 */
	SBIUNIT_EXPECT_NE(test, pos, عدم);
	إذا (pos != عدم) {
		SBIUNIT_EXPECT_EQ(test, pos - test_str_with_char, 14);  /* Last 't' at index 14 */
	}

	/* Find single occurrence */
	pos = sbi_strrchr(test_str1, 'W');
	SBIUNIT_EXPECT_NE(test, pos, عدم);
	إذا (pos != عدم) {
		SBIUNIT_EXPECT_EQ(test, pos - test_str1, 7);  /* 'W' at index 7 */
	}

	/* Find last character */
	pos = sbi_strrchr(test_str1, '!');
	SBIUNIT_EXPECT_NE(test, pos, عدم);
	إذا (pos != عدم) {
		SBIUNIT_EXPECT_EQ(test, pos - test_str1, 12);  /* '!' at index 12 */
	}

	/* Find non-existing character */
	pos = sbi_strrchr(test_str1, 'X');
	SBIUNIT_EXPECT_EQ(test, pos, عدم);

	/* Find in empty string */
	pos = sbi_strrchr(test_str_خالي_جايون, 'A');
	SBIUNIT_EXPECT_EQ(test, pos, عدم);
}

ساكن فراغ يادگيري_memset_test(بنية sbiunit_test_case *test)
{
	حرف بفر[20];

	/* Set all to 'A' */
	sbi_memset(بفر, 'A', 10);
	لكل (صحيح i = 0; i < 10; i++) {
		SBIUNIT_EXPECT_EQ(test, بفر[i], 'A');
	}

	/* Set with count 0 */
	sbi_memset(بفر, 'B', 0);
	/* Buffer should remain unchanged (not 'B') - depends on previous state */

	/* Set with different value */
	sbi_memset(بفر, 0, 5);  /* Null out first 5 bytes */
	لكل (صحيح i = 0; i < 5; i++) {
		SBIUNIT_EXPECT_EQ(test, بفر[i], 0);
	}
}

ساكن فراغ يادگيري_memcpy_test(بنية sbiunit_test_case *test)
{
	حرف dest[50];
	ثابت حرف *src = "memcpy test string";

	/* Copy string */
	sbi_memcpy(dest, src, sbi_strlen(src) + 1);  /* Include null terminator */
	SBIUNIT_EXPECT_STREQ(test, dest, src, sbi_strlen(src) + 1);

	/* Copy with specific size */
	sbi_memcpy(dest, src, 6);  /* Copy "memcpy" */
	SBIUNIT_EXPECT_STREQ(test, dest, "memcpy", 6);

	/* Copy 0 bytes */
	sbi_memcpy(dest, src, 0);  /* Should not change dest */
	SBIUNIT_EXPECT_STREQ(test, dest, "memcpy", 6);
}

ساكن فراغ يادگيري_memmove_test(بنية sbiunit_test_case *test)
{
	حرف بفر[50] = "This is a test string for memmove";

	/* Test overlapping copy - forward */
	sbi_strcpy(بفر, "abcdef");
	sbi_memmove(بفر + 2, بفر, 4);  /* Move "abcd" to position 2, result: "ababcd" */
	SBIUNIT_EXPECT_STREQ(test, بفر, "ababcd", 7);

	/* Test overlapping copy - backward */
	sbi_strcpy(بفر, "abcdef");
	sbi_memmove(بفر, بفر + 2, 4);  /* Move "cdef" to start, result: "cdefef" */
	SBIUNIT_EXPECT_STREQ(test, بفر, "cdefef", 7);

	/* Test non-overlapping copy */
	sbi_strcpy(بفر, "source");
	sbi_memmove(بفر + 10, بفر, 7);  /* Copy "source" + null to position 10 */
	SBIUNIT_EXPECT_STREQ(test, بفر, "source", 7);  /* Original string unchanged */
	SBIUNIT_EXPECT_STREQ(test, بفر + 10, "source", 7);  /* Copy at offset 10 */

	/* Test copy 0 bytes */
	sbi_memmove(بفر, بفر + 5, 0);  /* Should not change buffer */
	SBIUNIT_EXPECT_STREQ(test, بفر, "source", 7);
	SBIUNIT_EXPECT_STREQ(test, بفر + 10, "source", 7);
}

ساكن فراغ يادگيري_memcmp_test(بنية sbiunit_test_case *test)
{
	ثابت حرف *str1 = "compare";
	ثابت حرف *str2 = "compare";
	ثابت حرف *str3 = "comparf";
	ثابت حرف *str4 = "compare longer";

	/* Same strings */
	SBIUNIT_EXPECT_EQ(test, sbi_memcmp(str1, str2, 7), 0);

	/* Different strings */
	SBIUNIT_EXPECT_NE(test, sbi_memcmp(str1, str3, 7), 0);

	/* Compare with different lengths */
	SBIUNIT_EXPECT_EQ(test, sbi_memcmp(str1, str4, 7), 0);  /* First 7 chars match */
	SBIUNIT_EXPECT_NE(test, sbi_memcmp(str1, str4, 8), 0);  /* 8th char differs */
	
	/* Compare 0 bytes */
	SBIUNIT_EXPECT_EQ(test, sbi_memcmp(str1, str3, 0), 0);

	/* Compare empty regions */
	SBIUNIT_EXPECT_EQ(test, sbi_memcmp(str1, str1, 0), 0);
}

ساكن فراغ يادگيري_memchr_test(بنية sbiunit_test_case *test)
{
	ثابت حرف *str = "memory search test";
	فراغ *pos;

	/* Find existing character */
	pos = sbi_memchr(str, 's', sbi_strlen(str));
	SBIUNIT_EXPECT_NE(test, pos, عدم);
	إذا (pos != عدم) {
		SBIUNIT_EXPECT_EQ(test, (حرف*)pos - str, 7);  /* First 's' at index 7 */
	}

	/* Find character at specific position */
	pos = sbi_memchr(str, 'm', sbi_strlen(str));
	SBIUNIT_EXPECT_NE(test, pos, عدم);
	إذا (pos != عدم) {
		SBIUNIT_EXPECT_EQ(test, (حرف*)pos - str, 0);  /* 'm' at index 0 */
	}

	/* Find first occurrence of 't' character */
	pos = sbi_memchr(str, 't', sbi_strlen(str));
	SBIUNIT_EXPECT_NE(test, pos, عدم);
	إذا (pos != عدم) {
		SBIUNIT_EXPECT_EQ(test, (حرف*)pos - str, 14);  /* First 't' at index 14 */
	}

	/* Find non-existing character */
	pos = sbi_memchr(str, 'X', sbi_strlen(str));
	SBIUNIT_EXPECT_EQ(test, pos, عدم);

	/* Search with zero count */
	pos = sbi_memchr(str, 'm', 0);
	SBIUNIT_EXPECT_EQ(test, pos, عدم);
}

ساكن بنية sbiunit_test_case string_test_cases[] = {
	SBIUNIT_TEST_CASE(string_strcmp_test),
	SBIUNIT_TEST_CASE(string_strncmp_test),
	SBIUNIT_TEST_CASE(string_strlen_test),
	SBIUNIT_TEST_CASE(string_strnlen_test),
	SBIUNIT_TEST_CASE(string_strcpy_test),
	SBIUNIT_TEST_CASE(string_strncpy_test),
	SBIUNIT_TEST_CASE(string_strchr_test),
	SBIUNIT_TEST_CASE(string_strrchr_test),
	SBIUNIT_TEST_CASE(يادگيري_memset_test),
	SBIUNIT_TEST_CASE(يادگيري_memcpy_test),
	SBIUNIT_TEST_CASE(يادگيري_memmove_test),
	SBIUNIT_TEST_CASE(يادگيري_memcmp_test),
	SBIUNIT_TEST_CASE(يادگيري_memchr_test),
	SBIUNIT_پڇاڙي_CASE,
};

SBIUNIT_TEST_SUITE(string_test_suite, string_test_cases);
