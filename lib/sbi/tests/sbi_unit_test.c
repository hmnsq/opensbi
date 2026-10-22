/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#समावेश <sbi/sbi_unit_test.h>
#समावेश <sbi/sbi_types.h>
#समावेश <sbi/sbi_console.h>

#घोषणा ANSI_COLOR_GREEN "\x1b[32m"
#घोषणा ANSI_COLOR_RED "\x1b[31m"
#घोषणा ANSI_COLOR_RESET "\x1b[0m"

बाह्य काष्ठा sbiunit_test_suite *स्थिर sbi_unit_tests[];

अटल व्योम run_test_suite(काष्ठा sbiunit_test_suite *suite)
{
	काष्ठा sbiunit_test_case *s_case;
	u32 गिनती_pass = 0, गिनती_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	अगर (suite->init)
		suite->init();

	s_case = suite->cases;
	जबतक (s_case->test_func) {
		s_case->test_func(s_case);
		अगर (s_case->failed)
			गिनती_fail++;
		अन्यथा
			गिनती_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", गिनती_pass, गिनती_fail,
		   गिनती_pass + गिनती_fail);
}

व्योम run_all_tests(व्योम)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	क्रम (i = 0; sbi_unit_tests[i]; i++)
		run_test_suite(sbi_unit_tests[i]);
}
