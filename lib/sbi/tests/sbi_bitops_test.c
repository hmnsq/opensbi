/*
* SPDX-License-Identifier: BSD-2-Clause
*
* Copyright 2025 Beijing ESWIN Computing Technology Co., Ltd.
*
* Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
*/

#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_unit_test.h>

#تعریف_کرو BPL بٹ_فی_LONG

غیرعلامتی طویل بٹ_مقصد_تلاش = 64;

ساکن غیرعلامتی طویل ffb1[] = {};
ساکن غیرعلامتی طویل ffb2[] = { 0 };
ساکن غیرعلامتی طویل ffb3[] = { 1 };
ساکن غیرعلامتی طویل ffb4[] = { 1UL << (BPL - 1) };
ساکن غیرعلامتی طویل ffb5[] = { 0, 0x10 };
ساکن غیرعلامتی طویل ffb6[] = { 0, 0, 1UL << (BPL - 1) };
ساکن غیرعلامتی طویل ffb7[] = { 0, 0, 0, 0x01 };

ساکن خالی ڈھونڈو_پہلا_ثنائی_ہندسہ_test(ساخت sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_پہلا_ثنائی_ہندسہ(ffb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_پہلا_ثنائی_ہندسہ(ffb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_پہلا_ثنائی_ہندسہ(ffb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_پہلا_ثنائی_ہندسہ(ffb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_پہلا_ثنائی_ہندسہ(ffb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_پہلا_ثنائی_ہندسہ(ffb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_پہلا_ثنائی_ہندسہ(ffb7, 4 * BPL), 3 * BPL);
}

ساکن غیرعلامتی طویل ffzb1[] = {};
ساکن غیرعلامتی طویل ffzb2[] = { ~0UL };
ساکن غیرعلامتی طویل ffzb3[] = { ~1UL };
ساکن غیرعلامتی طویل ffzb4[] = { ~(1UL << (BPL - 1)) };
ساکن غیرعلامتی طویل ffzb5[] = { ~0UL, ~0x10UL };
ساکن غیرعلامتی طویل ffzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ساکن غیرعلامتی طویل ffzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ساکن خالی ڈھونڈو_پہلا_zero_ثنائی_ہندسہ_test(ساخت sbiunit_test_case *test)
{
	SBIUNIT_ASSERT_EQ(test, ڈھونڈو_پہلا_zero_ثنائی_ہندسہ(ffzb1, 0), 0);
	SBIUNIT_ASSERT_EQ(test, ڈھونڈو_پہلا_zero_ثنائی_ہندسہ(ffzb2, BPL), BPL);
	SBIUNIT_ASSERT_EQ(test, ڈھونڈو_پہلا_zero_ثنائی_ہندسہ(ffzb3, BPL), 0);
	SBIUNIT_ASSERT_EQ(test, ڈھونڈو_پہلا_zero_ثنائی_ہندسہ(ffzb4, BPL), BPL - 1);
	SBIUNIT_ASSERT_EQ(test, ڈھونڈو_پہلا_zero_ثنائی_ہندسہ(ffzb5, 2 * BPL), BPL + 4);
	SBIUNIT_ASSERT_EQ(test, ڈھونڈو_پہلا_zero_ثنائی_ہندسہ(ffzb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_ASSERT_EQ(test, ڈھونڈو_پہلا_zero_ثنائی_ہندسہ(ffzb7, 4 * BPL), 3 * BPL);
}

ساکن غیرعلامتی طویل flb1[] = {};
ساکن غیرعلامتی طویل flb2[] = { 0 };
ساکن غیرعلامتی طویل flb3[] = { 1 };
ساکن غیرعلامتی طویل flb4[] = { 1UL << (BPL - 1) };
ساکن غیرعلامتی طویل flb5[] = { 0, 0x10 };
ساکن غیرعلامتی طویل flb6[] = { 0, 0, 1UL << (BPL - 1) };
ساکن غیرعلامتی طویل flb7[] = { 0, 0, 0, 0x01 };

ساکن خالی ڈھونڈو_آخری_ثنائی_ہندسہ_test(ساخت sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_آخری_ثنائی_ہندسہ(flb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_آخری_ثنائی_ہندسہ(flb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_آخری_ثنائی_ہندسہ(flb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_آخری_ثنائی_ہندسہ(flb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_آخری_ثنائی_ہندسہ(flb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_آخری_ثنائی_ہندسہ(flb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_آخری_ثنائی_ہندسہ(flb7, 4 * BPL), 3 * BPL);
}

ساکن غیرعلامتی طویل fnb1[] = {};
ساکن غیرعلامتی طویل fnb2[] = { 0 };
ساکن غیرعلامتی طویل fnb3[] = { 1 };
ساکن غیرعلامتی طویل fnb4[] = { 1UL << (BPL - 1) };
ساکن غیرعلامتی طویل fnb5[] = { 0, 0x10 };
ساکن غیرعلامتی طویل fnb6[] = { 0, 0, 1UL << (BPL - 1) };
ساکن غیرعلامتی طویل fnb7[] = { 0, 0, 0, 0x01 };

ساکن خالی ڈھونڈو_اگلا_ثنائی_ہندسہ_test(ساخت sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_ثنائی_ہندسہ(fnb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_ثنائی_ہندسہ(fnb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_ثنائی_ہندسہ(fnb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_ثنائی_ہندسہ(fnb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_ثنائی_ہندسہ(fnb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_ثنائی_ہندسہ(fnb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_ثنائی_ہندسہ(fnb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_ثنائی_ہندسہ(fnb5, 2 * BPL, BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_ثنائی_ہندسہ(fnb7, 4 * BPL, 3 * BPL), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_ثنائی_ہندسہ(fnb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ساکن غیرعلامتی طویل fnzb1[] = {};
ساکن غیرعلامتی طویل fnzb2[] = { ~0UL };
ساکن غیرعلامتی طویل fnzb3[] = { ~1UL };
ساکن غیرعلامتی طویل fnzb4[] = { ~(1UL << (BPL - 1)) };
ساکن غیرعلامتی طویل fnzb5[] = { ~0UL, ~0x10UL };
ساکن غیرعلامتی طویل fnzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ساکن غیرعلامتی طویل fnzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ساکن خالی ڈھونڈو_اگلا_zero_ثنائی_ہندسہ_test(ساخت sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_zero_ثنائی_ہندسہ(fnzb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_zero_ثنائی_ہندسہ(fnzb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_zero_ثنائی_ہندسہ(fnzb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_zero_ثنائی_ہندسہ(fnzb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_zero_ثنائی_ہندسہ(fnzb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_zero_ثنائی_ہندسہ(fnzb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_zero_ثنائی_ہندسہ(fnzb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_zero_ثنائی_ہندسہ(fnzb5, 2 * BPL, BPL),
			  BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_zero_ثنائی_ہندسہ(fnzb7, 4 * BPL, 3 * BPL),
			  3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ڈھونڈو_اگلا_zero_ثنائی_ہندسہ(fnzb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ساکن ساخت sbiunit_test_case bitops_test_cases[] = {
	SBIUNIT_TEST_CASE(ڈھونڈو_پہلا_ثنائی_ہندسہ_test),
	SBIUNIT_TEST_CASE(ڈھونڈو_پہلا_zero_ثنائی_ہندسہ_test),
	SBIUNIT_TEST_CASE(ڈھونڈو_آخری_ثنائی_ہندسہ_test),
	SBIUNIT_TEST_CASE(ڈھونڈو_اگلا_ثنائی_ہندسہ_test),
	SBIUNIT_TEST_CASE(ڈھونڈو_اگلا_zero_ثنائی_ہندسہ_test),
	SBIUNIT_انتہا_CASE,
};

SBIUNIT_TEST_SUITE(bitops_test_suite, bitops_test_cases);
