/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_unit_test.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_types.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ubsan.h>

#define UBSAN_EXPECT_FIRES(test, stmt)                                    \
	ಕುರು {                                                              \
		unsigned long _before = sbi_ubsan_report_ಎಣಿಕೆ;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_ಎಣಿಕೆ, _before); \
	} ಯಾವತ್ (0)

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_add_overflow(ಸಂರಚನಾ sbiunit_test_case *test)
{
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ a = 0x7FFFFFFF; //INT_MAx
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ b = 1;
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(ಶೂನ್ಯ)c;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_sub_overflow(ಸಂರಚನಾ sbiunit_test_case *test)
{
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ a = 0x80000000; //INT_MIN
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ b = 1;
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(ಶೂನ್ಯ)c;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_mul_overflow(ಸಂರಚನಾ sbiunit_test_case *test)
{
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ a = 0x7FFFFFFF;
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ b = 2;
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(ಶೂನ್ಯ)c;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_divrem(ಸಂರಚನಾ sbiunit_test_case *test)
{
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ a = 10;
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ b = 0;
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(ಶೂನ್ಯ)c;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_oob(ಸಂರಚನಾ sbiunit_test_case *test)
{
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ idx = 5;
	ಪೂರ್ಣಾಙ್ಕ arr[3]	 = { 1, 2, 3 };
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(ಶೂನ್ಯ)val;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_shift_too_large(ಸಂರಚನಾ sbiunit_test_case *test)
{
	ಚಞ್ಚಲ unsigned long val = 1;
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ shift	   = 64;
	ಚಞ್ಚಲ unsigned long res;
	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(ಶೂನ್ಯ)res;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_shift_negative(ಸಂರಚನಾ sbiunit_test_case *test)
{
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ val   = 1;
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ shift = -1;
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ res;

	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(ಶೂನ್ಯ)res;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_load_invalid_bool(ಸಂರಚನಾ sbiunit_test_case *test)
{
	ಚಞ್ಚಲ ವರ್ಣ bool_val = 5;
	ಚಞ್ಚಲ bool *b_ptr   = (bool *)&bool_val;
	ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ taken     = 0;
	UBSAN_EXPECT_FIRES(test, ಯದಿ (*b_ptr) taken = 1);
	(ಶೂನ್ಯ)taken;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_pointer_overflow(ಸಂರಚನಾ sbiunit_test_case *test)
{
	ಚಞ್ಚಲ uintptr_t base = 0xFFFFFFFFFFFFFFFEUL;
	ಚಞ್ಚಲ ವರ್ಣ *ptr	= (ವರ್ಣ *)base;
	ಚಞ್ಚಲ ವರ್ಣ *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(ಶೂನ್ಯ)res;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbiunit_test_case ubsan_tests[] = {
	SBIUNIT_TEST_CASE(test_ubsan_add_overflow),
	SBIUNIT_TEST_CASE(test_ubsan_sub_overflow),
	SBIUNIT_TEST_CASE(test_ubsan_mul_overflow),
	SBIUNIT_TEST_CASE(test_ubsan_divrem),
	SBIUNIT_TEST_CASE(test_ubsan_oob),
	SBIUNIT_TEST_CASE(test_ubsan_shift_too_large),
	SBIUNIT_TEST_CASE(test_ubsan_shift_negative),
	SBIUNIT_TEST_CASE(test_ubsan_load_invalid_bool),
	SBIUNIT_TEST_CASE(test_ubsan_pointer_overflow),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(ubsan_test_suite, ubsan_tests);
