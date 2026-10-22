/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#شامل <sbi/sbi_bitmap.h>
#شامل <sbi/sbi_unit_test.h>

#تعریف_کرو ڈیٹا_سائز حجم(ڈیٹا_zero)
#تعریف_کرو ڈیٹا_ثنائی_ہندسہ_سائز (ڈیٹا_سائز * 8)

ساکن غیرعلامتی طویل ڈیٹا_a[] = { 0xDEADBEEF, 0x00BAB10C, 0x1BADB002, 0xABADBABE };
ساکن غیرعلامتی طویل ڈیٹا_b[] = { 0xC00010FF, 0x00BAB10C, 0xBAAAAAAD, 0xBADDCAFE };
ساکن غیرعلامتی طویل ڈیٹا_zero[] = { 0, 0, 0, 0 };

ساکن خالی bitmap_and_test(ساخت sbiunit_test_case *test)
{
	غیرعلامتی طویل res[ڈیٹا_سائز];
	غیرعلامتی طویل a_and_b[] = { ڈیٹا_a[0] & ڈیٹا_b[0], ڈیٹا_a[1] & ڈیٹا_b[1],
				    ڈیٹا_a[2] & ڈیٹا_b[2], ڈیٹا_a[3] & ڈیٹا_b[3] };

	__bitmap_and(res, ڈیٹا_a, ڈیٹا_b, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_and_b, ڈیٹا_سائز);

	/* a & a = a */
	__bitmap_and(res, ڈیٹا_a, ڈیٹا_a, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_ASSERT_MEMEQ(test, res, ڈیٹا_a, ڈیٹا_سائز);

	/* a & 0 = 0 */
	__bitmap_and(res, ڈیٹا_a, ڈیٹا_zero, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_MEMEQ(test, res, ڈیٹا_zero, ڈیٹا_سائز);

	/* 0 & 0 = 0 */
	__bitmap_and(res, ڈیٹا_zero, ڈیٹا_zero, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_MEMEQ(test, res, ڈیٹا_zero, ڈیٹا_سائز);

	sbi_memcpy(res, ڈیٹا_zero, ڈیٹا_سائز);
	/* Cover zero 'bits' argument */
	__bitmap_and(res, ڈیٹا_a, ڈیٹا_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, ڈیٹا_zero, ڈیٹا_سائز);
}

ساکن خالی bitmap_or_test(ساخت sbiunit_test_case *test)
{
	غیرعلامتی طویل res[ڈیٹا_سائز];
	غیرعلامتی طویل a_or_b[] = { ڈیٹا_a[0] | ڈیٹا_b[0], ڈیٹا_a[1] | ڈیٹا_b[1],
				   ڈیٹا_a[2] | ڈیٹا_b[2], ڈیٹا_a[3] | ڈیٹا_b[3] };

	__bitmap_or(res, ڈیٹا_a, ڈیٹا_b, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_or_b, ڈیٹا_سائز);

	/* a | a = a */
	__bitmap_or(res, ڈیٹا_a, ڈیٹا_a, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_MEMEQ(test, res, ڈیٹا_a, ڈیٹا_سائز);

	/* a | 0 = a */
	__bitmap_or(res, ڈیٹا_a, ڈیٹا_zero, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_MEMEQ(test, res, ڈیٹا_a, ڈیٹا_سائز);

	/* 0 | 0 = 0 */
	__bitmap_or(res, ڈیٹا_zero, ڈیٹا_zero, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_MEMEQ(test, res, ڈیٹا_zero, ڈیٹا_سائز);

	sbi_memcpy(res, ڈیٹا_zero, ڈیٹا_سائز);
	__bitmap_or(res, ڈیٹا_a, ڈیٹا_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, ڈیٹا_zero, ڈیٹا_سائز);
}

ساکن خالی bitmap_xor_test(ساخت sbiunit_test_case *test)
{
	غیرعلامتی طویل res[ڈیٹا_سائز];
	غیرعلامتی طویل a_xor_b[] = { ڈیٹا_a[0] ^ ڈیٹا_b[0], ڈیٹا_a[1] ^ ڈیٹا_b[1],
				    ڈیٹا_a[2] ^ ڈیٹا_b[2], ڈیٹا_a[3] ^ ڈیٹا_b[3] };

	__bitmap_xor(res, ڈیٹا_a, ڈیٹا_b, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_xor_b, ڈیٹا_سائز);

	/* a ^ 0 = a */
	__bitmap_xor(res, ڈیٹا_a, ڈیٹا_zero, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_MEMEQ(test, res, ڈیٹا_a, ڈیٹا_سائز);

	/* a ^ a = 0 */
	__bitmap_xor(res, ڈیٹا_a, ڈیٹا_a, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_MEMEQ(test, res, ڈیٹا_zero, ڈیٹا_سائز);

	/* 0 ^ 0 = 0 */
	__bitmap_xor(res, ڈیٹا_zero, ڈیٹا_zero, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_MEMEQ(test, res, ڈیٹا_zero, ڈیٹا_سائز);

	sbi_memcpy(res, ڈیٹا_zero, ڈیٹا_سائز);
	__bitmap_xor(res, ڈیٹا_a, ڈیٹا_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, ڈیٹا_zero, ڈیٹا_سائز);
}

ساکن خالی bitmap_خالی_خانے_test(ساخت sbiunit_test_case *test)
{
	غیرعلامتی طویل res[ڈیٹا_سائز];

	/* All zeros = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_خانے(ڈیٹا_zero, ڈیٹا_ثنائی_ہندسہ_سائز), true);

	/* Non-zero data = not empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_خانے(ڈیٹا_a, ڈیٹا_ثنائی_ہندسہ_سائز), false);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_خانے(ڈیٹا_b, ڈیٹا_ثنائی_ہندسہ_سائز), false);

	/* bitmap_zero creates empty bitmap */
	bitmap_zero(res, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_خانے(res, ڈیٹا_ثنائی_ہندسہ_سائز), true);

	/* bitmap_fill creates non-empty bitmap */
	bitmap_fill(res, ڈیٹا_ثنائی_ہندسہ_سائز);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_خانے(res, ڈیٹا_ثنائی_ہندسہ_سائز), false);

	/* Single bit set = not empty */
	bitmap_zero(res, ڈیٹا_ثنائی_ہندسہ_سائز);
	bitmap_سیٹ(res, 0, 1);
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_خانے(res, ڈیٹا_ثنائی_ہندسہ_سائز), false);

	/* Zero nbits = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_خالی_خانے(ڈیٹا_a, 0), true);
}

ساکن ساخت sbiunit_test_case bitmap_test_cases[] = {
	SBIUNIT_TEST_CASE(bitmap_and_test),
	SBIUNIT_TEST_CASE(bitmap_or_test),
	SBIUNIT_TEST_CASE(bitmap_xor_test),
	SBIUNIT_TEST_CASE(bitmap_خالی_خانے_test),
	SBIUNIT_انتہا_CASE,
};

SBIUNIT_TEST_SUITE(bitmap_test_suite, bitmap_test_cases);
