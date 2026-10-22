/*
* SPDX-License-Identifier: BSD-2-Clause
*
* Copyright 2025 Beijing ESWIN Computing Technology Co., Ltd.
*
* Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
*/

#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_unit_test.h>

#تعریف_کړه BPL بټونه_فی_LONG

بینښه اوږد بټونه_منزل_پلټنه = 64;

ساكن بینښه اوږد ffb1[] = {};
ساكن بینښه اوږد ffb2[] = { 0 };
ساكن بینښه اوږد ffb3[] = { 1 };
ساكن بینښه اوږد ffb4[] = { 1UL << (BPL - 1) };
ساكن بینښه اوږد ffb5[] = { 0, 0x10 };
ساكن بینښه اوږد ffb6[] = { 0, 0, 1UL << (BPL - 1) };
ساكن بینښه اوږد ffb7[] = { 0, 0, 0, 0x01 };

ساكن فراغ پیدا_کړه_لومړی_بټ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_لومړی_بټ(ffb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_لومړی_بټ(ffb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_لومړی_بټ(ffb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_لومړی_بټ(ffb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_لومړی_بټ(ffb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_لومړی_بټ(ffb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_لومړی_بټ(ffb7, 4 * BPL), 3 * BPL);
}

ساكن بینښه اوږد ffzb1[] = {};
ساكن بینښه اوږد ffzb2[] = { ~0UL };
ساكن بینښه اوږد ffzb3[] = { ~1UL };
ساكن بینښه اوږد ffzb4[] = { ~(1UL << (BPL - 1)) };
ساكن بینښه اوږد ffzb5[] = { ~0UL, ~0x10UL };
ساكن بینښه اوږد ffzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ساكن بینښه اوږد ffzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ساكن فراغ پیدا_کړه_لومړی_zero_بټ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_ASSERT_EQ(test, پیدا_کړه_لومړی_zero_بټ(ffzb1, 0), 0);
	SBIUNIT_ASSERT_EQ(test, پیدا_کړه_لومړی_zero_بټ(ffzb2, BPL), BPL);
	SBIUNIT_ASSERT_EQ(test, پیدا_کړه_لومړی_zero_بټ(ffzb3, BPL), 0);
	SBIUNIT_ASSERT_EQ(test, پیدا_کړه_لومړی_zero_بټ(ffzb4, BPL), BPL - 1);
	SBIUNIT_ASSERT_EQ(test, پیدا_کړه_لومړی_zero_بټ(ffzb5, 2 * BPL), BPL + 4);
	SBIUNIT_ASSERT_EQ(test, پیدا_کړه_لومړی_zero_بټ(ffzb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_ASSERT_EQ(test, پیدا_کړه_لومړی_zero_بټ(ffzb7, 4 * BPL), 3 * BPL);
}

ساكن بینښه اوږد flb1[] = {};
ساكن بینښه اوږد flb2[] = { 0 };
ساكن بینښه اوږد flb3[] = { 1 };
ساكن بینښه اوږد flb4[] = { 1UL << (BPL - 1) };
ساكن بینښه اوږد flb5[] = { 0, 0x10 };
ساكن بینښه اوږد flb6[] = { 0, 0, 1UL << (BPL - 1) };
ساكن بینښه اوږد flb7[] = { 0, 0, 0, 0x01 };

ساكن فراغ پیدا_کړه_وروستی_بټ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_وروستی_بټ(flb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_وروستی_بټ(flb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_وروستی_بټ(flb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_وروستی_بټ(flb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_وروستی_بټ(flb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_وروستی_بټ(flb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_وروستی_بټ(flb7, 4 * BPL), 3 * BPL);
}

ساكن بینښه اوږد fnb1[] = {};
ساكن بینښه اوږد fnb2[] = { 0 };
ساكن بینښه اوږد fnb3[] = { 1 };
ساكن بینښه اوږد fnb4[] = { 1UL << (BPL - 1) };
ساكن بینښه اوږد fnb5[] = { 0, 0x10 };
ساكن بینښه اوږد fnb6[] = { 0, 0, 1UL << (BPL - 1) };
ساكن بینښه اوږد fnb7[] = { 0, 0, 0, 0x01 };

ساكن فراغ پیدا_کړه_بل_بټ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_بټ(fnb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_بټ(fnb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_بټ(fnb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_بټ(fnb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_بټ(fnb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_بټ(fnb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_بټ(fnb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_بټ(fnb5, 2 * BPL, BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_بټ(fnb7, 4 * BPL, 3 * BPL), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_بټ(fnb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ساكن بینښه اوږد fnzb1[] = {};
ساكن بینښه اوږد fnzb2[] = { ~0UL };
ساكن بینښه اوږد fnzb3[] = { ~1UL };
ساكن بینښه اوږد fnzb4[] = { ~(1UL << (BPL - 1)) };
ساكن بینښه اوږد fnzb5[] = { ~0UL, ~0x10UL };
ساكن بینښه اوږد fnzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ساكن بینښه اوږد fnzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ساكن فراغ پیدا_کړه_بل_zero_بټ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_zero_بټ(fnzb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_zero_بټ(fnzb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_zero_بټ(fnzb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_zero_بټ(fnzb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_zero_بټ(fnzb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_zero_بټ(fnzb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_zero_بټ(fnzb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_zero_بټ(fnzb5, 2 * BPL, BPL),
			  BPL + 4);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_zero_بټ(fnzb7, 4 * BPL, 3 * BPL),
			  3 * BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کړه_بل_zero_بټ(fnzb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ساكن بنية sbiunit_test_case bitops_test_cases[] = {
	SBIUNIT_TEST_CASE(پیدا_کړه_لومړی_بټ_test),
	SBIUNIT_TEST_CASE(پیدا_کړه_لومړی_zero_بټ_test),
	SBIUNIT_TEST_CASE(پیدا_کړه_وروستی_بټ_test),
	SBIUNIT_TEST_CASE(پیدا_کړه_بل_بټ_test),
	SBIUNIT_TEST_CASE(پیدا_کړه_بل_zero_بټ_test),
	SBIUNIT_پای_ټکی_CASE,
};

SBIUNIT_TEST_SUITE(bitops_test_suite, bitops_test_cases);
