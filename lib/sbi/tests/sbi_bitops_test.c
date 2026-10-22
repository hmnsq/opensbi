/*
* SPDX-License-Identifier: BSD-2-Clause
*
* Copyright 2025 Beijing ESWIN Computing Technology Co., Ltd.
*
* Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
*/

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_unit_test.h>

#define BPL BITS_PER_LONG

unsigned long bits_to_ସନ୍ଧାନ = 64;

ସ୍ଥିର unsigned long ffb1[] = {};
ସ୍ଥିର unsigned long ffb2[] = { 0 };
ସ୍ଥିର unsigned long ffb3[] = { 1 };
ସ୍ଥିର unsigned long ffb4[] = { 1UL << (BPL - 1) };
ସ୍ଥିର unsigned long ffb5[] = { 0, 0x10 };
ସ୍ଥିର unsigned long ffb6[] = { 0, 0, 1UL << (BPL - 1) };
ସ୍ଥିର unsigned long ffb7[] = { 0, 0, 0, 0x01 };

ସ୍ଥିର ଶୂନ୍ୟ find_first_bit_test(ସଂରଚନା sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, find_first_bit(ffb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, find_first_bit(ffb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, find_first_bit(ffb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, find_first_bit(ffb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, find_first_bit(ffb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, find_first_bit(ffb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, find_first_bit(ffb7, 4 * BPL), 3 * BPL);
}

ସ୍ଥିର unsigned long ffzb1[] = {};
ସ୍ଥିର unsigned long ffzb2[] = { ~0UL };
ସ୍ଥିର unsigned long ffzb3[] = { ~1UL };
ସ୍ଥିର unsigned long ffzb4[] = { ~(1UL << (BPL - 1)) };
ସ୍ଥିର unsigned long ffzb5[] = { ~0UL, ~0x10UL };
ସ୍ଥିର unsigned long ffzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ସ୍ଥିର unsigned long ffzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ସ୍ଥିର ଶୂନ୍ୟ find_first_zero_bit_test(ସଂରଚନା sbiunit_test_case *test)
{
	SBIUNIT_ASSERT_EQ(test, find_first_zero_bit(ffzb1, 0), 0);
	SBIUNIT_ASSERT_EQ(test, find_first_zero_bit(ffzb2, BPL), BPL);
	SBIUNIT_ASSERT_EQ(test, find_first_zero_bit(ffzb3, BPL), 0);
	SBIUNIT_ASSERT_EQ(test, find_first_zero_bit(ffzb4, BPL), BPL - 1);
	SBIUNIT_ASSERT_EQ(test, find_first_zero_bit(ffzb5, 2 * BPL), BPL + 4);
	SBIUNIT_ASSERT_EQ(test, find_first_zero_bit(ffzb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_ASSERT_EQ(test, find_first_zero_bit(ffzb7, 4 * BPL), 3 * BPL);
}

ସ୍ଥିର unsigned long flb1[] = {};
ସ୍ଥିର unsigned long flb2[] = { 0 };
ସ୍ଥିର unsigned long flb3[] = { 1 };
ସ୍ଥିର unsigned long flb4[] = { 1UL << (BPL - 1) };
ସ୍ଥିର unsigned long flb5[] = { 0, 0x10 };
ସ୍ଥିର unsigned long flb6[] = { 0, 0, 1UL << (BPL - 1) };
ସ୍ଥିର unsigned long flb7[] = { 0, 0, 0, 0x01 };

ସ୍ଥିର ଶୂନ୍ୟ find_last_bit_test(ସଂରଚନା sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, find_last_bit(flb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, find_last_bit(flb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, find_last_bit(flb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, find_last_bit(flb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, find_last_bit(flb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, find_last_bit(flb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, find_last_bit(flb7, 4 * BPL), 3 * BPL);
}

ସ୍ଥିର unsigned long fnb1[] = {};
ସ୍ଥିର unsigned long fnb2[] = { 0 };
ସ୍ଥିର unsigned long fnb3[] = { 1 };
ସ୍ଥିର unsigned long fnb4[] = { 1UL << (BPL - 1) };
ସ୍ଥିର unsigned long fnb5[] = { 0, 0x10 };
ସ୍ଥିର unsigned long fnb6[] = { 0, 0, 1UL << (BPL - 1) };
ସ୍ଥିର unsigned long fnb7[] = { 0, 0, 0, 0x01 };

ସ୍ଥିର ଶୂନ୍ୟ find_next_bit_test(ସଂରଚନା sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, find_next_bit(fnb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, find_next_bit(fnb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, find_next_bit(fnb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, find_next_bit(fnb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, find_next_bit(fnb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, find_next_bit(fnb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, find_next_bit(fnb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, find_next_bit(fnb5, 2 * BPL, BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, find_next_bit(fnb7, 4 * BPL, 3 * BPL), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, find_next_bit(fnb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ସ୍ଥିର unsigned long fnzb1[] = {};
ସ୍ଥିର unsigned long fnzb2[] = { ~0UL };
ସ୍ଥିର unsigned long fnzb3[] = { ~1UL };
ସ୍ଥିର unsigned long fnzb4[] = { ~(1UL << (BPL - 1)) };
ସ୍ଥିର unsigned long fnzb5[] = { ~0UL, ~0x10UL };
ସ୍ଥିର unsigned long fnzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ସ୍ଥିର unsigned long fnzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ସ୍ଥିର ଶୂନ୍ୟ find_next_zero_bit_test(ସଂରଚନା sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, find_next_zero_bit(fnzb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, find_next_zero_bit(fnzb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, find_next_zero_bit(fnzb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, find_next_zero_bit(fnzb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, find_next_zero_bit(fnzb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, find_next_zero_bit(fnzb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, find_next_zero_bit(fnzb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, find_next_zero_bit(fnzb5, 2 * BPL, BPL),
			  BPL + 4);
	SBIUNIT_EXPECT_EQ(test, find_next_zero_bit(fnzb7, 4 * BPL, 3 * BPL),
			  3 * BPL);
	SBIUNIT_EXPECT_EQ(test, find_next_zero_bit(fnzb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ସ୍ଥିର ସଂରଚନା sbiunit_test_case bitops_test_cases[] = {
	SBIUNIT_TEST_CASE(find_first_bit_test),
	SBIUNIT_TEST_CASE(find_first_zero_bit_test),
	SBIUNIT_TEST_CASE(find_last_bit_test),
	SBIUNIT_TEST_CASE(find_next_bit_test),
	SBIUNIT_TEST_CASE(find_next_zero_bit_test),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(bitops_test_suite, bitops_test_cases);
