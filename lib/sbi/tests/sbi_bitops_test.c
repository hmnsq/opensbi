/*
* SPDX-License-Identifier: BSD-2-Clause
*
* Copyright 2025 Beijing ESWIN Computing Technology Co., Ltd.
*
* Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
*/

#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_unit_test.h>

#నిర్వచించు BPL బిట్లు_చొప్పున_LONG

గుర్తులేని దీర్ఘ బిట్లు_గమ్యం_శోధన = 64;

స్థిర గుర్తులేని దీర్ఘ ffb1[] = {};
స్థిర గుర్తులేని దీర్ఘ ffb2[] = { 0 };
స్థిర గుర్తులేని దీర్ఘ ffb3[] = { 1 };
స్థిర గుర్తులేని దీర్ఘ ffb4[] = { 1UL << (BPL - 1) };
స్థిర గుర్తులేని దీర్ఘ ffb5[] = { 0, 0x10 };
స్థిర గుర్తులేని దీర్ఘ ffb6[] = { 0, 0, 1UL << (BPL - 1) };
స్థిర గుర్తులేని దీర్ఘ ffb7[] = { 0, 0, 0, 0x01 };

స్థిర శూన్య కనుగొను_మొదటి_బిట్టు_test(నిర్మాణం sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, కనుగొను_మొదటి_బిట్టు(ffb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_మొదటి_బిట్టు(ffb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_మొదటి_బిట్టు(ffb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_మొదటి_బిట్టు(ffb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_మొదటి_బిట్టు(ffb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_మొదటి_బిట్టు(ffb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_మొదటి_బిట్టు(ffb7, 4 * BPL), 3 * BPL);
}

స్థిర గుర్తులేని దీర్ఘ ffzb1[] = {};
స్థిర గుర్తులేని దీర్ఘ ffzb2[] = { ~0UL };
స్థిర గుర్తులేని దీర్ఘ ffzb3[] = { ~1UL };
స్థిర గుర్తులేని దీర్ఘ ffzb4[] = { ~(1UL << (BPL - 1)) };
స్థిర గుర్తులేని దీర్ఘ ffzb5[] = { ~0UL, ~0x10UL };
స్థిర గుర్తులేని దీర్ఘ ffzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
స్థిర గుర్తులేని దీర్ఘ ffzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

స్థిర శూన్య కనుగొను_మొదటి_zero_బిట్టు_test(నిర్మాణం sbiunit_test_case *test)
{
	SBIUNIT_ASSERT_EQ(test, కనుగొను_మొదటి_zero_బిట్టు(ffzb1, 0), 0);
	SBIUNIT_ASSERT_EQ(test, కనుగొను_మొదటి_zero_బిట్టు(ffzb2, BPL), BPL);
	SBIUNIT_ASSERT_EQ(test, కనుగొను_మొదటి_zero_బిట్టు(ffzb3, BPL), 0);
	SBIUNIT_ASSERT_EQ(test, కనుగొను_మొదటి_zero_బిట్టు(ffzb4, BPL), BPL - 1);
	SBIUNIT_ASSERT_EQ(test, కనుగొను_మొదటి_zero_బిట్టు(ffzb5, 2 * BPL), BPL + 4);
	SBIUNIT_ASSERT_EQ(test, కనుగొను_మొదటి_zero_బిట్టు(ffzb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_ASSERT_EQ(test, కనుగొను_మొదటి_zero_బిట్టు(ffzb7, 4 * BPL), 3 * BPL);
}

స్థిర గుర్తులేని దీర్ఘ flb1[] = {};
స్థిర గుర్తులేని దీర్ఘ flb2[] = { 0 };
స్థిర గుర్తులేని దీర్ఘ flb3[] = { 1 };
స్థిర గుర్తులేని దీర్ఘ flb4[] = { 1UL << (BPL - 1) };
స్థిర గుర్తులేని దీర్ఘ flb5[] = { 0, 0x10 };
స్థిర గుర్తులేని దీర్ఘ flb6[] = { 0, 0, 1UL << (BPL - 1) };
స్థిర గుర్తులేని దీర్ఘ flb7[] = { 0, 0, 0, 0x01 };

స్థిర శూన్య కనుగొను_చివరి_బిట్టు_test(నిర్మాణం sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, కనుగొను_చివరి_బిట్టు(flb1, 0), 0);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_చివరి_బిట్టు(flb2, BPL), BPL);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_చివరి_బిట్టు(flb3, BPL), 0);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_చివరి_బిట్టు(flb4, BPL), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_చివరి_బిట్టు(flb5, 2 * BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_చివరి_బిట్టు(flb6, 3 * BPL),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_చివరి_బిట్టు(flb7, 4 * BPL), 3 * BPL);
}

స్థిర గుర్తులేని దీర్ఘ fnb1[] = {};
స్థిర గుర్తులేని దీర్ఘ fnb2[] = { 0 };
స్థిర గుర్తులేని దీర్ఘ fnb3[] = { 1 };
స్థిర గుర్తులేని దీర్ఘ fnb4[] = { 1UL << (BPL - 1) };
స్థిర గుర్తులేని దీర్ఘ fnb5[] = { 0, 0x10 };
స్థిర గుర్తులేని దీర్ఘ fnb6[] = { 0, 0, 1UL << (BPL - 1) };
స్థిర గుర్తులేని దీర్ఘ fnb7[] = { 0, 0, 0, 0x01 };

స్థిర శూన్య కనుగొను_తదుపరి_బిట్టు_test(నిర్మాణం sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_బిట్టు(fnb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_బిట్టు(fnb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_బిట్టు(fnb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_బిట్టు(fnb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_బిట్టు(fnb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_బిట్టు(fnb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_బిట్టు(fnb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_బిట్టు(fnb5, 2 * BPL, BPL), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_బిట్టు(fnb7, 4 * BPL, 3 * BPL), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_బిట్టు(fnb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

స్థిర గుర్తులేని దీర్ఘ fnzb1[] = {};
స్థిర గుర్తులేని దీర్ఘ fnzb2[] = { ~0UL };
స్థిర గుర్తులేని దీర్ఘ fnzb3[] = { ~1UL };
స్థిర గుర్తులేని దీర్ఘ fnzb4[] = { ~(1UL << (BPL - 1)) };
స్థిర గుర్తులేని దీర్ఘ fnzb5[] = { ~0UL, ~0x10UL };
స్థిర గుర్తులేని దీర్ఘ fnzb6[] = { ~0UL, ~0UL, ~(1UL << (BPL - 1)) };
స్థిర గుర్తులేని దీర్ఘ fnzb7[] = { ~0UL, ~0UL, ~0UL, ~0x01UL };

స్థిర శూన్య కనుగొను_తదుపరి_zero_బిట్టు_test(నిర్మాణం sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_zero_బిట్టు(fnzb1, 0, 0), 0);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_zero_బిట్టు(fnzb2, BPL, 0), BPL);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_zero_బిట్టు(fnzb3, BPL, 0), 0);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_zero_బిట్టు(fnzb4, BPL, 0), BPL - 1);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_zero_బిట్టు(fnzb5, 2 * BPL, 0), BPL + 4);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_zero_బిట్టు(fnzb6, 3 * BPL, 0),
			  2 * BPL + BPL - 1);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_zero_బిట్టు(fnzb7, 4 * BPL, 0), 3 * BPL);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_zero_బిట్టు(fnzb5, 2 * BPL, BPL),
			  BPL + 4);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_zero_బిట్టు(fnzb7, 4 * BPL, 3 * BPL),
			  3 * BPL);
	SBIUNIT_EXPECT_EQ(test, కనుగొను_తదుపరి_zero_బిట్టు(fnzb6, 3 * BPL, BPL),
			  2 * BPL + BPL - 1);
}

స్థిర నిర్మాణం sbiunit_test_case bitops_test_cases[] = {
	SBIUNIT_TEST_CASE(కనుగొను_మొదటి_బిట్టు_test),
	SBIUNIT_TEST_CASE(కనుగొను_మొదటి_zero_బిట్టు_test),
	SBIUNIT_TEST_CASE(కనుగొను_చివరి_బిట్టు_test),
	SBIUNIT_TEST_CASE(కనుగొను_తదుపరి_బిట్టు_test),
	SBIUNIT_TEST_CASE(కనుగొను_తదుపరి_zero_బిట్టు_test),
	SBIUNIT_చివర_CASE,
};

SBIUNIT_TEST_SUITE(bitops_test_suite, bitops_test_cases);
