/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#ضمن <sbi/sbi_bitmap.h>
#ضمن <sbi/sbi_unit_test.h>

#تعریف_کن داده_اندازه حجم(داده_zero)
#تعریف_کن داده_بیت_اندازه (داده_اندازه * 8)

ساكن بیعلامت دراز داده_a[] = { 0xDEADBEEF, 0x00BAB10C, 0x1BADB002, 0xABADBABE };
ساكن بیعلامت دراز داده_b[] = { 0xC00010FF, 0x00BAB10C, 0xBAAAAAAD, 0xBADDCAFE };
ساكن بیعلامت دراز داده_zero[] = { 0, 0, 0, 0 };

ساكن فراغ bitmap_and_test(بنية sbiunit_test_case *test)
{
	بیعلامت دراز res[داده_اندازه];
	بیعلامت دراز a_and_b[] = { داده_a[0] & داده_b[0], داده_a[1] & داده_b[1],
				    داده_a[2] & داده_b[2], داده_a[3] & داده_b[3] };

	__bitmap_and(res, داده_a, داده_b, داده_بیت_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_and_b, داده_اندازه);

	/* a & a = a */
	__bitmap_and(res, داده_a, داده_a, داده_بیت_اندازه);
	SBIUNIT_ASSERT_MEMEQ(test, res, داده_a, داده_اندازه);

	/* a & 0 = 0 */
	__bitmap_and(res, داده_a, داده_zero, داده_بیت_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, داده_zero, داده_اندازه);

	/* 0 & 0 = 0 */
	__bitmap_and(res, داده_zero, داده_zero, داده_بیت_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, داده_zero, داده_اندازه);

	sbi_memcpy(res, داده_zero, داده_اندازه);
	/* Cover zero 'bits' argument */
	__bitmap_and(res, داده_a, داده_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, داده_zero, داده_اندازه);
}

ساكن فراغ bitmap_or_test(بنية sbiunit_test_case *test)
{
	بیعلامت دراز res[داده_اندازه];
	بیعلامت دراز a_or_b[] = { داده_a[0] | داده_b[0], داده_a[1] | داده_b[1],
				   داده_a[2] | داده_b[2], داده_a[3] | داده_b[3] };

	__bitmap_or(res, داده_a, داده_b, داده_بیت_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_or_b, داده_اندازه);

	/* a | a = a */
	__bitmap_or(res, داده_a, داده_a, داده_بیت_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, داده_a, داده_اندازه);

	/* a | 0 = a */
	__bitmap_or(res, داده_a, داده_zero, داده_بیت_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, داده_a, داده_اندازه);

	/* 0 | 0 = 0 */
	__bitmap_or(res, داده_zero, داده_zero, داده_بیت_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, داده_zero, داده_اندازه);

	sbi_memcpy(res, داده_zero, داده_اندازه);
	__bitmap_or(res, داده_a, داده_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, داده_zero, داده_اندازه);
}

ساكن فراغ bitmap_xor_test(بنية sbiunit_test_case *test)
{
	بیعلامت دراز res[داده_اندازه];
	بیعلامت دراز a_xor_b[] = { داده_a[0] ^ داده_b[0], داده_a[1] ^ داده_b[1],
				    داده_a[2] ^ داده_b[2], داده_a[3] ^ داده_b[3] };

	__bitmap_xor(res, داده_a, داده_b, داده_بیت_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_xor_b, داده_اندازه);

	/* a ^ 0 = a */
	__bitmap_xor(res, داده_a, داده_zero, داده_بیت_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, داده_a, داده_اندازه);

	/* a ^ a = 0 */
	__bitmap_xor(res, داده_a, داده_a, داده_بیت_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, داده_zero, داده_اندازه);

	/* 0 ^ 0 = 0 */
	__bitmap_xor(res, داده_zero, داده_zero, داده_بیت_اندازه);
	SBIUNIT_EXPECT_MEMEQ(test, res, داده_zero, داده_اندازه);

	sbi_memcpy(res, داده_zero, داده_اندازه);
	__bitmap_xor(res, داده_a, داده_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, داده_zero, داده_اندازه);
}

ساكن فراغ bitmap_خالی_test(بنية sbiunit_test_case *test)
{
	بیعلامت دراز res[داده_اندازه];

	/* All zeros = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی(داده_zero, داده_بیت_اندازه), true);

	/* Non-zero data = not empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی(داده_a, داده_بیت_اندازه), false);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی(داده_b, داده_بیت_اندازه), false);

	/* bitmap_zero creates empty bitmap */
	bitmap_zero(res, داده_بیت_اندازه);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی(res, داده_بیت_اندازه), true);

	/* bitmap_fill creates non-empty bitmap */
	bitmap_fill(res, داده_بیت_اندازه);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی(res, داده_بیت_اندازه), false);

	/* Single bit set = not empty */
	bitmap_zero(res, داده_بیت_اندازه);
	bitmap_مجموعه(res, 0, 1);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی(res, داده_بیت_اندازه), false);

	/* Zero nbits = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی(داده_a, 0), true);
}

ساكن بنية sbiunit_test_case bitmap_test_cases[] = {
	SBIUNIT_TEST_CASE(bitmap_and_test),
	SBIUNIT_TEST_CASE(bitmap_or_test),
	SBIUNIT_TEST_CASE(bitmap_xor_test),
	SBIUNIT_TEST_CASE(bitmap_خالی_test),
	SBIUNIT_انتها_CASE,
};

SBIUNIT_TEST_SUITE(bitmap_test_suite, bitmap_test_cases);
