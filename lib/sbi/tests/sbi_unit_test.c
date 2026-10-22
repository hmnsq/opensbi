/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unit_test.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_COLOR_GREEN "\x1b[32m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_COLOR_RED "\x1b[31m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_COLOR_RESET "\x1b[0m"

ਬਾਹਰੀ ਬਣਤਰ sbiunit_test_suite *ਅਚਲ sbi_unit_tests[];

ਸਥਿਰ ਖਾਲੀ ਲੜੀਵਾਰ_test_suite(ਬਣਤਰ sbiunit_test_suite *suite)
{
	ਬਣਤਰ sbiunit_test_case *s_case;
	u32 ਗਿਣਤੀ_pass = 0, ਗਿਣਤੀ_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	ਜੇ (suite->init)
		suite->init();

	s_case = suite->cases;
	ਜਦੋਂ (s_case->test_func) {
		s_case->test_func(s_case);
		ਜੇ (s_case->failed)
			ਗਿਣਤੀ_fail++;
		ਨਹੀਂਤਾਂ
			ਗਿਣਤੀ_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", ਗਿਣਤੀ_pass, ਗਿਣਤੀ_fail,
		   ਗਿਣਤੀ_pass + ਗਿਣਤੀ_fail);
}

ਖਾਲੀ ਲੜੀਵਾਰ_all_tests(ਖਾਲੀ)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	ਲਈ (i = 0; sbi_unit_tests[i]; i++)
		ਲੜੀਵਾਰ_test_suite(sbi_unit_tests[i]);
}
