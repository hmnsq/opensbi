/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_unit_test.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_types.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

ବାହ୍ୟ ସଂରଚନା sbiunit_test_suite *ଅଚଳ sbi_unit_tests[];

ସ୍ଥିର ଶୂନ୍ୟ run_test_suite(ସଂରଚନା sbiunit_test_suite *suite)
{
	ସଂରଚନା sbiunit_test_case *s_case;
	u32 ଗଣନା_pass = 0, ଗଣନା_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	ଯଦି (suite->init)
		suite->init();

	s_case = suite->cases;
	ଯେତେବେଳେ (s_case->test_func) {
		s_case->test_func(s_case);
		ଯଦି (s_case->failed)
			ଗଣନା_fail++;
		ଅନ୍ୟଥା
			ଗଣନା_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", ଗଣନା_pass, ଗଣନା_fail,
		   ଗଣନା_pass + ଗଣନା_fail);
}

ଶୂନ୍ୟ run_all_tests(ଶୂନ୍ୟ)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	ପାଇଁ (i = 0; sbi_unit_tests[i]; i++)
		run_test_suite(sbi_unit_tests[i]);
}
