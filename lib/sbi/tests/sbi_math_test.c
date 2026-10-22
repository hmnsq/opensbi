/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright  2024  Beijing ESWIN Computing Technology Co., Ltd.
 *
 * Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
 */
#ضمن <sbi/sbi_math.h>
#ضمن <sbi/sbi_unit_test.h>

ساكن فراغ log2roundup_test(بنية sbiunit_test_case *test)
{
    بنية {
        بنانشان ڊگهو داخلا_3;
        بنانشان ڊگهو expected;
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

    لكل (صحيح i = 0; i < حجم(cases)/حجم(cases[0]); i++) {
        بنانشان ڊگهو نتيجو = log2roundup(cases[i].داخلا_3);
        SBIUNIT_EXPECT_EQ(test, نتيجو, cases[i].expected);
    }
}

ساكن بنية sbiunit_test_case math_test_cases[] = {
    SBIUNIT_TEST_CASE(log2roundup_test),
    SBIUNIT_پڇاڙي_CASE,
};

SBIUNIT_TEST_SUITE(math_test_suite, math_test_cases);
