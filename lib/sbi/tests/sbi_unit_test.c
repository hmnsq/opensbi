/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#ضمن <sbi/sbi_unit_test.h>
#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_console.h>

#تعریف_کړه ANSI_COLOR_GREEN "\x1b[32m"
#تعریف_کړه ANSI_COLOR_RED "\x1b[31m"
#تعریف_کړه ANSI_COLOR_RESET "\x1b[0m"

خارجي بنية sbiunit_test_suite *ثابت sbi_unit_tests[];

ساكن فراغ پرلپسې_test_suite(بنية sbiunit_test_suite *suite)
{
	بنية sbiunit_test_case *s_case;
	u32 شمېر_pass = 0, شمېر_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	إذا (suite->init)
		suite->init();

	s_case = suite->cases;
	طالما (s_case->test_func) {
		s_case->test_func(s_case);
		إذا (s_case->failed)
			شمېر_fail++;
		وإلا
			شمېر_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", شمېر_pass, شمېر_fail,
		   شمېر_pass + شمېر_fail);
}

فراغ پرلپسې_all_tests(فراغ)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	لكل (i = 0; sbi_unit_tests[i]; i++)
		پرلپسې_test_suite(sbi_unit_tests[i]);
}
