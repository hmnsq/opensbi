/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright  2024  Beijing ESWIN Computing Technology Co., Ltd.
 *
 * Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
 */
#అన్తర్భావ <sbi/sbi_math.h>
#అన్తర్భావ <sbi/sbi_unit_test.h>

స్థిర శూన్య log2roundup_test(నిర్మాణం sbiunit_test_case *test)
{
    నిర్మాణం {
        గుర్తులేని దీర్ఘ ఆదానం;
        గుర్తులేని దీర్ఘ expected;
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

    కొరకు (పూర్ణాంకం i = 0; i < పరిమాణం(cases)/పరిమాణం(cases[0]); i++) {
        గుర్తులేని దీర్ఘ ఫలితం = log2roundup(cases[i].ఆదానం);
        SBIUNIT_EXPECT_EQ(test, ఫలితం, cases[i].expected);
    }
}

స్థిర నిర్మాణం sbiunit_test_case math_test_cases[] = {
    SBIUNIT_TEST_CASE(log2roundup_test),
    SBIUNIT_చివర_CASE,
};

SBIUNIT_TEST_SUITE(math_test_suite, math_test_cases);
