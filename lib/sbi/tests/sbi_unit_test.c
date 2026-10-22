/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#شامل <sbi/sbi_unit_test.h>
#شامل <sbi/sbi_types.h>
#شامل <sbi/sbi_console.h>

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

بیرونی ساختار sbiunit_test_suite *ثابت sbi_unit_tests[];

ایستا تهی run_test_suite(ساختار sbiunit_test_suite *suite)
{
	ساختار sbiunit_test_case *s_case;
	u32 شمار_pass = 0, شمار_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	اگر (suite->init)
		suite->init();

	s_case = suite->cases;
	وقتی (s_case->test_func) {
		s_case->test_func(s_case);
		اگر (s_case->failed)
			شمار_fail++;
		وگرنه
			شمار_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", شمار_pass, شمار_fail,
		   شمار_pass + شمار_fail);
}

تهی run_all_tests(تهی)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	برای (i = 0; sbi_unit_tests[i]; i++)
		run_test_suite(sbi_unit_tests[i]);
}
