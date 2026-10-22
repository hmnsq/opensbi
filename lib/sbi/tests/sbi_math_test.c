/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright  2024  Beijing ESWIN Computing Technology Co., Ltd.
 *
 * Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
 */
#समावेश <sbi/sbi_math.h>
#समावेश <sbi/sbi_unit_test.h>

अटल व्योम log2roundup_test(काष्ठा sbiunit_test_case *test)
{
    काष्ठा {
        अचिन्हित दीर्घ निवेश;
        अचिन्हित दीर्घ expected;
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

    क्रम (पूर्णांक i = 0; i < माप(cases)/माप(cases[0]); i++) {
        अचिन्हित दीर्घ परिणाम = log2roundup(cases[i].निवेश);
        SBIUNIT_EXPECT_EQ(test, परिणाम, cases[i].expected);
    }
}

अटल काष्ठा sbiunit_test_case math_test_cases[] = {
    SBIUNIT_TEST_CASE(log2roundup_test),
    SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(math_test_suite, math_test_cases);
