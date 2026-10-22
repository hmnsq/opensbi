/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#ضمن <sbi/sbi_bitmap.h>
#ضمن <sbi/sbi_unit_test.h>

#تعریف_کرو انکڑے_ناپ حجم(انکڑے_zero)
#تعریف_کرو انکڑے_ثنائی_ہندسہ_ناپ (انکڑے_ناپ * 8)

ساكن بنانشانی لما انکڑے_a[] = { 0xDEADBEEF, 0x00BAB10C, 0x1BADB002, 0xABADBABE };
ساكن بنانشانی لما انکڑے_b[] = { 0xC00010FF, 0x00BAB10C, 0xBAAAAAAD, 0xBADDCAFE };
ساكن بنانشانی لما انکڑے_zero[] = { 0, 0, 0, 0 };

ساكن فراغ bitmap_and_test(بنية sbiunit_test_case *test)
{
	بنانشانی لما res[انکڑے_ناپ];
	بنانشانی لما a_and_b[] = { انکڑے_a[0] & انکڑے_b[0], انکڑے_a[1] & انکڑے_b[1],
				    انکڑے_a[2] & انکڑے_b[2], انکڑے_a[3] & انکڑے_b[3] };

	__bitmap_and(res, انکڑے_a, انکڑے_b, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_and_b, انکڑے_ناپ);

	/* a & a = a */
	__bitmap_and(res, انکڑے_a, انکڑے_a, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_ASSERT_MEMEQ(test, res, انکڑے_a, انکڑے_ناپ);

	/* a & 0 = 0 */
	__bitmap_and(res, انکڑے_a, انکڑے_zero, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_MEMEQ(test, res, انکڑے_zero, انکڑے_ناپ);

	/* 0 & 0 = 0 */
	__bitmap_and(res, انکڑے_zero, انکڑے_zero, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_MEMEQ(test, res, انکڑے_zero, انکڑے_ناپ);

	sbi_memcpy(res, انکڑے_zero, انکڑے_ناپ);
	/* Cover zero 'bits' argument */
	__bitmap_and(res, انکڑے_a, انکڑے_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, انکڑے_zero, انکڑے_ناپ);
}

ساكن فراغ bitmap_or_test(بنية sbiunit_test_case *test)
{
	بنانشانی لما res[انکڑے_ناپ];
	بنانشانی لما a_or_b[] = { انکڑے_a[0] | انکڑے_b[0], انکڑے_a[1] | انکڑے_b[1],
				   انکڑے_a[2] | انکڑے_b[2], انکڑے_a[3] | انکڑے_b[3] };

	__bitmap_or(res, انکڑے_a, انکڑے_b, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_or_b, انکڑے_ناپ);

	/* a | a = a */
	__bitmap_or(res, انکڑے_a, انکڑے_a, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_MEMEQ(test, res, انکڑے_a, انکڑے_ناپ);

	/* a | 0 = a */
	__bitmap_or(res, انکڑے_a, انکڑے_zero, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_MEMEQ(test, res, انکڑے_a, انکڑے_ناپ);

	/* 0 | 0 = 0 */
	__bitmap_or(res, انکڑے_zero, انکڑے_zero, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_MEMEQ(test, res, انکڑے_zero, انکڑے_ناپ);

	sbi_memcpy(res, انکڑے_zero, انکڑے_ناپ);
	__bitmap_or(res, انکڑے_a, انکڑے_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, انکڑے_zero, انکڑے_ناپ);
}

ساكن فراغ bitmap_xor_test(بنية sbiunit_test_case *test)
{
	بنانشانی لما res[انکڑے_ناپ];
	بنانشانی لما a_xor_b[] = { انکڑے_a[0] ^ انکڑے_b[0], انکڑے_a[1] ^ انکڑے_b[1],
				    انکڑے_a[2] ^ انکڑے_b[2], انکڑے_a[3] ^ انکڑے_b[3] };

	__bitmap_xor(res, انکڑے_a, انکڑے_b, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_xor_b, انکڑے_ناپ);

	/* a ^ 0 = a */
	__bitmap_xor(res, انکڑے_a, انکڑے_zero, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_MEMEQ(test, res, انکڑے_a, انکڑے_ناپ);

	/* a ^ a = 0 */
	__bitmap_xor(res, انکڑے_a, انکڑے_a, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_MEMEQ(test, res, انکڑے_zero, انکڑے_ناپ);

	/* 0 ^ 0 = 0 */
	__bitmap_xor(res, انکڑے_zero, انکڑے_zero, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_MEMEQ(test, res, انکڑے_zero, انکڑے_ناپ);

	sbi_memcpy(res, انکڑے_zero, انکڑے_ناپ);
	__bitmap_xor(res, انکڑے_a, انکڑے_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, انکڑے_zero, انکڑے_ناپ);
}

ساكن فراغ bitmap_خالی_تھاواں_test(بنية sbiunit_test_case *test)
{
	بنانشانی لما res[انکڑے_ناپ];

	/* All zeros = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_تھاواں(انکڑے_zero, انکڑے_ثنائی_ہندسہ_ناپ), true);

	/* Non-zero data = not empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_تھاواں(انکڑے_a, انکڑے_ثنائی_ہندسہ_ناپ), false);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_تھاواں(انکڑے_b, انکڑے_ثنائی_ہندسہ_ناپ), false);

	/* bitmap_zero creates empty bitmap */
	bitmap_zero(res, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_تھاواں(res, انکڑے_ثنائی_ہندسہ_ناپ), true);

	/* bitmap_fill creates non-empty bitmap */
	bitmap_fill(res, انکڑے_ثنائی_ہندسہ_ناپ);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_تھاواں(res, انکڑے_ثنائی_ہندسہ_ناپ), false);

	/* Single bit set = not empty */
	bitmap_zero(res, انکڑے_ثنائی_ہندسہ_ناپ);
	bitmap_سیٹ(res, 0, 1);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_تھاواں(res, انکڑے_ثنائی_ہندسہ_ناپ), false);

	/* Zero nbits = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_تھاواں(انکڑے_a, 0), true);
}

ساكن بنية sbiunit_test_case bitmap_test_cases[] = {
	SBIUNIT_TEST_CASE(bitmap_and_test),
	SBIUNIT_TEST_CASE(bitmap_or_test),
	SBIUNIT_TEST_CASE(bitmap_xor_test),
	SBIUNIT_TEST_CASE(bitmap_خالی_تھاواں_test),
	SBIUNIT_سرا_CASE,
};

SBIUNIT_TEST_SUITE(bitmap_test_suite, bitmap_test_cases);
