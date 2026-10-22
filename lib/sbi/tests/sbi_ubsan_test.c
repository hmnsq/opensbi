/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_unit_test.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_types.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ubsan.h>

#define UBSAN_EXPECT_FIRES(test, stmt)                                    \
	କର {                                                              \
		unsigned long _before = sbi_ubsan_report_ଗଣନା;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_ଗଣନା, _before); \
	} ଯେତେବେଳେ (0)

ସ୍ଥିର ଶୂନ୍ୟ test_ubsan_add_overflow(ସଂରଚନା sbiunit_test_case *test)
{
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା a = 0x7FFFFFFF; //INT_MAx
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା b = 1;
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(ଶୂନ୍ୟ)c;
}

ସ୍ଥିର ଶୂନ୍ୟ test_ubsan_sub_overflow(ସଂରଚନା sbiunit_test_case *test)
{
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା a = 0x80000000; //INT_MIN
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା b = 1;
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(ଶୂନ୍ୟ)c;
}

ସ୍ଥିର ଶୂନ୍ୟ test_ubsan_mul_overflow(ସଂରଚନା sbiunit_test_case *test)
{
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା a = 0x7FFFFFFF;
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା b = 2;
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(ଶୂନ୍ୟ)c;
}

ସ୍ଥିର ଶୂନ୍ୟ test_ubsan_divrem(ସଂରଚନା sbiunit_test_case *test)
{
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା a = 10;
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା b = 0;
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(ଶୂନ୍ୟ)c;
}

ସ୍ଥିର ଶୂନ୍ୟ test_ubsan_oob(ସଂରଚନା sbiunit_test_case *test)
{
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 5;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା arr[3]	 = { 1, 2, 3 };
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(ଶୂନ୍ୟ)val;
}

ସ୍ଥିର ଶୂନ୍ୟ test_ubsan_shift_too_large(ସଂରଚନା sbiunit_test_case *test)
{
	ଅସ୍ଥିର unsigned long val = 1;
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା shift	   = 64;
	ଅସ୍ଥିର unsigned long res;
	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(ଶୂନ୍ୟ)res;
}

ସ୍ଥିର ଶୂନ୍ୟ test_ubsan_shift_negative(ସଂରଚନା sbiunit_test_case *test)
{
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା val   = 1;
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା shift = -1;
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା res;

	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(ଶୂନ୍ୟ)res;
}

ସ୍ଥିର ଶୂନ୍ୟ test_ubsan_load_invalid_bool(ସଂରଚନା sbiunit_test_case *test)
{
	ଅସ୍ଥିର ଅକ୍ଷର bool_val = 5;
	ଅସ୍ଥିର bool *b_ptr   = (bool *)&bool_val;
	ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା taken     = 0;
	UBSAN_EXPECT_FIRES(test, ଯଦି (*b_ptr) taken = 1);
	(ଶୂନ୍ୟ)taken;
}

ସ୍ଥିର ଶୂନ୍ୟ test_ubsan_pointer_overflow(ସଂରଚନା sbiunit_test_case *test)
{
	ଅସ୍ଥିର uintptr_t base = 0xFFFFFFFFFFFFFFFEUL;
	ଅସ୍ଥିର ଅକ୍ଷର *ptr	= (ଅକ୍ଷର *)base;
	ଅସ୍ଥିର ଅକ୍ଷର *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(ଶୂନ୍ୟ)res;
}

ସ୍ଥିର ସଂରଚନା sbiunit_test_case ubsan_tests[] = {
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
