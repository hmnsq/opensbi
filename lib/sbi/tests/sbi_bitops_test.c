/*
* SPDX-License-Identifier: BSD-2-Clause
*
* Copyright 2025 Beijing ESWIN Computing Technology Co., Ltd.
*
* Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
*/

#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_unit_test.h>

#ವ್ಯಾಖ್ಯಾನಿಸು BPL ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_ಹುಡುಕು = 64;

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffb1[] = {};
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffb2[] = { 0 };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffb3[] = { 1 };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffb4[] = { 1UL << (BPL - 1) };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffb5[] = { 0, 0x10 };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffb6[] = { 0, 0, 1UL << (BPL - 1) };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffb7[] = { 0, 0, 0, 0x01 };

ಸ್ಥಿರ ಶೂನ್ಯ ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಬಿಟ್_test(ರಚನೆ sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಬಿಟ್(ffb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಬಿಟ್(ffb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಬಿಟ್(ffb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಬಿಟ್(ffb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಬಿಟ್(ffb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಬಿಟ್(ffb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಬಿಟ್(ffb7, 4 * BPL), 3 * BPL);
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffzb1[] = {};
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffzb2[] = { ~0UL };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffzb3[] = { ~1UL };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffzb4[] = { ~(1UL << (BPL - 1)) };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffzb5[] = { ~0UL, ~0x10UL };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ffzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ಸ್ಥಿರ ಶೂನ್ಯ ಕಂಡುಹಿಡಿ_ಮೊದಲ_zero_ಬಿಟ್_test(ರಚನೆ sbiunit_test_case *test)
{
	SBIUNIT_ASSERT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_zero_ಬಿಟ್(ffzb1, 0), 0);
	SBIUNIT_ASSERT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_zero_ಬಿಟ್(ffzb2, BPL), BPL);
	SBIUNIT_ASSERT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_zero_ಬಿಟ್(ffzb3, BPL), 0);
	SBIUNIT_ASSERT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_zero_ಬಿಟ್(ffzb4, BPL), BPL - 1);
	SBIUNIT_ASSERT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_zero_ಬಿಟ್(ffzb5, 2 * BPL), BPL + 4);
	SBIUNIT_ASSERT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_zero_ಬಿಟ್(ffzb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_ASSERT_EQ(test, ಕಂಡುಹಿಡಿ_ಮೊದಲ_zero_ಬಿಟ್(ffzb7, 4 * BPL), 3 * BPL);
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flb1[] = {};
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flb2[] = { 0 };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flb3[] = { 1 };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flb4[] = { 1UL << (BPL - 1) };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flb5[] = { 0, 0x10 };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flb6[] = { 0, 0, 1UL << (BPL - 1) };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flb7[] = { 0, 0, 0, 0x01 };

ಸ್ಥಿರ ಶೂನ್ಯ ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಬಿಟ್_test(ರಚನೆ sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಬಿಟ್(flb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಬಿಟ್(flb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಬಿಟ್(flb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಬಿಟ್(flb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಬಿಟ್(flb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಬಿಟ್(flb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಬಿಟ್(flb7, 4 * BPL), 3 * BPL);
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnb1[] = {};
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnb2[] = { 0 };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnb3[] = { 1 };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnb4[] = { 1UL << (BPL - 1) };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnb5[] = { 0, 0x10 };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnb6[] = { 0, 0, 1UL << (BPL - 1) };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnb7[] = { 0, 0, 0, 0x01 };

ಸ್ಥಿರ ಶೂನ್ಯ ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್_test(ರಚನೆ sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್(fnb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್(fnb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್(fnb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್(fnb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್(fnb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್(fnb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್(fnb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್(fnb5, 2 * BPL, BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್(fnb7, 4 * BPL, 3 * BPL), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್(fnb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnzb1[] = {};
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnzb2[] = { ~0UL };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnzb3[] = { ~1UL };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnzb4[] = { ~(1UL << (BPL - 1)) };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnzb5[] = { ~0UL, ~0x10UL };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fnzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ಸ್ಥಿರ ಶೂನ್ಯ ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್_test(ರಚನೆ sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್(fnzb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್(fnzb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್(fnzb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್(fnzb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್(fnzb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್(fnzb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್(fnzb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್(fnzb5, 2 * BPL, BPL),
			  BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್(fnzb7, 4 * BPL, 3 * BPL),
			  3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್(fnzb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ಸ್ಥಿರ ರಚನೆ sbiunit_test_case bitops_test_cases[] = {
	SBIUNIT_TEST_CASE(ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಬಿಟ್_test),
	SBIUNIT_TEST_CASE(ಕಂಡುಹಿಡಿ_ಮೊದಲ_zero_ಬಿಟ್_test),
	SBIUNIT_TEST_CASE(ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಬಿಟ್_test),
	SBIUNIT_TEST_CASE(ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್_test),
	SBIUNIT_TEST_CASE(ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್_test),
	SBIUNIT_ಕೊನೆ_CASE,
};

SBIUNIT_TEST_SUITE(bitops_test_suite, bitops_test_cases);
