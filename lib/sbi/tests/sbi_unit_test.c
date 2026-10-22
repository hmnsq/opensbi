/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#شامل <sbi/sbi_unit_test.h>
#شامل <sbi/sbi_types.h>
#شامل <sbi/sbi_console.h>

#تعریف_کرو ANSI_COLOR_GREEN "\x1b[32m"
#تعریف_کرو ANSI_COLOR_RED "\x1b[31m"
#تعریف_کرو ANSI_COLOR_RESET "\x1b[0m"

خارجی ساخت sbiunit_test_suite *ثابت sbi_unit_tests[];

ساکن خالی تسلسل_test_suite(ساخت sbiunit_test_suite *suite)
{
	ساخت sbiunit_test_case *s_case;
	u32 گنتی_pass = 0, گنتی_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	اگر (suite->init)
		suite->init();

	s_case = suite->cases;
	جب (s_case->test_func) {
		s_case->test_func(s_case);
		اگر (s_case->failed)
			گنتی_fail++;
		ورنہ
			گنتی_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", گنتی_pass, گنتی_fail,
		   گنتی_pass + گنتی_fail);
}

خالی تسلسل_all_tests(خالی)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	برائے (i = 0; sbi_unit_tests[i]; i++)
		تسلسل_test_suite(sbi_unit_tests[i]);
}
