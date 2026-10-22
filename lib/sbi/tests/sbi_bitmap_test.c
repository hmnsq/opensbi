/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#অন্তর্ভাব <sbi/sbi_bitmap.h>
#অন্তর্ভাব <sbi/sbi_unit_test.h>

#define তথ্য_আকার আকার(তথ্য_zero)
#define তথ্য_BIT_আকার (তথ্য_আকার * 8)

স্থির unsigned long তথ্য_a[] = { 0xDEADBEEF, 0x00BAB10C, 0x1BADB002, 0xABADBABE };
স্থির unsigned long তথ্য_b[] = { 0xC00010FF, 0x00BAB10C, 0xBAAAAAAD, 0xBADDCAFE };
স্থির unsigned long তথ্য_zero[] = { 0, 0, 0, 0 };

স্থির শূন্য bitmap_and_test(কাঠামো sbiunit_test_case *test)
{
	unsigned long res[তথ্য_আকার];
	unsigned long a_and_b[] = { তথ্য_a[0] & তথ্য_b[0], তথ্য_a[1] & তথ্য_b[1],
				    তথ্য_a[2] & তথ্য_b[2], তথ্য_a[3] & তথ্য_b[3] };

	__bitmap_and(res, তথ্য_a, তথ্য_b, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_and_b, তথ্য_আকার);

	/* a & a = a */
	__bitmap_and(res, তথ্য_a, তথ্য_a, তথ্য_BIT_আকার);
	SBIUNIT_ASSERT_MEMEQ(test, res, তথ্য_a, তথ্য_আকার);

	/* a & 0 = 0 */
	__bitmap_and(res, তথ্য_a, তথ্য_zero, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_MEMEQ(test, res, তথ্য_zero, তথ্য_আকার);

	/* 0 & 0 = 0 */
	__bitmap_and(res, তথ্য_zero, তথ্য_zero, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_MEMEQ(test, res, তথ্য_zero, তথ্য_আকার);

	sbi_memcpy(res, তথ্য_zero, তথ্য_আকার);
	/* Cover zero 'bits' argument */
	__bitmap_and(res, তথ্য_a, তথ্য_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, তথ্য_zero, তথ্য_আকার);
}

স্থির শূন্য bitmap_or_test(কাঠামো sbiunit_test_case *test)
{
	unsigned long res[তথ্য_আকার];
	unsigned long a_or_b[] = { তথ্য_a[0] | তথ্য_b[0], তথ্য_a[1] | তথ্য_b[1],
				   তথ্য_a[2] | তথ্য_b[2], তথ্য_a[3] | তথ্য_b[3] };

	__bitmap_or(res, তথ্য_a, তথ্য_b, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_or_b, তথ্য_আকার);

	/* a | a = a */
	__bitmap_or(res, তথ্য_a, তথ্য_a, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_MEMEQ(test, res, তথ্য_a, তথ্য_আকার);

	/* a | 0 = a */
	__bitmap_or(res, তথ্য_a, তথ্য_zero, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_MEMEQ(test, res, তথ্য_a, তথ্য_আকার);

	/* 0 | 0 = 0 */
	__bitmap_or(res, তথ্য_zero, তথ্য_zero, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_MEMEQ(test, res, তথ্য_zero, তথ্য_আকার);

	sbi_memcpy(res, তথ্য_zero, তথ্য_আকার);
	__bitmap_or(res, তথ্য_a, তথ্য_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, তথ্য_zero, তথ্য_আকার);
}

স্থির শূন্য bitmap_xor_test(কাঠামো sbiunit_test_case *test)
{
	unsigned long res[তথ্য_আকার];
	unsigned long a_xor_b[] = { তথ্য_a[0] ^ তথ্য_b[0], তথ্য_a[1] ^ তথ্য_b[1],
				    তথ্য_a[2] ^ তথ্য_b[2], তথ্য_a[3] ^ তথ্য_b[3] };

	__bitmap_xor(res, তথ্য_a, তথ্য_b, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_xor_b, তথ্য_আকার);

	/* a ^ 0 = a */
	__bitmap_xor(res, তথ্য_a, তথ্য_zero, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_MEMEQ(test, res, তথ্য_a, তথ্য_আকার);

	/* a ^ a = 0 */
	__bitmap_xor(res, তথ্য_a, তথ্য_a, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_MEMEQ(test, res, তথ্য_zero, তথ্য_আকার);

	/* 0 ^ 0 = 0 */
	__bitmap_xor(res, তথ্য_zero, তথ্য_zero, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_MEMEQ(test, res, তথ্য_zero, তথ্য_আকার);

	sbi_memcpy(res, তথ্য_zero, তথ্য_আকার);
	__bitmap_xor(res, তথ্য_a, তথ্য_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, তথ্য_zero, তথ্য_আকার);
}

স্থির শূন্য bitmap_empty_test(কাঠামো sbiunit_test_case *test)
{
	unsigned long res[তথ্য_আকার];

	/* All zeros = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(তথ্য_zero, তথ্য_BIT_আকার), true);

	/* Non-zero data = not empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(তথ্য_a, তথ্য_BIT_আকার), false);
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(তথ্য_b, তথ্য_BIT_আকার), false);

	/* bitmap_zero creates empty bitmap */
	bitmap_zero(res, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(res, তথ্য_BIT_আকার), true);

	/* bitmap_fill creates non-empty bitmap */
	bitmap_fill(res, তথ্য_BIT_আকার);
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(res, তথ্য_BIT_আকার), false);

	/* Single bit set = not empty */
	bitmap_zero(res, তথ্য_BIT_আকার);
	bitmap_set(res, 0, 1);
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(res, তথ্য_BIT_আকার), false);

	/* Zero nbits = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(তথ্য_a, 0), true);
}

স্থির কাঠামো sbiunit_test_case bitmap_test_cases[] = {
	SBIUNIT_TEST_CASE(bitmap_and_test),
	SBIUNIT_TEST_CASE(bitmap_or_test),
	SBIUNIT_TEST_CASE(bitmap_xor_test),
	SBIUNIT_TEST_CASE(bitmap_empty_test),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(bitmap_test_suite, bitmap_test_cases);
