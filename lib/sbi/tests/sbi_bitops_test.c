/*
* SPDX-License-Identifier: BSD-2-Clause
*
* Copyright 2025 Beijing ESWIN Computing Technology Co., Ltd.
*
* Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
*/

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unit_test.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BPL ਬਿੱਟ_ਪ੍ਰਤੀ_LONG

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਬਿੱਟ_ਮੰਜ਼ਿਲ_ਖੋਜ = 64;

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffb1[] = {};
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffb2[] = { 0 };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffb3[] = { 1 };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffb4[] = { 1UL << (BPL - 1) };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffb5[] = { 0, 0x10 };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffb6[] = { 0, 0, 1UL << (BPL - 1) };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffb7[] = { 0, 0, 0, 0x01 };

ਸਥਿਰ ਖਾਲੀ ਲੱਭੋ_ਪਹਿਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ_test(ਬਣਤਰ sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(ffb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(ffb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(ffb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(ffb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(ffb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(ffb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(ffb7, 4 * BPL), 3 * BPL);
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffzb1[] = {};
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffzb2[] = { ~0UL };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffzb3[] = { ~1UL };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffzb4[] = { ~(1UL << (BPL - 1)) };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffzb5[] = { ~0UL, ~0x10UL };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ffzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ਸਥਿਰ ਖਾਲੀ ਲੱਭੋ_ਪਹਿਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ_test(ਬਣਤਰ sbiunit_test_case *test)
{
	SBIUNIT_ASSERT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(ffzb1, 0), 0);
	SBIUNIT_ASSERT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(ffzb2, BPL), BPL);
	SBIUNIT_ASSERT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(ffzb3, BPL), 0);
	SBIUNIT_ASSERT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(ffzb4, BPL), BPL - 1);
	SBIUNIT_ASSERT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(ffzb5, 2 * BPL), BPL + 4);
	SBIUNIT_ASSERT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(ffzb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_ASSERT_EQ(test, ਲੱਭੋ_ਪਹਿਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(ffzb7, 4 * BPL), 3 * BPL);
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flb1[] = {};
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flb2[] = { 0 };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flb3[] = { 1 };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flb4[] = { 1UL << (BPL - 1) };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flb5[] = { 0, 0x10 };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flb6[] = { 0, 0, 1UL << (BPL - 1) };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flb7[] = { 0, 0, 0, 0x01 };

ਸਥਿਰ ਖਾਲੀ ਲੱਭੋ_ਆਖਰੀ_ਦੋਆਧਾਰੀ_ਅੰਕ_test(ਬਣਤਰ sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਆਖਰੀ_ਦੋਆਧਾਰੀ_ਅੰਕ(flb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਆਖਰੀ_ਦੋਆਧਾਰੀ_ਅੰਕ(flb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਆਖਰੀ_ਦੋਆਧਾਰੀ_ਅੰਕ(flb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਆਖਰੀ_ਦੋਆਧਾਰੀ_ਅੰਕ(flb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਆਖਰੀ_ਦੋਆਧਾਰੀ_ਅੰਕ(flb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਆਖਰੀ_ਦੋਆਧਾਰੀ_ਅੰਕ(flb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਆਖਰੀ_ਦੋਆਧਾਰੀ_ਅੰਕ(flb7, 4 * BPL), 3 * BPL);
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnb1[] = {};
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnb2[] = { 0 };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnb3[] = { 1 };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnb4[] = { 1UL << (BPL - 1) };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnb5[] = { 0, 0x10 };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnb6[] = { 0, 0, 1UL << (BPL - 1) };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnb7[] = { 0, 0, 0, 0x01 };

ਸਥਿਰ ਖਾਲੀ ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ_test(ਬਣਤਰ sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(fnb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(fnb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(fnb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(fnb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(fnb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(fnb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(fnb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(fnb5, 2 * BPL, BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(fnb7, 4 * BPL, 3 * BPL), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(fnb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnzb1[] = {};
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnzb2[] = { ~0UL };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnzb3[] = { ~1UL };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnzb4[] = { ~(1UL << (BPL - 1)) };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnzb5[] = { ~0UL, ~0x10UL };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fnzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

ਸਥਿਰ ਖਾਲੀ ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ_test(ਬਣਤਰ sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(fnzb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(fnzb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(fnzb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(fnzb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(fnzb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(fnzb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(fnzb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(fnzb5, 2 * BPL, BPL),
			  BPL + 4);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(fnzb7, 4 * BPL, 3 * BPL),
			  3 * BPL);
	SBIUNIT_EXPECT_EQ(test, ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(fnzb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

ਸਥਿਰ ਬਣਤਰ sbiunit_test_case bitops_test_cases[] = {
	SBIUNIT_TEST_CASE(ਲੱਭੋ_ਪਹਿਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ_test),
	SBIUNIT_TEST_CASE(ਲੱਭੋ_ਪਹਿਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ_test),
	SBIUNIT_TEST_CASE(ਲੱਭੋ_ਆਖਰੀ_ਦੋਆਧਾਰੀ_ਅੰਕ_test),
	SBIUNIT_TEST_CASE(ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ_test),
	SBIUNIT_TEST_CASE(ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ_test),
	SBIUNIT_ਸਿਰਾ_CASE,
};

SBIUNIT_TEST_SUITE(bitops_test_suite, bitops_test_cases);
