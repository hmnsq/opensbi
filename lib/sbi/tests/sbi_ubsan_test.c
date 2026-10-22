/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#અન્તર્ભાવ <sbi/sbi_unit_test.h>
#અન્તર્ભાવ <sbi/sbi_types.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_ubsan.h>

#define UBSAN_EXPECT_FIRES(test, stmt)                                    \
	કરો {                                                              \
		unsigned long _before = sbi_ubsan_report_ગણતરી;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_ગણતરી, _before); \
	} જ્યારે (0)

સ્થિર શૂન્ય test_ubsan_add_overflow(સંરચના sbiunit_test_case *test)
{
	અસ્થિર પૂર્ણાંક a = 0x7FFFFFFF; //INT_MAx
	અસ્થિર પૂર્ણાંક b = 1;
	અસ્થિર પૂર્ણાંક c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(શૂન્ય)c;
}

સ્થિર શૂન્ય test_ubsan_sub_overflow(સંરચના sbiunit_test_case *test)
{
	અસ્થિર પૂર્ણાંક a = 0x80000000; //INT_MIN
	અસ્થિર પૂર્ણાંક b = 1;
	અસ્થિર પૂર્ણાંક c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(શૂન્ય)c;
}

સ્થિર શૂન્ય test_ubsan_mul_overflow(સંરચના sbiunit_test_case *test)
{
	અસ્થિર પૂર્ણાંક a = 0x7FFFFFFF;
	અસ્થિર પૂર્ણાંક b = 2;
	અસ્થિર પૂર્ણાંક c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(શૂન્ય)c;
}

સ્થિર શૂન્ય test_ubsan_divrem(સંરચના sbiunit_test_case *test)
{
	અસ્થિર પૂર્ણાંક a = 10;
	અસ્થિર પૂર્ણાંક b = 0;
	અસ્થિર પૂર્ણાંક c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(શૂન્ય)c;
}

સ્થિર શૂન્ય test_ubsan_oob(સંરચના sbiunit_test_case *test)
{
	અસ્થિર પૂર્ણાંક idx = 5;
	પૂર્ણાંક arr[3]	 = { 1, 2, 3 };
	અસ્થિર પૂર્ણાંક val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(શૂન્ય)val;
}

સ્થિર શૂન્ય test_ubsan_shift_too_large(સંરચના sbiunit_test_case *test)
{
	અસ્થિર unsigned long val = 1;
	અસ્થિર પૂર્ણાંક shift	   = 64;
	અસ્થિર unsigned long res;
	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(શૂન્ય)res;
}

સ્થિર શૂન્ય test_ubsan_shift_negative(સંરચના sbiunit_test_case *test)
{
	અસ્થિર પૂર્ણાંક val   = 1;
	અસ્થિર પૂર્ણાંક shift = -1;
	અસ્થિર પૂર્ણાંક res;

	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(શૂન્ય)res;
}

સ્થિર શૂન્ય test_ubsan_load_invalid_bool(સંરચના sbiunit_test_case *test)
{
	અસ્થિર અક્ષર bool_val = 5;
	અસ્થિર bool *b_ptr   = (bool *)&bool_val;
	અસ્થિર પૂર્ણાંક taken     = 0;
	UBSAN_EXPECT_FIRES(test, જો (*b_ptr) taken = 1);
	(શૂન્ય)taken;
}

સ્થિર શૂન્ય test_ubsan_pointer_overflow(સંરચના sbiunit_test_case *test)
{
	અસ્થિર uintptr_t base = 0xFFFFFFFFFFFFFFFEUL;
	અસ્થિર અક્ષર *ptr	= (અક્ષર *)base;
	અસ્થિર અક્ષર *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(શૂન્ય)res;
}

સ્થિર સંરચના sbiunit_test_case ubsan_tests[] = {
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
