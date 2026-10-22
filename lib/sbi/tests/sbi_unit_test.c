/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#அந்தர்பாவ <sbi/sbi_unit_test.h>
#அந்தர்பாவ <sbi/sbi_types.h>
#அந்தர்பாவ <sbi/sbi_console.h>

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

வெளி கட்டமைப்பு sbiunit_test_suite *மாறிலி sbi_unit_tests[];

நிலையான வெறுமை run_test_suite(கட்டமைப்பு sbiunit_test_suite *suite)
{
	கட்டமைப்பு sbiunit_test_case *s_case;
	u32 மொத்தம்_pass = 0, மொத்தம்_fail = 0;

	sbi_printf("## Running test suite: %s\n", suite->name);

	எனில் (suite->init)
		suite->init();

	s_case = suite->cases;
	போது (s_case->test_func) {
		s_case->test_func(s_case);
		எனில் (s_case->failed)
			மொத்தம்_fail++;
		இல்லையெனில்
			மொத்தம்_pass++;
		sbi_printf("%s[%s]%s %s\n",
			   s_case->failed ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
			   s_case->failed ? "FAILED" : "PASSED",
			   ANSI_COLOR_RESET, s_case->name);
		s_case++;
	}

	sbi_printf("%u PASSED / %u FAILED / %u TOTAL\n", மொத்தம்_pass, மொத்தம்_fail,
		   மொத்தம்_pass + மொத்தம்_fail);
}

வெறுமை run_all_tests(வெறுமை)
{
	u32 i;

	sbi_printf("\n# Running SBIUNIT tests #\n");

	க்கு (i = 0; sbi_unit_tests[i]; i++)
		run_test_suite(sbi_unit_tests[i]);
}
