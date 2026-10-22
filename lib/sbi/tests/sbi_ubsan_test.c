/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#অন্তর্ভাব <sbi/sbi_unit_test.h>
#অন্তর্ভাব <sbi/sbi_types.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_ubsan.h>

#define UBSAN_EXPECT_FIRES(test, stmt)                                    \
	করো {                                                              \
		unsigned long _before = sbi_ubsan_report_গুনতি;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_গুনতি, _before); \
	} যখন (0)

স্থির শূন্য test_ubsan_add_overflow(কাঠামো sbiunit_test_case *test)
{
	অস্থির পূর্ণসংখ্যা a = 0x7FFFFFFF; //INT_MAx
	অস্থির পূর্ণসংখ্যা b = 1;
	অস্থির পূর্ণসংখ্যা c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(শূন্য)c;
}

স্থির শূন্য test_ubsan_sub_overflow(কাঠামো sbiunit_test_case *test)
{
	অস্থির পূর্ণসংখ্যা a = 0x80000000; //INT_MIN
	অস্থির পূর্ণসংখ্যা b = 1;
	অস্থির পূর্ণসংখ্যা c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(শূন্য)c;
}

স্থির শূন্য test_ubsan_mul_overflow(কাঠামো sbiunit_test_case *test)
{
	অস্থির পূর্ণসংখ্যা a = 0x7FFFFFFF;
	অস্থির পূর্ণসংখ্যা b = 2;
	অস্থির পূর্ণসংখ্যা c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(শূন্য)c;
}

স্থির শূন্য test_ubsan_divrem(কাঠামো sbiunit_test_case *test)
{
	অস্থির পূর্ণসংখ্যা a = 10;
	অস্থির পূর্ণসংখ্যা b = 0;
	অস্থির পূর্ণসংখ্যা c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(শূন্য)c;
}

স্থির শূন্য test_ubsan_oob(কাঠামো sbiunit_test_case *test)
{
	অস্থির পূর্ণসংখ্যা idx = 5;
	পূর্ণসংখ্যা arr[3]	 = { 1, 2, 3 };
	অস্থির পূর্ণসংখ্যা val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(শূন্য)val;
}

স্থির শূন্য test_ubsan_shift_too_large(কাঠামো sbiunit_test_case *test)
{
	অস্থির unsigned long val = 1;
	অস্থির পূর্ণসংখ্যা shift	   = 64;
	অস্থির unsigned long res;
	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(শূন্য)res;
}

স্থির শূন্য test_ubsan_shift_negative(কাঠামো sbiunit_test_case *test)
{
	অস্থির পূর্ণসংখ্যা val   = 1;
	অস্থির পূর্ণসংখ্যা shift = -1;
	অস্থির পূর্ণসংখ্যা res;

	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(শূন্য)res;
}

স্থির শূন্য test_ubsan_load_invalid_bool(কাঠামো sbiunit_test_case *test)
{
	অস্থির অক্ষর bool_val = 5;
	অস্থির bool *b_ptr   = (bool *)&bool_val;
	অস্থির পূর্ণসংখ্যা taken     = 0;
	UBSAN_EXPECT_FIRES(test, যদি (*b_ptr) taken = 1);
	(শূন্য)taken;
}

স্থির শূন্য test_ubsan_pointer_overflow(কাঠামো sbiunit_test_case *test)
{
	অস্থির uintptr_t base = 0xFFFFFFFFFFFFFFFEUL;
	অস্থির অক্ষর *ptr	= (অক্ষর *)base;
	অস্থির অক্ষর *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(শূন্য)res;
}

স্থির কাঠামো sbiunit_test_case ubsan_tests[] = {
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
