/*
* SPDX-License-Identifier: BSD-2-Clause
*
* Copyright 2025 Beijing ESWIN Computing Technology Co., Ltd.
*
* Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
*/

#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_unit_test.h>

#define BPL BITS_PER_LONG

अचिन्हित दीर्घ bits_to_खोज = 64;

स्थिर अचिन्हित दीर्घ ffb1[] = {};
स्थिर अचिन्हित दीर्घ ffb2[] = { 0 };
स्थिर अचिन्हित दीर्घ ffb3[] = { 1 };
स्थिर अचिन्हित दीर्घ ffb4[] = { 1UL << (BPL - 1) };
स्थिर अचिन्हित दीर्घ ffb5[] = { 0, 0x10 };
स्थिर अचिन्हित दीर्घ ffb6[] = { 0, 0, 1UL << (BPL - 1) };
स्थिर अचिन्हित दीर्घ ffb7[] = { 0, 0, 0, 0x01 };

स्थिर शून्य find_first_bit_test(संरचना sbiunit_test_case *test)
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

स्थिर अचिन्हित दीर्घ ffzb1[] = {};
स्थिर अचिन्हित दीर्घ ffzb2[] = { ~0UL };
स्थिर अचिन्हित दीर्घ ffzb3[] = { ~1UL };
स्थिर अचिन्हित दीर्घ ffzb4[] = { ~(1UL << (BPL - 1)) };
स्थिर अचिन्हित दीर्घ ffzb5[] = { ~0UL, ~0x10UL };
स्थिर अचिन्हित दीर्घ ffzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
स्थिर अचिन्हित दीर्घ ffzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

स्थिर शून्य find_first_zero_bit_test(संरचना sbiunit_test_case *test)
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

स्थिर अचिन्हित दीर्घ flb1[] = {};
स्थिर अचिन्हित दीर्घ flb2[] = { 0 };
स्थिर अचिन्हित दीर्घ flb3[] = { 1 };
स्थिर अचिन्हित दीर्घ flb4[] = { 1UL << (BPL - 1) };
स्थिर अचिन्हित दीर्घ flb5[] = { 0, 0x10 };
स्थिर अचिन्हित दीर्घ flb6[] = { 0, 0, 1UL << (BPL - 1) };
स्थिर अचिन्हित दीर्घ flb7[] = { 0, 0, 0, 0x01 };

स्थिर शून्य find_last_bit_test(संरचना sbiunit_test_case *test)
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

स्थिर अचिन्हित दीर्घ fnb1[] = {};
स्थिर अचिन्हित दीर्घ fnb2[] = { 0 };
स्थिर अचिन्हित दीर्घ fnb3[] = { 1 };
स्थिर अचिन्हित दीर्घ fnb4[] = { 1UL << (BPL - 1) };
स्थिर अचिन्हित दीर्घ fnb5[] = { 0, 0x10 };
स्थिर अचिन्हित दीर्घ fnb6[] = { 0, 0, 1UL << (BPL - 1) };
स्थिर अचिन्हित दीर्घ fnb7[] = { 0, 0, 0, 0x01 };

स्थिर शून्य find_next_bit_test(संरचना sbiunit_test_case *test)
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

स्थिर अचिन्हित दीर्घ fnzb1[] = {};
स्थिर अचिन्हित दीर्घ fnzb2[] = { ~0UL };
स्थिर अचिन्हित दीर्घ fnzb3[] = { ~1UL };
स्थिर अचिन्हित दीर्घ fnzb4[] = { ~(1UL << (BPL - 1)) };
स्थिर अचिन्हित दीर्घ fnzb5[] = { ~0UL, ~0x10UL };
स्थिर अचिन्हित दीर्घ fnzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
स्थिर अचिन्हित दीर्घ fnzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

स्थिर शून्य find_next_zero_bit_test(संरचना sbiunit_test_case *test)
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

स्थिर संरचना sbiunit_test_case bitops_test_cases[] = {
	SBIUNIT_TEST_CASE(find_first_bit_test),
	SBIUNIT_TEST_CASE(find_first_zero_bit_test),
	SBIUNIT_TEST_CASE(find_last_bit_test),
	SBIUNIT_TEST_CASE(find_next_bit_test),
	SBIUNIT_TEST_CASE(find_next_zero_bit_test),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(bitops_test_suite, bitops_test_cases);
