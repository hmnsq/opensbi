/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#അന്തര്ഭാവ <sbi/sbi_unit_test.h>
#അന്തര്ഭാവ <sbi/sbi_types.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_ubsan.h>

#define UBSAN_EXPECT_FIRES(test, stmt)                                    \
	ചെയ്യുക {                                                              \
		unsigned long _before = sbi_ubsan_report_എണ്ണം;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_എണ്ണം, _before); \
	} ആകുമ്പോൾ (0)

സ്ഥിതം ശൂന്യം test_ubsan_add_overflow(ഘടന sbiunit_test_case *test)
{
	അസ്ഥിര പൂർണ്ണസംഖ്യ a = 0x7FFFFFFF; //INT_MAx
	അസ്ഥിര പൂർണ്ണസംഖ്യ b = 1;
	അസ്ഥിര പൂർണ്ണസംഖ്യ c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(ശൂന്യം)c;
}

സ്ഥിതം ശൂന്യം test_ubsan_sub_overflow(ഘടന sbiunit_test_case *test)
{
	അസ്ഥിര പൂർണ്ണസംഖ്യ a = 0x80000000; //INT_MIN
	അസ്ഥിര പൂർണ്ണസംഖ്യ b = 1;
	അസ്ഥിര പൂർണ്ണസംഖ്യ c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(ശൂന്യം)c;
}

സ്ഥിതം ശൂന്യം test_ubsan_mul_overflow(ഘടന sbiunit_test_case *test)
{
	അസ്ഥിര പൂർണ്ണസംഖ്യ a = 0x7FFFFFFF;
	അസ്ഥിര പൂർണ്ണസംഖ്യ b = 2;
	അസ്ഥിര പൂർണ്ണസംഖ്യ c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(ശൂന്യം)c;
}

സ്ഥിതം ശൂന്യം test_ubsan_divrem(ഘടന sbiunit_test_case *test)
{
	അസ്ഥിര പൂർണ്ണസംഖ്യ a = 10;
	അസ്ഥിര പൂർണ്ണസംഖ്യ b = 0;
	അസ്ഥിര പൂർണ്ണസംഖ്യ c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(ശൂന്യം)c;
}

സ്ഥിതം ശൂന്യം test_ubsan_oob(ഘടന sbiunit_test_case *test)
{
	അസ്ഥിര പൂർണ്ണസംഖ്യ idx = 5;
	പൂർണ്ണസംഖ്യ arr[3]	 = { 1, 2, 3 };
	അസ്ഥിര പൂർണ്ണസംഖ്യ val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(ശൂന്യം)val;
}

സ്ഥിതം ശൂന്യം test_ubsan_shift_too_large(ഘടന sbiunit_test_case *test)
{
	അസ്ഥിര unsigned long val = 1;
	അസ്ഥിര പൂർണ്ണസംഖ്യ shift	   = 64;
	അസ്ഥിര unsigned long res;
	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(ശൂന്യം)res;
}

സ്ഥിതം ശൂന്യം test_ubsan_shift_negative(ഘടന sbiunit_test_case *test)
{
	അസ്ഥിര പൂർണ്ണസംഖ്യ val   = 1;
	അസ്ഥിര പൂർണ്ണസംഖ്യ shift = -1;
	അസ്ഥിര പൂർണ്ണസംഖ്യ res;

	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(ശൂന്യം)res;
}

സ്ഥിതം ശൂന്യം test_ubsan_load_invalid_bool(ഘടന sbiunit_test_case *test)
{
	അസ്ഥിര അക്ഷരം bool_val = 5;
	അസ്ഥിര bool *b_ptr   = (bool *)&bool_val;
	അസ്ഥിര പൂർണ്ണസംഖ്യ taken     = 0;
	UBSAN_EXPECT_FIRES(test, എങ്കിൽ (*b_ptr) taken = 1);
	(ശൂന്യം)taken;
}

സ്ഥിതം ശൂന്യം test_ubsan_pointer_overflow(ഘടന sbiunit_test_case *test)
{
	അസ്ഥിര uintptr_t base = 0xFFFFFFFFFFFFFFFEUL;
	അസ്ഥിര അക്ഷരം *ptr	= (അക്ഷരം *)base;
	അസ്ഥിര അക്ഷരം *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(ശൂന്യം)res;
}

സ്ഥിതം ഘടന sbiunit_test_case ubsan_tests[] = {
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
