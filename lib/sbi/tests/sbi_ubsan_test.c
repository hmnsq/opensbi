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
	} ໃນຂະນະທີ່ (0)

static void test_ubsan_add_overflow(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala ຈຳນວນເຕັມ a = 0x7FFFFFFF; //INT_MAx
	ca_ncala ຈຳນວນເຕັມ b = 1;
	ca_ncala ຈຳນວນເຕັມ c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(void)c;
}

static void test_ubsan_sub_overflow(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala ຈຳນວນເຕັມ a = 0x80000000; //INT_MIN
	ca_ncala ຈຳນວນເຕັມ b = 1;
	ca_ncala ຈຳນວນເຕັມ c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(void)c;
}

static void test_ubsan_mul_overflow(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala ຈຳນວນເຕັມ a = 0x7FFFFFFF;
	ca_ncala ຈຳນວນເຕັມ b = 2;
	ca_ncala ຈຳນວນເຕັມ c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(void)c;
}

static void test_ubsan_divrem(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala ຈຳນວນເຕັມ a = 10;
	ca_ncala ຈຳນວນເຕັມ b = 0;
	ca_ncala ຈຳນວນເຕັມ c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(void)c;
}

static void test_ubsan_oob(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala ຈຳນວນເຕັມ idx = 5;
	ຈຳນວນເຕັມ arr[3]	 = { 1, 2, 3 };
	ca_ncala ຈຳນວນເຕັມ val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(void)val;
}

static void test_ubsan_shift_too_large(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala unsigned long val = 1;
	ca_ncala ຈຳນວນເຕັມ shift	   = 64;
	ca_ncala unsigned long res;
	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(void)res;
}

static void test_ubsan_shift_negative(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala ຈຳນວນເຕັມ val   = 1;
	ca_ncala ຈຳນວນເຕັມ shift = -1;
	ca_ncala ຈຳນວນເຕັມ res;

	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(void)res;
}

static void test_ubsan_load_invalid_bool(sa_nracanaa sbiunit_test_case *test)
{
	ca_ncala var_na bool_val = 5;
	ca_ncala bool *b_ptr   = (bool *)&bool_val;
	ca_ncala ຈຳນວນເຕັມ taken     = 0;
	UBSAN_EXPECT_FIRES(test, ຖ້າ (*b_ptr) taken = 1);
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
