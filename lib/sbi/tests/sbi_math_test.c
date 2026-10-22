/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright  2024  Beijing ESWIN Computing Technology Co., Ltd.
 *
 * Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
 */
#شامل <sbi/sbi_math.h>
#شامل <sbi/sbi_unit_test.h>

ساکن خالی log2roundup_test(ساخت sbiunit_test_case *test)
{
    ساخت {
        غیرعلامتی طویل داخلہ;
        غیرعلامتی طویل expected;
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

    برائے (صحیح i = 0; i < حجم(cases)/حجم(cases[0]); i++) {
        غیرعلامتی طویل نتیجہ = log2roundup(cases[i].داخلہ);
        SBIUNIT_EXPECT_EQ(test, نتیجہ, cases[i].expected);
    }
}

ساکن ساخت sbiunit_test_case math_test_cases[] = {
    SBIUNIT_TEST_CASE(log2roundup_test),
    SBIUNIT_انتہا_CASE,
};

SBIUNIT_TEST_SUITE(math_test_suite, math_test_cases);
