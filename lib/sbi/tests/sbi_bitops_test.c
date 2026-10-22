/*
* SPDX-License-Identifier: BSD-2-Clause
*
* Copyright 2025 Beijing ESWIN Computing Technology Co., Ltd.
*
* Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
*/

#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_unit_test.h>

#تعریف_کرو BPL بٹ_فی_LONG

بنانشانی لما بٹ_منزل_تلاش = 64;

ساكن بنانشانی لما ffb1[] = {};
ساكن بنانشانی لما ffb2[] = { 0 };
ساكن بنانشانی لما ffb3[] = { 1 };
ساكن بنانشانی لما ffb4[] = { 1UL << (BPL - 1) };
ساكن بنانشانی لما ffb5[] = { 0, 0x10 };
ساكن بنانشانی لما ffb6[] = { 0, 0, 1UL << (BPL - 1) };
ساكن بنانشانی لما ffb7[] = { 0, 0, 0, 0x01 };

ساكن فراغ لبھو_پہلا_ثنائی_ہندسہ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, لبھو_پہلا_ثنائی_ہندسہ(ffb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, لبھو_پہلا_ثنائی_ہندسہ(ffb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, لبھو_پہلا_ثنائی_ہندسہ(ffb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, لبھو_پہلا_ثنائی_ہندسہ(ffb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, لبھو_پہلا_ثنائی_ہندسہ(ffb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, لبھو_پہلا_ثنائی_ہندسہ(ffb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, لبھو_پہلا_ثنائی_ہندسہ(ffb7, 4 * BPL), 3 * BPL);
}

ساكن بنانشانی لما ffzb1[] = {};
ساكن بنانشانی لما ffzb2[] = { ~0UL };
ساكن بنانشانی لما ffzb3[] = { ~1UL };
ساكن بنانشانی لما ffzb4[] = { ~(1UL << (BPL - 1)) };
ساكن بنانشانی لما ffzb5[] = { ~0UL, ~0x10UL };
ساكن بنانشانی لما ffzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ساكن بنانشانی لما ffzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ساكن فراغ لبھو_پہلا_zero_ثنائی_ہندسہ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_ASSERT_EQ(test, لبھو_پہلا_zero_ثنائی_ہندسہ(ffzb1, 0), 0);
	SBIUNIT_ASSERT_EQ(test, لبھو_پہلا_zero_ثنائی_ہندسہ(ffzb2, BPL), BPL);
	SBIUNIT_ASSERT_EQ(test, لبھو_پہلا_zero_ثنائی_ہندسہ(ffzb3, BPL), 0);
	SBIUNIT_ASSERT_EQ(test, لبھو_پہلا_zero_ثنائی_ہندسہ(ffzb4, BPL), BPL - 1);
	SBIUNIT_ASSERT_EQ(test, لبھو_پہلا_zero_ثنائی_ہندسہ(ffzb5, 2 * BPL), BPL + 4);
	SBIUNIT_ASSERT_EQ(test, لبھو_پہلا_zero_ثنائی_ہندسہ(ffzb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_ASSERT_EQ(test, لبھو_پہلا_zero_ثنائی_ہندسہ(ffzb7, 4 * BPL), 3 * BPL);
}

ساكن بنانشانی لما flb1[] = {};
ساكن بنانشانی لما flb2[] = { 0 };
ساكن بنانشانی لما flb3[] = { 1 };
ساكن بنانشانی لما flb4[] = { 1UL << (BPL - 1) };
ساكن بنانشانی لما flb5[] = { 0, 0x10 };
ساكن بنانشانی لما flb6[] = { 0, 0, 1UL << (BPL - 1) };
ساكن بنانشانی لما flb7[] = { 0, 0, 0, 0x01 };

ساكن فراغ لبھو_آخری_ثنائی_ہندسہ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, لبھو_آخری_ثنائی_ہندسہ(flb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, لبھو_آخری_ثنائی_ہندسہ(flb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, لبھو_آخری_ثنائی_ہندسہ(flb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, لبھو_آخری_ثنائی_ہندسہ(flb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, لبھو_آخری_ثنائی_ہندسہ(flb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, لبھو_آخری_ثنائی_ہندسہ(flb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, لبھو_آخری_ثنائی_ہندسہ(flb7, 4 * BPL), 3 * BPL);
}

ساكن بنانشانی لما fnb1[] = {};
ساكن بنانشانی لما fnb2[] = { 0 };
ساكن بنانشانی لما fnb3[] = { 1 };
ساكن بنانشانی لما fnb4[] = { 1UL << (BPL - 1) };
ساكن بنانشانی لما fnb5[] = { 0, 0x10 };
ساكن بنانشانی لما fnb6[] = { 0, 0, 1UL << (BPL - 1) };
ساكن بنانشانی لما fnb7[] = { 0, 0, 0, 0x01 };

ساكن فراغ لبھو_اگلا_ثنائی_ہندسہ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_ثنائی_ہندسہ(fnb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_ثنائی_ہندسہ(fnb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_ثنائی_ہندسہ(fnb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_ثنائی_ہندسہ(fnb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_ثنائی_ہندسہ(fnb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_ثنائی_ہندسہ(fnb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_ثنائی_ہندسہ(fnb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_ثنائی_ہندسہ(fnb5, 2 * BPL, BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_ثنائی_ہندسہ(fnb7, 4 * BPL, 3 * BPL), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_ثنائی_ہندسہ(fnb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ساكن بنانشانی لما fnzb1[] = {};
ساكن بنانشانی لما fnzb2[] = { ~0UL };
ساكن بنانشانی لما fnzb3[] = { ~1UL };
ساكن بنانشانی لما fnzb4[] = { ~(1UL << (BPL - 1)) };
ساكن بنانشانی لما fnzb5[] = { ~0UL, ~0x10UL };
ساكن بنانشانی لما fnzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ساكن بنانشانی لما fnzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ساكن فراغ لبھو_اگلا_zero_ثنائی_ہندسہ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_zero_ثنائی_ہندسہ(fnzb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_zero_ثنائی_ہندسہ(fnzb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_zero_ثنائی_ہندسہ(fnzb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_zero_ثنائی_ہندسہ(fnzb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_zero_ثنائی_ہندسہ(fnzb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_zero_ثنائی_ہندسہ(fnzb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_zero_ثنائی_ہندسہ(fnzb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_zero_ثنائی_ہندسہ(fnzb5, 2 * BPL, BPL),
			  BPL + 4);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_zero_ثنائی_ہندسہ(fnzb7, 4 * BPL, 3 * BPL),
			  3 * BPL);
	SBIUNIT_EXPECT_EQ(test, لبھو_اگلا_zero_ثنائی_ہندسہ(fnzb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ساكن بنية sbiunit_test_case bitops_test_cases[] = {
	SBIUNIT_TEST_CASE(لبھو_پہلا_ثنائی_ہندسہ_test),
	SBIUNIT_TEST_CASE(لبھو_پہلا_zero_ثنائی_ہندسہ_test),
	SBIUNIT_TEST_CASE(لبھو_آخری_ثنائی_ہندسہ_test),
	SBIUNIT_TEST_CASE(لبھو_اگلا_ثنائی_ہندسہ_test),
	SBIUNIT_TEST_CASE(لبھو_اگلا_zero_ثنائی_ہندسہ_test),
	SBIUNIT_سرا_CASE,
};

SBIUNIT_TEST_SUITE(bitops_test_suite, bitops_test_cases);
