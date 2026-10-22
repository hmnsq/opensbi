/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright  2024  Beijing ESWIN Computing Technology Co., Ltd.
 *
 * Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unit_test.h>

ਸਥਿਰ ਖਾਲੀ log2roundup_test(ਬਣਤਰ sbiunit_test_case *test)
{
    ਬਣਤਰ {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਿਵੇਸ਼;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ expected;
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

    ਲਈ (ਪੂਰਨ i = 0; i < ਆਕਾਰ(cases)/ਆਕਾਰ(cases[0]); i++) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਤੀਜਾ = log2roundup(cases[i].ਨਿਵੇਸ਼);
        SBIUNIT_EXPECT_EQ(test, ਨਤੀਜਾ, cases[i].expected);
    }
}

ਸਥਿਰ ਬਣਤਰ sbiunit_test_case math_test_cases[] = {
    SBIUNIT_TEST_CASE(log2roundup_test),
    SBIUNIT_ਸਿਰਾ_CASE,
};

SBIUNIT_TEST_SUITE(math_test_suite, math_test_cases);
