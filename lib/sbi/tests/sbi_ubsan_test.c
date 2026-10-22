/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_unit_test.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_types.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ubsan.h>

#ವ್ಯಾಖ್ಯಾನಿಸು UBSAN_EXPECT_FIRES(test, stmt)                                    \
	ಮಾಡು {                                                              \
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ _before = sbi_ubsan_report_ಎಣಿಕೆ;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_ಎಣಿಕೆ, _before); \
	} ಆಗ (0)

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_ಕೂಡಿಸು_ಅತಿಪ್ರವಾಹ(ರಚನೆ sbiunit_test_case *test)
{
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ a = 0x7FFFFFFF; //INT_MAx
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ b = 1;
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(ಶೂನ್ಯ)c;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_sub_ಅತಿಪ್ರವಾಹ(ರಚನೆ sbiunit_test_case *test)
{
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ a = 0x80000000; //INT_MIN
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ b = 1;
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(ಶೂನ್ಯ)c;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_mul_ಅತಿಪ್ರವಾಹ(ರಚನೆ sbiunit_test_case *test)
{
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ a = 0x7FFFFFFF;
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ b = 2;
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(ಶೂನ್ಯ)c;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_divrem(ರಚನೆ sbiunit_test_case *test)
{
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ a = 10;
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ b = 0;
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(ಶೂನ್ಯ)c;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_oob(ರಚನೆ sbiunit_test_case *test)
{
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ idx = 5;
	ಪೂರ್ಣಾಂಕ arr[3]	 = { 1, 2, 3 };
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(ಶೂನ್ಯ)val;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_ಸರಿಸು_too_large(ರಚನೆ sbiunit_test_case *test)
{
	ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val = 1;
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸರಿಸು	   = 64;
	ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ res;
	UBSAN_EXPECT_FIRES(test, res = val << ಸರಿಸು);
	(ಶೂನ್ಯ)res;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_ಸರಿಸು_negative(ರಚನೆ sbiunit_test_case *test)
{
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ val   = 1;
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸರಿಸು = -1;
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ res;

	UBSAN_EXPECT_FIRES(test, res = val << ಸರಿಸು);
	(ಶೂನ್ಯ)res;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_ತುಂಬು_invalid_bool(ರಚನೆ sbiunit_test_case *test)
{
	ಅಸ್ಥಿರ ಅಕ್ಷರ bool_val = 5;
	ಅಸ್ಥಿರ bool *b_ptr   = (bool *)&bool_val;
	ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ತೆಗೆದುಕೊಂಡ     = 0;
	UBSAN_EXPECT_FIRES(test, ಆದರೆ (*b_ptr) ತೆಗೆದುಕೊಂಡ = 1);
	(ಶೂನ್ಯ)ತೆಗೆದುಕೊಂಡ;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_ubsan_ಸೂಚಕ_ಅತಿಪ್ರವಾಹ(ರಚನೆ sbiunit_test_case *test)
{
	ಅಸ್ಥಿರ uintptr_t ತಳ_2 = 0xFFFFFFFFFFFFFFFEUL;
	ಅಸ್ಥಿರ ಅಕ್ಷರ *ptr	= (ಅಕ್ಷರ *)ತಳ_2;
	ಅಸ್ಥಿರ ಅಕ್ಷರ *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(ಶೂನ್ಯ)res;
}

ಸ್ಥಿರ ರಚನೆ sbiunit_test_case ubsan_tests[] = {
	SBIUNIT_TEST_CASE(test_ubsan_ಕೂಡಿಸು_ಅತಿಪ್ರವಾಹ),
	SBIUNIT_TEST_CASE(test_ubsan_sub_ಅತಿಪ್ರವಾಹ),
	SBIUNIT_TEST_CASE(test_ubsan_mul_ಅತಿಪ್ರವಾಹ),
	SBIUNIT_TEST_CASE(test_ubsan_divrem),
	SBIUNIT_TEST_CASE(test_ubsan_oob),
	SBIUNIT_TEST_CASE(test_ubsan_ಸರಿಸು_too_large),
	SBIUNIT_TEST_CASE(test_ubsan_ಸರಿಸು_negative),
	SBIUNIT_TEST_CASE(test_ubsan_ತುಂಬು_invalid_bool),
	SBIUNIT_TEST_CASE(test_ubsan_ಸೂಚಕ_ಅತಿಪ್ರವಾಹ),
	SBIUNIT_ಕೊನೆ_CASE,
};

SBIUNIT_TEST_SUITE(ubsan_test_suite, ubsan_tests);
