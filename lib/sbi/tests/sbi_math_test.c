/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright  2024  Beijing ESWIN Computing Technology Co., Ltd.
 *
 * Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
 */
#അന്തര്ഭാവ <sbi/sbi_math.h>
#അന്തര്ഭാവ <sbi/sbi_unit_test.h>

സ്ഥിതം ശൂന്യം log2roundup_test(ഘടന sbiunit_test_case *test)
{
    ഘടന {
        unsigned long നിവേശം;
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

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < വലുപ്പം(cases)/വലുപ്പം(cases[0]); i++) {
        unsigned long result = log2roundup(cases[i].നിവേശം);
        SBIUNIT_EXPECT_EQ(test, result, cases[i].expected);
    }
}

സ്ഥിതം ഘടന sbiunit_test_case math_test_cases[] = {
    SBIUNIT_TEST_CASE(log2roundup_test),
    SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(math_test_suite, math_test_cases);
