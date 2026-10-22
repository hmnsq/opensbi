/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#অন্তর্ভাৱ <sbi/sbi_unit_test.h>
#অন্তর্ভাৱ <sbi/sbi_types.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_ubsan.h>

#define UBSAN_EXPECT_FIRES(test, stmt)                                    \
	কৰা {                                                              \
		unsigned long _before = sbi_ubsan_report_গুনতি;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_গুনতি, _before); \
	} যেতিয়ালৈ (0)

স্থির শূন্য test_ubsan_add_overflow(সংরচনা sbiunit_test_case *test)
{
	চঞ্চল পূর্ণাঙ্ক a = 0x7FFFFFFF; //INT_MAx
	চঞ্চল পূর্ণাঙ্ক b = 1;
	চঞ্চল পূর্ণাঙ্ক c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(শূন্য)c;
}

স্থির শূন্য test_ubsan_sub_overflow(সংরচনা sbiunit_test_case *test)
{
	চঞ্চল পূর্ণাঙ্ক a = 0x80000000; //INT_MIN
	চঞ্চল পূর্ণাঙ্ক b = 1;
	চঞ্চল পূর্ণাঙ্ক c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(শূন্য)c;
}

স্থির শূন্য test_ubsan_mul_overflow(সংরচনা sbiunit_test_case *test)
{
	চঞ্চল পূর্ণাঙ্ক a = 0x7FFFFFFF;
	চঞ্চল পূর্ণাঙ্ক b = 2;
	চঞ্চল পূর্ণাঙ্ক c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(শূন্য)c;
}

স্থির শূন্য test_ubsan_divrem(সংরচনা sbiunit_test_case *test)
{
	চঞ্চল পূর্ণাঙ্ক a = 10;
	চঞ্চল পূর্ণাঙ্ক b = 0;
	চঞ্চল পূর্ণাঙ্ক c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(শূন্য)c;
}

স্থির শূন্য test_ubsan_oob(সংরচনা sbiunit_test_case *test)
{
	চঞ্চল পূর্ণাঙ্ক idx = 5;
	পূর্ণাঙ্ক arr[3]	 = { 1, 2, 3 };
	চঞ্চল পূর্ণাঙ্ক val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(শূন্য)val;
}

স্থির শূন্য test_ubsan_shift_too_large(সংরচনা sbiunit_test_case *test)
{
	চঞ্চল unsigned long val = 1;
	চঞ্চল পূর্ণাঙ্ক shift	   = 64;
	চঞ্চল unsigned long res;
	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(শূন্য)res;
}

স্থির শূন্য test_ubsan_shift_negative(সংরচনা sbiunit_test_case *test)
{
	চঞ্চল পূর্ণাঙ্ক val   = 1;
	চঞ্চল পূর্ণাঙ্ক shift = -1;
	চঞ্চল পূর্ণাঙ্ক res;

	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(শূন্য)res;
}

স্থির শূন্য test_ubsan_load_invalid_bool(সংরচনা sbiunit_test_case *test)
{
	চঞ্চল ৱর্ণ bool_val = 5;
	চঞ্চল bool *b_ptr   = (bool *)&bool_val;
	চঞ্চল পূর্ণাঙ্ক taken     = 0;
	UBSAN_EXPECT_FIRES(test, যদি (*b_ptr) taken = 1);
	(শূন্য)taken;
}

স্থির শূন্য test_ubsan_pointer_overflow(সংরচনা sbiunit_test_case *test)
{
	চঞ্চল uintptr_t base = 0xFFFFFFFFFFFFFFFEUL;
	চঞ্চল ৱর্ণ *ptr	= (ৱর্ণ *)base;
	চঞ্চল ৱর্ণ *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(শূন্য)res;
}

স্থির সংরচনা sbiunit_test_case ubsan_tests[] = {
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
