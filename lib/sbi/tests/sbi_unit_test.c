/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#അന്തര്ഭാവ <sbi/sbi_unit_test.h>
#അന്തര്ഭാവ <sbi/sbi_types.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

ബാഹ്യ ഘടന sbiunit_test_suite *സ്ഥിരം sbi_unit_tests[];

സ്ഥിതം ശൂന്യം run_test_suite(ഘടന sbiunit_test_suite *suite)
{
	ഘടന sbiunit_test_case *s_case;
	u32 എണ്ണം_pass = 0, എണ്ണം_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	എങ്കിൽ (suite->init)
		suite->init();

	s_case = suite->cases;
	ആകുമ്പോൾ (s_case->test_func) {
		s_case->test_func(s_case);
		എങ്കിൽ (s_case->failed)
			എണ്ണം_fail++;
		അല്ലെങ്കിൽ
			എണ്ണം_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", എണ്ണം_pass, എണ്ണം_fail,
		   എണ്ണം_pass + എണ്ണം_fail);
}

ശൂന്യം run_all_tests(ശൂന്യം)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	വേണ്ടി (i = 0; sbi_unit_tests[i]; i++)
		run_test_suite(sbi_unit_tests[i]);
}
