/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#ضمن <sbi/sbi_bitmap.h>
#ضمن <sbi/sbi_unit_test.h>

#تعریف_کړه معلومات_اندازه حجم(معلومات_zero)
#تعریف_کړه معلومات_بټ_اندازه (معلومات_اندازه * 8)

ساكن بینښه اوږد معلومات_a[] = { 0xDEADBEEF, 0x00BAB10C, 0x1BADB002, 0xABADBABE };
ساكن بینښه اوږد معلومات_b[] = { 0xC00010FF, 0x00BAB10C, 0xBAAAAAAD, 0xBADDCAFE };
ساكن بینښه اوږد معلومات_zero[] = { 0, 0, 0, 0 };

ساكن فراغ bitmap_and_test(بنية sbiunit_test_case *test)
{
	بینښه اوږد res[معلومات_اندازه];
	بینښه اوږد a_and_b[] = { معلومات_a[0] & معلومات_b[0], معلومات_a[1] & معلومات_b[1],
				    معلومات_a[2] & معلومات_b[2], معلومات_a[3] & معلومات_b[3] };

	__bitmap_and(res, معلومات_a, معلومات_b, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_and_b, معلومات_اندازه);

	/* a & a = a */
	__bitmap_and(res, معلومات_a, معلومات_a, معلومات_بټ_اندازه);
	SBIUNIT_ASSERT_MEMEQ(test, res, معلومات_a, معلومات_اندازه);

	/* a & 0 = 0 */
	__bitmap_and(res, معلومات_a, معلومات_zero, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_اندازه);

	/* 0 & 0 = 0 */
	__bitmap_and(res, معلومات_zero, معلومات_zero, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_اندازه);

	sbi_memcpy(res, معلومات_zero, معلومات_اندازه);
	/* Cover zero 'bits' argument */
	__bitmap_and(res, معلومات_a, معلومات_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_اندازه);
}

ساكن فراغ bitmap_or_test(بنية sbiunit_test_case *test)
{
	بینښه اوږد res[معلومات_اندازه];
	بینښه اوږد a_or_b[] = { معلومات_a[0] | معلومات_b[0], معلومات_a[1] | معلومات_b[1],
				   معلومات_a[2] | معلومات_b[2], معلومات_a[3] | معلومات_b[3] };

	__bitmap_or(res, معلومات_a, معلومات_b, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_or_b, معلومات_اندازه);

	/* a | a = a */
	__bitmap_or(res, معلومات_a, معلومات_a, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_a, معلومات_اندازه);

	/* a | 0 = a */
	__bitmap_or(res, معلومات_a, معلومات_zero, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_a, معلومات_اندازه);

	/* 0 | 0 = 0 */
	__bitmap_or(res, معلومات_zero, معلومات_zero, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_اندازه);

	sbi_memcpy(res, معلومات_zero, معلومات_اندازه);
	__bitmap_or(res, معلومات_a, معلومات_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_اندازه);
}

ساكن فراغ bitmap_xor_test(بنية sbiunit_test_case *test)
{
	بینښه اوږد res[معلومات_اندازه];
	بینښه اوږد a_xor_b[] = { معلومات_a[0] ^ معلومات_b[0], معلومات_a[1] ^ معلومات_b[1],
				    معلومات_a[2] ^ معلومات_b[2], معلومات_a[3] ^ معلومات_b[3] };

	__bitmap_xor(res, معلومات_a, معلومات_b, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_xor_b, معلومات_اندازه);

	/* a ^ 0 = a */
	__bitmap_xor(res, معلومات_a, معلومات_zero, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_a, معلومات_اندازه);

	/* a ^ a = 0 */
	__bitmap_xor(res, معلومات_a, معلومات_a, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_اندازه);

	/* 0 ^ 0 = 0 */
	__bitmap_xor(res, معلومات_zero, معلومات_zero, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_اندازه);

	sbi_memcpy(res, معلومات_zero, معلومات_اندازه);
	__bitmap_xor(res, معلومات_a, معلومات_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_اندازه);
}

ساكن فراغ bitmap_تشه_test(بنية sbiunit_test_case *test)
{
	بینښه اوږد res[معلومات_اندازه];

	/* All zeros = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_تشه(معلومات_zero, معلومات_بټ_اندازه), true);

	/* Non-zero data = not empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_تشه(معلومات_a, معلومات_بټ_اندازه), false);
	SBIUNIT_EXPECT_EQ(test, bitmap_تشه(معلومات_b, معلومات_بټ_اندازه), false);

	/* bitmap_zero creates empty bitmap */
	bitmap_zero(res, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_EQ(test, bitmap_تشه(res, معلومات_بټ_اندازه), true);

	/* bitmap_fill creates non-empty bitmap */
	bitmap_fill(res, معلومات_بټ_اندازه);
	SBIUNIT_EXPECT_EQ(test, bitmap_تشه(res, معلومات_بټ_اندازه), false);

	/* Single bit set = not empty */
	bitmap_zero(res, معلومات_بټ_اندازه);
	bitmap_سېټ(res, 0, 1);
	SBIUNIT_EXPECT_EQ(test, bitmap_تشه(res, معلومات_بټ_اندازه), false);

	/* Zero nbits = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_تشه(معلومات_a, 0), true);
}

ساكن بنية sbiunit_test_case bitmap_test_cases[] = {
	SBIUNIT_TEST_CASE(bitmap_and_test),
	SBIUNIT_TEST_CASE(bitmap_or_test),
	SBIUNIT_TEST_CASE(bitmap_xor_test),
	SBIUNIT_TEST_CASE(bitmap_تشه_test),
	SBIUNIT_پای_ټکی_CASE,
};

SBIUNIT_TEST_SUITE(bitmap_test_suite, bitmap_test_cases);
