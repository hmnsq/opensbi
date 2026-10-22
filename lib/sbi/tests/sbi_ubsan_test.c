/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#அந்தர்பாவ <sbi/sbi_unit_test.h>
#அந்தர்பாவ <sbi/sbi_types.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_ubsan.h>

#define UBSAN_EXPECT_FIRES(test, stmt)                                    \
	செய் {                                                              \
		unsigned long _before = sbi_ubsan_report_மொத்தம்;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_மொத்தம், _before); \
	} போது (0)

நிலையான வெறுமை test_ubsan_add_overflow(கட்டமைப்பு sbiunit_test_case *test)
{
	நிலையற்ற முழு a = 0x7FFFFFFF; //INT_MAx
	நிலையற்ற முழு b = 1;
	நிலையற்ற முழு c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(வெறுமை)c;
}

நிலையான வெறுமை test_ubsan_sub_overflow(கட்டமைப்பு sbiunit_test_case *test)
{
	நிலையற்ற முழு a = 0x80000000; //INT_MIN
	நிலையற்ற முழு b = 1;
	நிலையற்ற முழு c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(வெறுமை)c;
}

நிலையான வெறுமை test_ubsan_mul_overflow(கட்டமைப்பு sbiunit_test_case *test)
{
	நிலையற்ற முழு a = 0x7FFFFFFF;
	நிலையற்ற முழு b = 2;
	நிலையற்ற முழு c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(வெறுமை)c;
}

நிலையான வெறுமை test_ubsan_divrem(கட்டமைப்பு sbiunit_test_case *test)
{
	நிலையற்ற முழு a = 10;
	நிலையற்ற முழு b = 0;
	நிலையற்ற முழு c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(வெறுமை)c;
}

நிலையான வெறுமை test_ubsan_oob(கட்டமைப்பு sbiunit_test_case *test)
{
	நிலையற்ற முழு idx = 5;
	முழு arr[3]	 = { 1, 2, 3 };
	நிலையற்ற முழு val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(வெறுமை)val;
}

நிலையான வெறுமை test_ubsan_shift_too_large(கட்டமைப்பு sbiunit_test_case *test)
{
	நிலையற்ற unsigned long val = 1;
	நிலையற்ற முழு shift	   = 64;
	நிலையற்ற unsigned long res;
	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(வெறுமை)res;
}

நிலையான வெறுமை test_ubsan_shift_negative(கட்டமைப்பு sbiunit_test_case *test)
{
	நிலையற்ற முழு val   = 1;
	நிலையற்ற முழு shift = -1;
	நிலையற்ற முழு res;

	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(வெறுமை)res;
}

நிலையான வெறுமை test_ubsan_load_invalid_bool(கட்டமைப்பு sbiunit_test_case *test)
{
	நிலையற்ற எழுத்து bool_val = 5;
	நிலையற்ற bool *b_ptr   = (bool *)&bool_val;
	நிலையற்ற முழு taken     = 0;
	UBSAN_EXPECT_FIRES(test, எனில் (*b_ptr) taken = 1);
	(வெறுமை)taken;
}

நிலையான வெறுமை test_ubsan_pointer_overflow(கட்டமைப்பு sbiunit_test_case *test)
{
	நிலையற்ற uintptr_t base = 0xFFFFFFFFFFFFFFFEUL;
	நிலையற்ற எழுத்து *ptr	= (எழுத்து *)base;
	நிலையற்ற எழுத்து *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(வெறுமை)res;
}

நிலையான கட்டமைப்பு sbiunit_test_case ubsan_tests[] = {
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
