/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright  2024  Beijing ESWIN Computing Technology Co., Ltd.
 *
 * Author: Dongdong Zhang <zhangdongdong@eswincomputing.com>
 */
#ಅನ್ತರ್ಭಾವ <sbi/sbi_math.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_unit_test.h>

ಸ್ಥಿರ ಶೂನ್ಯ log2roundup_test(ರಚನೆ sbiunit_test_case *test)
{
    ರಚನೆ {
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಒಳಹರಿವು;
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ expected;
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

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಗಾತ್ರ(cases)/ಗಾತ್ರ(cases[0]); i++) {
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಫಲಿತಾಂಶ = log2roundup(cases[i].ಒಳಹರಿವು);
        SBIUNIT_EXPECT_EQ(test, ಫಲಿತಾಂಶ, cases[i].expected);
    }
}

ಸ್ಥಿರ ರಚನೆ sbiunit_test_case math_test_cases[] = {
    SBIUNIT_TEST_CASE(log2roundup_test),
    SBIUNIT_ಕೊನೆ_CASE,
};

SBIUNIT_TEST_SUITE(math_test_suite, math_test_cases);
