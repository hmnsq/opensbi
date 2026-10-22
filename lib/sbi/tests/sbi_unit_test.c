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
	u32 गणनम्_pass = 0, गणनम्_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	सचे (suite->init)
		suite->init();

	s_case = suite->cases;
	याव (s_case->test_func) {
		s_case->test_func(s_case);
		सचे (s_case->failed)
			गणनम्_fail++;
		अञ्ञथा
			गणनम्_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", गणनम्_pass, गणनम्_fail,
		   गणनम्_pass + गणनम्_fail);
}

शून्य run_all_tests(शून्य)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	अत्थाय (i = 0; sbi_unit_tests[i]; i++)
		run_test_suite(sbi_unit_tests[i]);
}
