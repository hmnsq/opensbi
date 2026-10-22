/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#અન્તર્ભાવ <sbi/sbi_unit_test.h>
#અન્તર્ભાવ <sbi/sbi_types.h>
#અન્તર્ભાવ <sbi/sbi_console.h>

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

બાહ્ય સંરચના sbiunit_test_suite *અચલ sbi_unit_tests[];

સ્થિર શૂન્ય run_test_suite(સંરચના sbiunit_test_suite *suite)
{
	સંરચના sbiunit_test_case *s_case;
	u32 ગણતરી_pass = 0, ગણતરી_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	જો (suite->init)
		suite->init();

	s_case = suite->cases;
	જ્યારે (s_case->test_func) {
		s_case->test_func(s_case);
		જો (s_case->failed)
			ગણતરી_fail++;
		નહીંતર
			ગણતરી_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", ગણતરી_pass, ગણતરી_fail,
		   ગણતરી_pass + ગણતરી_fail);
}

શૂન્ય run_all_tests(શૂન્ય)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	માટે (i = 0; sbi_unit_tests[i]; i++)
		run_test_suite(sbi_unit_tests[i]);
}
