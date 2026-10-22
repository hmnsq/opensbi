/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitmap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_unit_test.h>

#ವ್ಯಾಖ್ಯಾನಿಸು ದತ್ತಾಂಶ_ಗಾತ್ರ ಗಾತ್ರ(ದತ್ತಾಂಶ_zero)
#ವ್ಯಾಖ್ಯಾನಿಸು ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ (ದತ್ತಾಂಶ_ಗಾತ್ರ * 8)

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದತ್ತಾಂಶ_a[] = { 0xDEADBEEF, 0x00BAB10C, 0x1BADB002, 0xABADBABE };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದತ್ತಾಂಶ_b[] = { 0xC00010FF, 0x00BAB10C, 0xBAAAAAAD, 0xBADDCAFE };
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದತ್ತಾಂಶ_zero[] = { 0, 0, 0, 0 };

ಸ್ಥಿರ ಶೂನ್ಯ bitmap_and_test(ರಚನೆ sbiunit_test_case *test)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ res[ದತ್ತಾಂಶ_ಗಾತ್ರ];
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a_and_b[] = { ದತ್ತಾಂಶ_a[0] & ದತ್ತಾಂಶ_b[0], ದತ್ತಾಂಶ_a[1] & ದತ್ತಾಂಶ_b[1],
				    ದತ್ತಾಂಶ_a[2] & ದತ್ತಾಂಶ_b[2], ದತ್ತಾಂಶ_a[3] & ದತ್ತಾಂಶ_b[3] };

	__bitmap_and(res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_b, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_and_b, ದತ್ತಾಂಶ_ಗಾತ್ರ);

	/* a & a = a */
	__bitmap_and(res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_ASSERT_MEMEQ(test, res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_ಗಾತ್ರ);

	/* a & 0 = 0 */
	__bitmap_and(res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಗಾತ್ರ);

	/* 0 & 0 = 0 */
	__bitmap_and(res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಗಾತ್ರ);

	sbi_memcpy(res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಗಾತ್ರ);
	/* Cover zero 'bits' argument */
	__bitmap_and(res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಗಾತ್ರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ bitmap_or_test(ರಚನೆ sbiunit_test_case *test)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ res[ದತ್ತಾಂಶ_ಗಾತ್ರ];
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a_or_b[] = { ದತ್ತಾಂಶ_a[0] | ದತ್ತಾಂಶ_b[0], ದತ್ತಾಂಶ_a[1] | ದತ್ತಾಂಶ_b[1],
				   ದತ್ತಾಂಶ_a[2] | ದತ್ತಾಂಶ_b[2], ದತ್ತಾಂಶ_a[3] | ದತ್ತಾಂಶ_b[3] };

	__bitmap_or(res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_b, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_or_b, ದತ್ತಾಂಶ_ಗಾತ್ರ);

	/* a | a = a */
	__bitmap_or(res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_ಗಾತ್ರ);

	/* a | 0 = a */
	__bitmap_or(res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_ಗಾತ್ರ);

	/* 0 | 0 = 0 */
	__bitmap_or(res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಗಾತ್ರ);

	sbi_memcpy(res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಗಾತ್ರ);
	__bitmap_or(res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಗಾತ್ರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ bitmap_xor_test(ರಚನೆ sbiunit_test_case *test)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ res[ದತ್ತಾಂಶ_ಗಾತ್ರ];
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a_xor_b[] = { ದತ್ತಾಂಶ_a[0] ^ ದತ್ತಾಂಶ_b[0], ದತ್ತಾಂಶ_a[1] ^ ದತ್ತಾಂಶ_b[1],
				    ದತ್ತಾಂಶ_a[2] ^ ದತ್ತಾಂಶ_b[2], ದತ್ತಾಂಶ_a[3] ^ ದತ್ತಾಂಶ_b[3] };

	__bitmap_xor(res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_b, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_xor_b, ದತ್ತಾಂಶ_ಗಾತ್ರ);

	/* a ^ 0 = a */
	__bitmap_xor(res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_ಗಾತ್ರ);

	/* a ^ a = 0 */
	__bitmap_xor(res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಗಾತ್ರ);

	/* 0 ^ 0 = 0 */
	__bitmap_xor(res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಗಾತ್ರ);

	sbi_memcpy(res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಗಾತ್ರ);
	__bitmap_xor(res, ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಗಾತ್ರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ bitmap_ಖಾಲಿಗಳು_test(ರಚನೆ sbiunit_test_case *test)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ res[ದತ್ತಾಂಶ_ಗಾತ್ರ];

	/* All zeros = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_ಖಾಲಿಗಳು(ದತ್ತಾಂಶ_zero, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ), true);

	/* Non-zero data = not empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_ಖಾಲಿಗಳು(ದತ್ತಾಂಶ_a, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ), false);
	SBIUNIT_EXPECT_EQ(test, bitmap_ಖಾಲಿಗಳು(ದತ್ತಾಂಶ_b, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ), false);

	/* bitmap_zero creates empty bitmap */
	bitmap_zero(res, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_EQ(test, bitmap_ಖಾಲಿಗಳು(res, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ), true);

	/* bitmap_fill creates non-empty bitmap */
	bitmap_fill(res, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	SBIUNIT_EXPECT_EQ(test, bitmap_ಖಾಲಿಗಳು(res, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ), false);

	/* Single bit set = not empty */
	bitmap_zero(res, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ);
	bitmap_ಗಣ(res, 0, 1);
	SBIUNIT_EXPECT_EQ(test, bitmap_ಖಾಲಿಗಳು(res, ದತ್ತಾಂಶ_ಬಿಟ್_ಗಾತ್ರ), false);

	/* Zero nbits = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_ಖಾಲಿಗಳು(ದತ್ತಾಂಶ_a, 0), true);
}

ಸ್ಥಿರ ರಚನೆ sbiunit_test_case bitmap_test_cases[] = {
	SBIUNIT_TEST_CASE(bitmap_and_test),
	SBIUNIT_TEST_CASE(bitmap_or_test),
	SBIUNIT_TEST_CASE(bitmap_xor_test),
	SBIUNIT_TEST_CASE(bitmap_ಖಾಲಿಗಳು_test),
	SBIUNIT_ಕೊನೆ_CASE,
};

SBIUNIT_TEST_SUITE(bitmap_test_suite, bitmap_test_cases);
