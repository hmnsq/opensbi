/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#ಅನ್ತರ್ಭಾವ <sbi/sbi_unit_test.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_types.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>

#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_COLOR_GREEN "\x1b[32m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_COLOR_RED "\x1b[31m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_COLOR_RESET "\x1b[0m"

ಬಾಹ್ಯ ರಚನೆ sbiunit_test_suite *ಸ್ಥಿರಾಂಕ sbi_unit_tests[];

ಸ್ಥಿರ ಶೂನ್ಯ ಸತತ_test_suite(ರಚನೆ sbiunit_test_suite *suite)
{
	ರಚನೆ sbiunit_test_case *s_case;
	u32 ಎಣಿಕೆ_pass = 0, ಎಣಿಕೆ_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	ಆದರೆ (suite->init)
		suite->init();

	s_case = suite->cases;
	ಆಗ (s_case->test_func) {
		s_case->test_func(s_case);
		ಆದರೆ (s_case->failed)
			ಎಣಿಕೆ_fail++;
		ಇಲ್ಲದಿದ್ದರೆ
			ಎಣಿಕೆ_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", ಎಣಿಕೆ_pass, ಎಣಿಕೆ_fail,
		   ಎಣಿಕೆ_pass + ಎಣಿಕೆ_fail);
}

ಶೂನ್ಯ ಸತತ_all_tests(ಶೂನ್ಯ)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	ಗಾಗಿ (i = 0; sbi_unit_tests[i]; i++)
		ಸತತ_test_suite(sbi_unit_tests[i]);
}
