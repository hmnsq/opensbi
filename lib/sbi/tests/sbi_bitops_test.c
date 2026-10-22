/*
* SPDX-License-Identifier: BSD-2-Clause
*
* Copyright 2025 Beijing ESWIN Computing Technology Co., Ltd.
*
* Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
*/

#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_unit_test.h>

#تعریف_کن BPL بیتها_در_هر_LONG

بیعلامت دراز بیتها_مقصد_جستجو = 64;

ساكن بیعلامت دراز ffb1[] = {};
ساكن بیعلامت دراز ffb2[] = { 0 };
ساكن بیعلامت دراز ffb3[] = { 1 };
ساكن بیعلامت دراز ffb4[] = { 1UL << (BPL - 1) };
ساكن بیعلامت دراز ffb5[] = { 0, 0x10 };
ساكن بیعلامت دراز ffb6[] = { 0, 0, 1UL << (BPL - 1) };
ساكن بیعلامت دراز ffb7[] = { 0, 0, 0, 0x01 };

ساكن فراغ پیدا_کن_اولی_بیت_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_اولی_بیت(ffb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_اولی_بیت(ffb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_اولی_بیت(ffb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_اولی_بیت(ffb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_اولی_بیت(ffb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_اولی_بیت(ffb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_اولی_بیت(ffb7, 4 * BPL), 3 * BPL);
}

ساكن بیعلامت دراز ffzb1[] = {};
ساكن بیعلامت دراز ffzb2[] = { ~0UL };
ساكن بیعلامت دراز ffzb3[] = { ~1UL };
ساكن بیعلامت دراز ffzb4[] = { ~(1UL << (BPL - 1)) };
ساكن بیعلامت دراز ffzb5[] = { ~0UL, ~0x10UL };
ساكن بیعلامت دراز ffzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ساكن بیعلامت دراز ffzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ساكن فراغ پیدا_کن_اولی_zero_بیت_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_ASSERT_EQ(test, پیدا_کن_اولی_zero_بیت(ffzb1, 0), 0);
	SBIUNIT_ASSERT_EQ(test, پیدا_کن_اولی_zero_بیت(ffzb2, BPL), BPL);
	SBIUNIT_ASSERT_EQ(test, پیدا_کن_اولی_zero_بیت(ffzb3, BPL), 0);
	SBIUNIT_ASSERT_EQ(test, پیدا_کن_اولی_zero_بیت(ffzb4, BPL), BPL - 1);
	SBIUNIT_ASSERT_EQ(test, پیدا_کن_اولی_zero_بیت(ffzb5, 2 * BPL), BPL + 4);
	SBIUNIT_ASSERT_EQ(test, پیدا_کن_اولی_zero_بیت(ffzb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_ASSERT_EQ(test, پیدا_کن_اولی_zero_بیت(ffzb7, 4 * BPL), 3 * BPL);
}

ساكن بیعلامت دراز flb1[] = {};
ساكن بیعلامت دراز flb2[] = { 0 };
ساكن بیعلامت دراز flb3[] = { 1 };
ساكن بیعلامت دراز flb4[] = { 1UL << (BPL - 1) };
ساكن بیعلامت دراز flb5[] = { 0, 0x10 };
ساكن بیعلامت دراز flb6[] = { 0, 0, 1UL << (BPL - 1) };
ساكن بیعلامت دراز flb7[] = { 0, 0, 0, 0x01 };

ساكن فراغ پیدا_کن_آخرین_بیت_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_آخرین_بیت(flb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_آخرین_بیت(flb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_آخرین_بیت(flb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_آخرین_بیت(flb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_آخرین_بیت(flb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_آخرین_بیت(flb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_آخرین_بیت(flb7, 4 * BPL), 3 * BPL);
}

ساكن بیعلامت دراز fnb1[] = {};
ساكن بیعلامت دراز fnb2[] = { 0 };
ساكن بیعلامت دراز fnb3[] = { 1 };
ساكن بیعلامت دراز fnb4[] = { 1UL << (BPL - 1) };
ساكن بیعلامت دراز fnb5[] = { 0, 0x10 };
ساكن بیعلامت دراز fnb6[] = { 0, 0, 1UL << (BPL - 1) };
ساكن بیعلامت دراز fnb7[] = { 0, 0, 0, 0x01 };

ساكن فراغ پیدا_کن_بعدی_بیت_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_بیت(fnb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_بیت(fnb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_بیت(fnb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_بیت(fnb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_بیت(fnb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_بیت(fnb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_بیت(fnb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_بیت(fnb5, 2 * BPL, BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_بیت(fnb7, 4 * BPL, 3 * BPL), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_بیت(fnb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ساكن بیعلامت دراز fnzb1[] = {};
ساكن بیعلامت دراز fnzb2[] = { ~0UL };
ساكن بیعلامت دراز fnzb3[] = { ~1UL };
ساكن بیعلامت دراز fnzb4[] = { ~(1UL << (BPL - 1)) };
ساكن بیعلامت دراز fnzb5[] = { ~0UL, ~0x10UL };
ساكن بیعلامت دراز fnzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ساكن بیعلامت دراز fnzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ساكن فراغ پیدا_کن_بعدی_zero_بیت_test(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_zero_بیت(fnzb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_zero_بیت(fnzb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_zero_بیت(fnzb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_zero_بیت(fnzb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_zero_بیت(fnzb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_zero_بیت(fnzb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_zero_بیت(fnzb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_zero_بیت(fnzb5, 2 * BPL, BPL),
			  BPL + 4);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_zero_بیت(fnzb7, 4 * BPL, 3 * BPL),
			  3 * BPL);
	SBIUNIT_EXPECT_EQ(test, پیدا_کن_بعدی_zero_بیت(fnzb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ساكن بنية sbiunit_test_case bitops_test_cases[] = {
	SBIUNIT_TEST_CASE(پیدا_کن_اولی_بیت_test),
	SBIUNIT_TEST_CASE(پیدا_کن_اولی_zero_بیت_test),
	SBIUNIT_TEST_CASE(پیدا_کن_آخرین_بیت_test),
	SBIUNIT_TEST_CASE(پیدا_کن_بعدی_بیت_test),
	SBIUNIT_TEST_CASE(پیدا_کن_بعدی_zero_بیت_test),
	SBIUNIT_انتها_CASE,
};

SBIUNIT_TEST_SUITE(bitops_test_suite, bitops_test_cases);
