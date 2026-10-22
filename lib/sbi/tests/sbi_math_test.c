/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright  2024  Beijing ESWIN Computing Technology Co., Ltd.
 *
 * Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
 */
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_math.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_unit_test.h>

ସ୍ଥିର ଶୂନ୍ୟ log2roundup_test(ସଂରଚନା sbiunit_test_case *test)
{
    ସଂରଚନା {
        unsigned long ନିବେଶ;
        unsigned long expected;
    } cases[] = {
        {1, 0},
        {2, 1},
        {3, 2},
        {4, 2},
        {5, 3},
        {8, 3},
        {9, 4},
        {15, 4},
        {16, 4},
        {17, 5},
        {31, 5},
        {32, 5},
        {33, 6},
        {63, 6},
        {64, 6},
        {65, 7},
    };

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଆକାର(cases)/ଆକାର(cases[0]); i++) {
        unsigned long result = log2roundup(cases[i].ନିବେଶ);
        SBIUNIT_EXPECT_EQ(test, result, cases[i].expected);
    }
}

ସ୍ଥିର ସଂରଚନା sbiunit_test_case math_test_cases[] = {
    SBIUNIT_TEST_CASE(log2roundup_test),
    SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(math_test_suite, math_test_cases);
