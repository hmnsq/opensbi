/*
* SPDX-License-Identifier: BSD-2-Clause
*
* Copyright 2025 Beijing ESWIN Computing Technology Co., Ltd.
*
* Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
*/

#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_unit_test.h>

#وصف_ڪريو BPL بٽ_في_LONG

بنانشان ڊگهو بٽ_منزل_ڳولا = 64;

ساكن بنانشان ڊگهو ffb1[] = {};
ساكن بنانشان ڊگهو ffb2[] = { 0 };
ساكن بنانشان ڊگهو ffb3[] = { 1 };
ساكن بنانشان ڊگهو ffb4[] = { 1UL << (BPL - 1) };
ساكن بنانشان ڊگهو ffb5[] = { 0, 0x10 };
ساكن بنانشان ڊگهو ffb6[] = { 0, 0, 1UL << (BPL - 1) };
ساكن بنانشان ڊگهو ffb7[] = { 0, 0, 0, 0x01 };

ساكن فراغ ڳوليو_پهريون_ٻٽو_انگ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ڳوليو_پهريون_ٻٽو_انگ(ffb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_پهريون_ٻٽو_انگ(ffb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_پهريون_ٻٽو_انگ(ffb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_پهريون_ٻٽو_انگ(ffb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_پهريون_ٻٽو_انگ(ffb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_پهريون_ٻٽو_انگ(ffb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_پهريون_ٻٽو_انگ(ffb7, 4 * BPL), 3 * BPL);
}

ساكن بنانشان ڊگهو ffzb1[] = {};
ساكن بنانشان ڊگهو ffzb2[] = { ~0UL };
ساكن بنانشان ڊگهو ffzb3[] = { ~1UL };
ساكن بنانشان ڊگهو ffzb4[] = { ~(1UL << (BPL - 1)) };
ساكن بنانشان ڊگهو ffzb5[] = { ~0UL, ~0x10UL };
ساكن بنانشان ڊگهو ffzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ساكن بنانشان ڊگهو ffzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ساكن فراغ ڳوليو_پهريون_zero_ٻٽو_انگ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_ASSERT_EQ(test, ڳوليو_پهريون_zero_ٻٽو_انگ(ffzb1, 0), 0);
	SBIUNIT_ASSERT_EQ(test, ڳوليو_پهريون_zero_ٻٽو_انگ(ffzb2, BPL), BPL);
	SBIUNIT_ASSERT_EQ(test, ڳوليو_پهريون_zero_ٻٽو_انگ(ffzb3, BPL), 0);
	SBIUNIT_ASSERT_EQ(test, ڳوليو_پهريون_zero_ٻٽو_انگ(ffzb4, BPL), BPL - 1);
	SBIUNIT_ASSERT_EQ(test, ڳوليو_پهريون_zero_ٻٽو_انگ(ffzb5, 2 * BPL), BPL + 4);
	SBIUNIT_ASSERT_EQ(test, ڳوليو_پهريون_zero_ٻٽو_انگ(ffzb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_ASSERT_EQ(test, ڳوليو_پهريون_zero_ٻٽو_انگ(ffzb7, 4 * BPL), 3 * BPL);
}

ساكن بنانشان ڊگهو flb1[] = {};
ساكن بنانشان ڊگهو flb2[] = { 0 };
ساكن بنانشان ڊگهو flb3[] = { 1 };
ساكن بنانشان ڊگهو flb4[] = { 1UL << (BPL - 1) };
ساكن بنانشان ڊگهو flb5[] = { 0, 0x10 };
ساكن بنانشان ڊگهو flb6[] = { 0, 0, 1UL << (BPL - 1) };
ساكن بنانشان ڊگهو flb7[] = { 0, 0, 0, 0x01 };

ساكن فراغ ڳوليو_آخري_ٻٽو_انگ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ڳوليو_آخري_ٻٽو_انگ(flb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_آخري_ٻٽو_انگ(flb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_آخري_ٻٽو_انگ(flb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_آخري_ٻٽو_انگ(flb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_آخري_ٻٽو_انگ(flb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_آخري_ٻٽو_انگ(flb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_آخري_ٻٽو_انگ(flb7, 4 * BPL), 3 * BPL);
}

ساكن بنانشان ڊگهو fnb1[] = {};
ساكن بنانشان ڊگهو fnb2[] = { 0 };
ساكن بنانشان ڊگهو fnb3[] = { 1 };
ساكن بنانشان ڊگهو fnb4[] = { 1UL << (BPL - 1) };
ساكن بنانشان ڊگهو fnb5[] = { 0, 0x10 };
ساكن بنانشان ڊگهو fnb6[] = { 0, 0, 1UL << (BPL - 1) };
ساكن بنانشان ڊگهو fnb7[] = { 0, 0, 0, 0x01 };

ساكن فراغ ڳوليو_ايندڙ_ٻٽو_انگ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_ٻٽو_انگ(fnb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_ٻٽو_انگ(fnb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_ٻٽو_انگ(fnb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_ٻٽو_انگ(fnb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_ٻٽو_انگ(fnb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_ٻٽو_انگ(fnb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_ٻٽو_انگ(fnb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_ٻٽو_انگ(fnb5, 2 * BPL, BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_ٻٽو_انگ(fnb7, 4 * BPL, 3 * BPL), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_ٻٽو_انگ(fnb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ساكن بنانشان ڊگهو fnzb1[] = {};
ساكن بنانشان ڊگهو fnzb2[] = { ~0UL };
ساكن بنانشان ڊگهو fnzb3[] = { ~1UL };
ساكن بنانشان ڊگهو fnzb4[] = { ~(1UL << (BPL - 1)) };
ساكن بنانشان ڊگهو fnzb5[] = { ~0UL, ~0x10UL };
ساكن بنانشان ڊگهو fnzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ساكن بنانشان ڊگهو fnzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ساكن فراغ ڳوليو_ايندڙ_zero_ٻٽو_انگ_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_zero_ٻٽو_انگ(fnzb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_zero_ٻٽو_انگ(fnzb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_zero_ٻٽو_انگ(fnzb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_zero_ٻٽو_انگ(fnzb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_zero_ٻٽو_انگ(fnzb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_zero_ٻٽو_انگ(fnzb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_zero_ٻٽو_انگ(fnzb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_zero_ٻٽو_انگ(fnzb5, 2 * BPL, BPL),
			  BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_zero_ٻٽو_انگ(fnzb7, 4 * BPL, 3 * BPL),
			  3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ڳوليو_ايندڙ_zero_ٻٽو_انگ(fnzb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ساكن بنية sbiunit_test_case bitops_test_cases[] = {
	SBIUNIT_TEST_CASE(ڳوليو_پهريون_ٻٽو_انگ_test),
	SBIUNIT_TEST_CASE(ڳوليو_پهريون_zero_ٻٽو_انگ_test),
	SBIUNIT_TEST_CASE(ڳوليو_آخري_ٻٽو_انگ_test),
	SBIUNIT_TEST_CASE(ڳوليو_ايندڙ_ٻٽو_انگ_test),
	SBIUNIT_TEST_CASE(ڳوليو_ايندڙ_zero_ٻٽو_انگ_test),
	SBIUNIT_پڇاڙي_CASE,
};

SBIUNIT_TEST_SUITE(bitops_test_suite, bitops_test_cases);
