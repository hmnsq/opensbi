/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#شامل <sbi/sbi_unit_test.h>
#شامل <sbi/sbi_types.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_ubsan.h>

#define UBSAN_EXPECT_FIRES(test, stmt)                                    \
	انجامبده {                                                              \
		unsigned long _before = sbi_ubsan_report_شمار;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_شمار, _before); \
	} وقتی (0)

ایستا تهی test_ubsan_add_overflow(ساختار sbiunit_test_case *test)
{
	فرار صحیح a = 0x7FFFFFFF; //INT_MAx
	فرار صحیح b = 1;
	فرار صحیح c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(تهی)c;
}

ایستا تهی test_ubsan_sub_overflow(ساختار sbiunit_test_case *test)
{
	فرار صحیح a = 0x80000000; //INT_MIN
	فرار صحیح b = 1;
	فرار صحیح c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(تهی)c;
}

ایستا تهی test_ubsan_mul_overflow(ساختار sbiunit_test_case *test)
{
	فرار صحیح a = 0x7FFFFFFF;
	فرار صحیح b = 2;
	فرار صحیح c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(تهی)c;
}

ایستا تهی test_ubsan_divrem(ساختار sbiunit_test_case *test)
{
	فرار صحیح a = 10;
	فرار صحیح b = 0;
	فرار صحیح c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(تهی)c;
}

ایستا تهی test_ubsan_oob(ساختار sbiunit_test_case *test)
{
	فرار صحیح idx = 5;
	صحیح arr[3]	 = { 1, 2, 3 };
	فرار صحیح val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(تهی)val;
}

ایستا تهی test_ubsan_shift_too_large(ساختار sbiunit_test_case *test)
{
	فرار unsigned long val = 1;
	فرار صحیح shift	   = 64;
	فرار unsigned long res;
	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(تهی)res;
}

ایستا تهی test_ubsan_shift_negative(ساختار sbiunit_test_case *test)
{
	فرار صحیح val   = 1;
	فرار صحیح shift = -1;
	فرار صحیح res;

	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(تهی)res;
}

ایستا تهی test_ubsan_load_invalid_bool(ساختار sbiunit_test_case *test)
{
	فرار نویسه bool_val = 5;
	فرار bool *b_ptr   = (bool *)&bool_val;
	فرار صحیح taken     = 0;
	UBSAN_EXPECT_FIRES(test, اگر (*b_ptr) taken = 1);
	(تهی)taken;
}

ایستا تهی test_ubsan_pointer_overflow(ساختار sbiunit_test_case *test)
{
	فرار uintptr_t base = 0xFFFFFFFFFFFFFFFEUL;
	فرار نویسه *ptr	= (نویسه *)base;
	فرار نویسه *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(تهی)res;
}

ایستا ساختار sbiunit_test_case ubsan_tests[] = {
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
