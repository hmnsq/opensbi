/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#অন্তর্ভাৱ <sbi/sbi_unit_test.h>
#অন্তর্ভাৱ <sbi/sbi_types.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

বাহ্য সংরচনা sbiunit_test_suite *ধ্রুৱ sbi_unit_tests[];

স্থির শূন্য run_test_suite(সংরচনা sbiunit_test_suite *suite)
{
	সংরচনা sbiunit_test_case *s_case;
	u32 গুনতি_pass = 0, গুনতি_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	যদি (suite->init)
		suite->init();

	s_case = suite->cases;
	যেতিয়ালৈ (s_case->test_func) {
		s_case->test_func(s_case);
		যদি (s_case->failed)
			গুনতি_fail++;
		অন্যথা
			গুনতি_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", গুনতি_pass, গুনতি_fail,
		   গুনতি_pass + গুনতি_fail);
}

শূন্য run_all_tests(শূন্য)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	বাবে (i = 0; sbi_unit_tests[i]; i++)
		run_test_suite(sbi_unit_tests[i]);
}
