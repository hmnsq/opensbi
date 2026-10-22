/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#ضمن <sbi/sbi_bitmap.h>
#ضمن <sbi/sbi_unit_test.h>

#وصف_ڪريو معلومات_سائيز حجم(معلومات_zero)
#وصف_ڪريو معلومات_ٻٽو_انگ_سائيز (معلومات_سائيز * 8)

ساكن بنانشان ڊگهو معلومات_a[] = { 0xDEADBEEF, 0x00BAB10C, 0x1BADB002, 0xABADBABE };
ساكن بنانشان ڊگهو معلومات_b[] = { 0xC00010FF, 0x00BAB10C, 0xBAAAAAAD, 0xBADDCAFE };
ساكن بنانشان ڊگهو معلومات_zero[] = { 0, 0, 0, 0 };

ساكن فراغ bitmap_and_test(بنية sbiunit_test_case *test)
{
	بنانشان ڊگهو res[معلومات_سائيز];
	بنانشان ڊگهو a_and_b[] = { معلومات_a[0] & معلومات_b[0], معلومات_a[1] & معلومات_b[1],
				    معلومات_a[2] & معلومات_b[2], معلومات_a[3] & معلومات_b[3] };

	__bitmap_and(res, معلومات_a, معلومات_b, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_and_b, معلومات_سائيز);

	/* a & a = a */
	__bitmap_and(res, معلومات_a, معلومات_a, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_ASSERT_MEMEQ(test, res, معلومات_a, معلومات_سائيز);

	/* a & 0 = 0 */
	__bitmap_and(res, معلومات_a, معلومات_zero, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_سائيز);

	/* 0 & 0 = 0 */
	__bitmap_and(res, معلومات_zero, معلومات_zero, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_سائيز);

	sbi_memcpy(res, معلومات_zero, معلومات_سائيز);
	/* Cover zero 'bits' argument */
	__bitmap_and(res, معلومات_a, معلومات_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_سائيز);
}

ساكن فراغ bitmap_or_test(بنية sbiunit_test_case *test)
{
	بنانشان ڊگهو res[معلومات_سائيز];
	بنانشان ڊگهو a_or_b[] = { معلومات_a[0] | معلومات_b[0], معلومات_a[1] | معلومات_b[1],
				   معلومات_a[2] | معلومات_b[2], معلومات_a[3] | معلومات_b[3] };

	__bitmap_or(res, معلومات_a, معلومات_b, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_or_b, معلومات_سائيز);

	/* a | a = a */
	__bitmap_or(res, معلومات_a, معلومات_a, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_a, معلومات_سائيز);

	/* a | 0 = a */
	__bitmap_or(res, معلومات_a, معلومات_zero, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_a, معلومات_سائيز);

	/* 0 | 0 = 0 */
	__bitmap_or(res, معلومات_zero, معلومات_zero, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_سائيز);

	sbi_memcpy(res, معلومات_zero, معلومات_سائيز);
	__bitmap_or(res, معلومات_a, معلومات_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_سائيز);
}

ساكن فراغ bitmap_xor_test(بنية sbiunit_test_case *test)
{
	بنانشان ڊگهو res[معلومات_سائيز];
	بنانشان ڊگهو a_xor_b[] = { معلومات_a[0] ^ معلومات_b[0], معلومات_a[1] ^ معلومات_b[1],
				    معلومات_a[2] ^ معلومات_b[2], معلومات_a[3] ^ معلومات_b[3] };

	__bitmap_xor(res, معلومات_a, معلومات_b, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_xor_b, معلومات_سائيز);

	/* a ^ 0 = a */
	__bitmap_xor(res, معلومات_a, معلومات_zero, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_a, معلومات_سائيز);

	/* a ^ a = 0 */
	__bitmap_xor(res, معلومات_a, معلومات_a, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_سائيز);

	/* 0 ^ 0 = 0 */
	__bitmap_xor(res, معلومات_zero, معلومات_zero, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_سائيز);

	sbi_memcpy(res, معلومات_zero, معلومات_سائيز);
	__bitmap_xor(res, معلومات_a, معلومات_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, معلومات_zero, معلومات_سائيز);
}

ساكن فراغ bitmap_خالي_جايون_test(بنية sbiunit_test_case *test)
{
	بنانشان ڊگهو res[معلومات_سائيز];

	/* All zeros = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_خالي_جايون(معلومات_zero, معلومات_ٻٽو_انگ_سائيز), true);

	/* Non-zero data = not empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_خالي_جايون(معلومات_a, معلومات_ٻٽو_انگ_سائيز), false);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالي_جايون(معلومات_b, معلومات_ٻٽو_انگ_سائيز), false);

	/* bitmap_zero creates empty bitmap */
	bitmap_zero(res, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالي_جايون(res, معلومات_ٻٽو_انگ_سائيز), true);

	/* bitmap_fill creates non-empty bitmap */
	bitmap_fill(res, معلومات_ٻٽو_انگ_سائيز);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالي_جايون(res, معلومات_ٻٽو_انگ_سائيز), false);

	/* Single bit set = not empty */
	bitmap_zero(res, معلومات_ٻٽو_انگ_سائيز);
	bitmap_سيٽ(res, 0, 1);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالي_جايون(res, معلومات_ٻٽو_انگ_سائيز), false);

	/* Zero nbits = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_خالي_جايون(معلومات_a, 0), true);
}

ساكن بنية sbiunit_test_case bitmap_test_cases[] = {
	SBIUNIT_TEST_CASE(bitmap_and_test),
	SBIUNIT_TEST_CASE(bitmap_or_test),
	SBIUNIT_TEST_CASE(bitmap_xor_test),
	SBIUNIT_TEST_CASE(bitmap_خالي_جايون_test),
	SBIUNIT_پڇاڙي_CASE,
};

SBIUNIT_TEST_SUITE(bitmap_test_suite, bitmap_test_cases);
