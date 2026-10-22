/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#समावेश <sbi/sbi_bitmap.h>
#समावेश <sbi/sbi_unit_test.h>

#घोषणा तथ्य_आकार माप(तथ्य_zero)
#घोषणा तथ्य_BIT_आकार (तथ्य_आकार * 8)

अटल अचिन्हित दीर्घ तथ्य_a[] = { 0xDEADBEEF, 0x00BAB10C, 0x1BADB002, 0xABADBABE };
अटल अचिन्हित दीर्घ तथ्य_b[] = { 0xC00010FF, 0x00BAB10C, 0xBAAAAAAD, 0xBADDCAFE };
अटल अचिन्हित दीर्घ तथ्य_zero[] = { 0, 0, 0, 0 };

अटल व्योम bitmap_and_test(काष्ठा sbiunit_test_case *test)
{
	अचिन्हित दीर्घ res[तथ्य_आकार];
	अचिन्हित दीर्घ a_and_b[] = { तथ्य_a[0] & तथ्य_b[0], तथ्य_a[1] & तथ्य_b[1],
				    तथ्य_a[2] & तथ्य_b[2], तथ्य_a[3] & तथ्य_b[3] };

	__bitmap_and(res, तथ्य_a, तथ्य_b, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_and_b, तथ्य_आकार);

	/* a & a = a */
	__bitmap_and(res, तथ्य_a, तथ्य_a, तथ्य_BIT_आकार);
	SBIUNIT_ASSERT_MEMEQ(test, res, तथ्य_a, तथ्य_आकार);

	/* a & 0 = 0 */
	__bitmap_and(res, तथ्य_a, तथ्य_zero, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, तथ्य_zero, तथ्य_आकार);

	/* 0 & 0 = 0 */
	__bitmap_and(res, तथ्य_zero, तथ्य_zero, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, तथ्य_zero, तथ्य_आकार);

	sbi_memcpy(res, तथ्य_zero, तथ्य_आकार);
	/* Cover zero 'bits' argument */
	__bitmap_and(res, तथ्य_a, तथ्य_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, तथ्य_zero, तथ्य_आकार);
}

अटल व्योम bitmap_or_test(काष्ठा sbiunit_test_case *test)
{
	अचिन्हित दीर्घ res[तथ्य_आकार];
	अचिन्हित दीर्घ a_or_b[] = { तथ्य_a[0] | तथ्य_b[0], तथ्य_a[1] | तथ्य_b[1],
				   तथ्य_a[2] | तथ्य_b[2], तथ्य_a[3] | तथ्य_b[3] };

	__bitmap_or(res, तथ्य_a, तथ्य_b, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_or_b, तथ्य_आकार);

	/* a | a = a */
	__bitmap_or(res, तथ्य_a, तथ्य_a, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, तथ्य_a, तथ्य_आकार);

	/* a | 0 = a */
	__bitmap_or(res, तथ्य_a, तथ्य_zero, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, तथ्य_a, तथ्य_आकार);

	/* 0 | 0 = 0 */
	__bitmap_or(res, तथ्य_zero, तथ्य_zero, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, तथ्य_zero, तथ्य_आकार);

	sbi_memcpy(res, तथ्य_zero, तथ्य_आकार);
	__bitmap_or(res, तथ्य_a, तथ्य_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, तथ्य_zero, तथ्य_आकार);
}

अटल व्योम bitmap_xor_test(काष्ठा sbiunit_test_case *test)
{
	अचिन्हित दीर्घ res[तथ्य_आकार];
	अचिन्हित दीर्घ a_xor_b[] = { तथ्य_a[0] ^ तथ्य_b[0], तथ्य_a[1] ^ तथ्य_b[1],
				    तथ्य_a[2] ^ तथ्य_b[2], तथ्य_a[3] ^ तथ्य_b[3] };

	__bitmap_xor(res, तथ्य_a, तथ्य_b, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_xor_b, तथ्य_आकार);

	/* a ^ 0 = a */
	__bitmap_xor(res, तथ्य_a, तथ्य_zero, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, तथ्य_a, तथ्य_आकार);

	/* a ^ a = 0 */
	__bitmap_xor(res, तथ्य_a, तथ्य_a, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, तथ्य_zero, तथ्य_आकार);

	/* 0 ^ 0 = 0 */
	__bitmap_xor(res, तथ्य_zero, तथ्य_zero, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, तथ्य_zero, तथ्य_आकार);

	sbi_memcpy(res, तथ्य_zero, तथ्य_आकार);
	__bitmap_xor(res, तथ्य_a, तथ्य_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, तथ्य_zero, तथ्य_आकार);
}

अटल व्योम bitmap_empty_test(काष्ठा sbiunit_test_case *test)
{
	अचिन्हित दीर्घ res[तथ्य_आकार];

	/* All zeros = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(तथ्य_zero, तथ्य_BIT_आकार), true);

	/* Non-zero data = not empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(तथ्य_a, तथ्य_BIT_आकार), false);
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(तथ्य_b, तथ्य_BIT_आकार), false);

	/* bitmap_zero creates empty bitmap */
	bitmap_zero(res, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(res, तथ्य_BIT_आकार), true);

	/* bitmap_fill creates non-empty bitmap */
	bitmap_fill(res, तथ्य_BIT_आकार);
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(res, तथ्य_BIT_आकार), false);

	/* Single bit set = not empty */
	bitmap_zero(res, तथ्य_BIT_आकार);
	bitmap_set(res, 0, 1);
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(res, तथ्य_BIT_आकार), false);

	/* Zero nbits = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(तथ्य_a, 0), true);
}

अटल काष्ठा sbiunit_test_case bitmap_test_cases[] = {
	SBIUNIT_TEST_CASE(bitmap_and_test),
	SBIUNIT_TEST_CASE(bitmap_or_test),
	SBIUNIT_TEST_CASE(bitmap_xor_test),
	SBIUNIT_TEST_CASE(bitmap_empty_test),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(bitmap_test_suite, bitmap_test_cases);
