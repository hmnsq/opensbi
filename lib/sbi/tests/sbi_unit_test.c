/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#అన్తర్భావ <sbi/sbi_unit_test.h>
#అన్తర్భావ <sbi/sbi_types.h>
#అన్తర్భావ <sbi/sbi_console.h>

#నిర్వచించు ANSI_COLOR_GREEN "\x1b[32m"
#నిర్వచించు ANSI_COLOR_RED "\x1b[31m"
#నిర్వచించు ANSI_COLOR_RESET "\x1b[0m"

బాహ్య నిర్మాణం sbiunit_test_suite *స్థిరరాశి sbi_unit_tests[];

స్థిర శూన్య నిరంతరం_test_suite(నిర్మాణం sbiunit_test_suite *suite)
{
	నిర్మాణం sbiunit_test_case *s_case;
	u32 లెక్క_pass = 0, లెక్క_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	ఒకవేళ (suite->init)
		suite->init();

	s_case = suite->cases;
	అయితే (s_case->test_func) {
		s_case->test_func(s_case);
		ఒకవేళ (s_case->failed)
			లెక్క_fail++;
		లేకపోతే
			లెక్క_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", లెక్క_pass, లెక్క_fail,
		   లెక్క_pass + లెక్క_fail);
}

శూన్య నిరంతరం_all_tests(శూన్య)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	కొరకు (i = 0; sbi_unit_tests[i]; i++)
		నిరంతరం_test_suite(sbi_unit_tests[i]);
}
