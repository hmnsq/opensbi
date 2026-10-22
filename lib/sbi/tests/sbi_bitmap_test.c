/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitmap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unit_test.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕੜੇ_ਮਾਪ ਆਕਾਰ(ਅੰਕੜੇ_zero)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ (ਅੰਕੜੇ_ਮਾਪ * 8)

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅੰਕੜੇ_a[] = { 0xDEADBEEF, 0x00BAB10C, 0x1BADB002, 0xABADBABE };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅੰਕੜੇ_b[] = { 0xC00010FF, 0x00BAB10C, 0xBAAAAAAD, 0xBADDCAFE };
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅੰਕੜੇ_zero[] = { 0, 0, 0, 0 };

ਸਥਿਰ ਖਾਲੀ bitmap_and_test(ਬਣਤਰ sbiunit_test_case *test)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ res[ਅੰਕੜੇ_ਮਾਪ];
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a_and_b[] = { ਅੰਕੜੇ_a[0] & ਅੰਕੜੇ_b[0], ਅੰਕੜੇ_a[1] & ਅੰਕੜੇ_b[1],
				    ਅੰਕੜੇ_a[2] & ਅੰਕੜੇ_b[2], ਅੰਕੜੇ_a[3] & ਅੰਕੜੇ_b[3] };

	__bitmap_and(res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_b, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_and_b, ਅੰਕੜੇ_ਮਾਪ);

	/* a & a = a */
	__bitmap_and(res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_a, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_ASSERT_MEMEQ(test, res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_ਮਾਪ);

	/* a & 0 = 0 */
	__bitmap_and(res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਮਾਪ);

	/* 0 & 0 = 0 */
	__bitmap_and(res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਮਾਪ);

	sbi_memcpy(res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਮਾਪ);
	/* Cover zero 'bits' argument */
	__bitmap_and(res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ bitmap_or_test(ਬਣਤਰ sbiunit_test_case *test)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ res[ਅੰਕੜੇ_ਮਾਪ];
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a_or_b[] = { ਅੰਕੜੇ_a[0] | ਅੰਕੜੇ_b[0], ਅੰਕੜੇ_a[1] | ਅੰਕੜੇ_b[1],
				   ਅੰਕੜੇ_a[2] | ਅੰਕੜੇ_b[2], ਅੰਕੜੇ_a[3] | ਅੰਕੜੇ_b[3] };

	__bitmap_or(res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_b, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_or_b, ਅੰਕੜੇ_ਮਾਪ);

	/* a | a = a */
	__bitmap_or(res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_a, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_ਮਾਪ);

	/* a | 0 = a */
	__bitmap_or(res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_ਮਾਪ);

	/* 0 | 0 = 0 */
	__bitmap_or(res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਮਾਪ);

	sbi_memcpy(res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਮਾਪ);
	__bitmap_or(res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ bitmap_xor_test(ਬਣਤਰ sbiunit_test_case *test)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ res[ਅੰਕੜੇ_ਮਾਪ];
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a_xor_b[] = { ਅੰਕੜੇ_a[0] ^ ਅੰਕੜੇ_b[0], ਅੰਕੜੇ_a[1] ^ ਅੰਕੜੇ_b[1],
				    ਅੰਕੜੇ_a[2] ^ ਅੰਕੜੇ_b[2], ਅੰਕੜੇ_a[3] ^ ਅੰਕੜੇ_b[3] };

	__bitmap_xor(res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_b, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_xor_b, ਅੰਕੜੇ_ਮਾਪ);

	/* a ^ 0 = a */
	__bitmap_xor(res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_ਮਾਪ);

	/* a ^ a = 0 */
	__bitmap_xor(res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_a, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਮਾਪ);

	/* 0 ^ 0 = 0 */
	__bitmap_xor(res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_MEMEQ(test, res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਮਾਪ);

	sbi_memcpy(res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਮਾਪ);
	__bitmap_xor(res, ਅੰਕੜੇ_a, ਅੰਕੜੇ_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ bitmap_ਖਾਲੀ_ਥਾਵਾਂ_test(ਬਣਤਰ sbiunit_test_case *test)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ res[ਅੰਕੜੇ_ਮਾਪ];

	/* All zeros = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_ਖਾਲੀ_ਥਾਵਾਂ(ਅੰਕੜੇ_zero, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ), true);

	/* Non-zero data = not empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_ਖਾਲੀ_ਥਾਵਾਂ(ਅੰਕੜੇ_a, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ), false);
	SBIUNIT_EXPECT_EQ(test, bitmap_ਖਾਲੀ_ਥਾਵਾਂ(ਅੰਕੜੇ_b, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ), false);

	/* bitmap_zero creates empty bitmap */
	bitmap_zero(res, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_EQ(test, bitmap_ਖਾਲੀ_ਥਾਵਾਂ(res, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ), true);

	/* bitmap_fill creates non-empty bitmap */
	bitmap_fill(res, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	SBIUNIT_EXPECT_EQ(test, bitmap_ਖਾਲੀ_ਥਾਵਾਂ(res, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ), false);

	/* Single bit set = not empty */
	bitmap_zero(res, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ);
	bitmap_ਸੈੱਟ(res, 0, 1);
	SBIUNIT_EXPECT_EQ(test, bitmap_ਖਾਲੀ_ਥਾਵਾਂ(res, ਅੰਕੜੇ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮਾਪ), false);

	/* Zero nbits = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_ਖਾਲੀ_ਥਾਵਾਂ(ਅੰਕੜੇ_a, 0), true);
}

ਸਥਿਰ ਬਣਤਰ sbiunit_test_case bitmap_test_cases[] = {
	SBIUNIT_TEST_CASE(bitmap_and_test),
	SBIUNIT_TEST_CASE(bitmap_or_test),
	SBIUNIT_TEST_CASE(bitmap_xor_test),
	SBIUNIT_TEST_CASE(bitmap_ਖਾਲੀ_ਥਾਵਾਂ_test),
	SBIUNIT_ਸਿਰਾ_CASE,
};

SBIUNIT_TEST_SUITE(bitmap_test_suite, bitmap_test_cases);
