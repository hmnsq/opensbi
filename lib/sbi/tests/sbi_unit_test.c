/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#अन्तर्भाव <sbi/sbi_unit_test.h>
#अन्तर्भाव <sbi/sbi_types.h>
#अन्तर्भाव <sbi/sbi_console.h>

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

बाह्य संरचना sbiunit_test_suite *ध्रुव sbi_unit_tests[];

स्थिर शून्य run_test_suite(संरचना sbiunit_test_suite *suite)
{
	संरचना sbiunit_test_case *s_case;
	u32 मोजणी_pass = 0, मोजणी_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	यदि (suite->init)
		suite->init();

	s_case = suite->cases;
	यावत् (s_case->test_func) {
		s_case->test_func(s_case);
		यदि (s_case->failed)
			मोजणी_fail++;
		अन्यथा
			मोजणी_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", मोजणी_pass, मोजणी_fail,
		   मोजणी_pass + मोजणी_fail);
}

शून्य run_all_tests(शून्य)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	प्रति (i = 0; sbi_unit_tests[i]; i++)
		run_test_suite(sbi_unit_tests[i]);
}
