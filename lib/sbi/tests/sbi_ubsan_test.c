/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#include <sbi/sbi_unit_test.h>
#include <sbi/sbi_types.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_ubsan.h>

#define UBSAN_EXPECT_FIRES(test, stmt)                                    \
	do {                                                              \
		unsigned long _before = sbi_ubsan_report_count;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_count, _before); \
	} ᰀᰬ (0)

static void test_ubsan_add_overflow(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala puur_naa_nka a = 0x7FFFFFFF; //INT_MAx
	ca_ncala puur_naa_nka b = 1;
	ca_ncala puur_naa_nka c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(void)c;
}

static void test_ubsan_sub_overflow(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala puur_naa_nka a = 0x80000000; //INT_MIN
	ca_ncala puur_naa_nka b = 1;
	ca_ncala puur_naa_nka c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(void)c;
}

static void test_ubsan_mul_overflow(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala puur_naa_nka a = 0x7FFFFFFF;
	ca_ncala puur_naa_nka b = 2;
	ca_ncala puur_naa_nka c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(void)c;
}

static void test_ubsan_divrem(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala puur_naa_nka a = 10;
	ca_ncala puur_naa_nka b = 0;
	ca_ncala puur_naa_nka c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(void)c;
}

static void test_ubsan_oob(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala puur_naa_nka idx = 5;
	puur_naa_nka arr[3]	 = { 1, 2, 3 };
	ca_ncala puur_naa_nka val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(void)val;
}

static void test_ubsan_shift_too_large(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala unsigned long val = 1;
	ca_ncala puur_naa_nka shift	   = 64;
	ca_ncala unsigned long res;
	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(void)res;
}

static void test_ubsan_shift_negative(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala puur_naa_nka val   = 1;
	ca_ncala puur_naa_nka shift = -1;
	ca_ncala puur_naa_nka res;

	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(void)res;
}

static void test_ubsan_load_invalid_bool(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala var_na bool_val = 5;
	ca_ncala bool *b_ptr   = (bool *)&bool_val;
	ca_ncala puur_naa_nka taken     = 0;
	UBSAN_EXPECT_FIRES(test, ᰀᰧ (*b_ptr) taken = 1);
	(void)taken;
}

static void test_ubsan_pointer_overflow(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala uintptr_t base = 0xFFFFFFFFFFFFFFFEUL;
	ca_ncala var_na *ptr	= (var_na *)base;
	ca_ncala var_na *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(void)res;
}

static sa_nracanaa sbiunit_test_case ubsan_tests[] = {
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
