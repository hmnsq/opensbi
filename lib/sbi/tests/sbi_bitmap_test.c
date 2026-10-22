/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#అన్తర్భావ <sbi/sbi_bitmap.h>
#అన్తర్భావ <sbi/sbi_unit_test.h>

#నిర్వచించు దత్తాంశం_కొలత పరిమాణం(దత్తాంశం_zero)
#నిర్వచించు దత్తాంశం_బిట్టు_కొలత (దత్తాంశం_కొలత * 8)

స్థిర గుర్తులేని దీర్ఘ దత్తాంశం_a[] = { 0xDEADBEEF, 0x00BAB10C, 0x1BADB002, 0xABADBABE };
స్థిర గుర్తులేని దీర్ఘ దత్తాంశం_b[] = { 0xC00010FF, 0x00BAB10C, 0xBAAAAAAD, 0xBADDCAFE };
స్థిర గుర్తులేని దీర్ఘ దత్తాంశం_zero[] = { 0, 0, 0, 0 };

స్థిర శూన్య bitmap_and_test(నిర్మాణం sbiunit_test_case *test)
{
	గుర్తులేని దీర్ఘ res[దత్తాంశం_కొలత];
	గుర్తులేని దీర్ఘ a_and_b[] = { దత్తాంశం_a[0] & దత్తాంశం_b[0], దత్తాంశం_a[1] & దత్తాంశం_b[1],
				    దత్తాంశం_a[2] & దత్తాంశం_b[2], దత్తాంశం_a[3] & దత్తాంశం_b[3] };

	__bitmap_and(res, దత్తాంశం_a, దత్తాంశం_b, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_and_b, దత్తాంశం_కొలత);

	/* a & a = a */
	__bitmap_and(res, దత్తాంశం_a, దత్తాంశం_a, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_ASSERT_MEMEQ(test, res, దత్తాంశం_a, దత్తాంశం_కొలత);

	/* a & 0 = 0 */
	__bitmap_and(res, దత్తాంశం_a, దత్తాంశం_zero, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_MEMEQ(test, res, దత్తాంశం_zero, దత్తాంశం_కొలత);

	/* 0 & 0 = 0 */
	__bitmap_and(res, దత్తాంశం_zero, దత్తాంశం_zero, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_MEMEQ(test, res, దత్తాంశం_zero, దత్తాంశం_కొలత);

	sbi_memcpy(res, దత్తాంశం_zero, దత్తాంశం_కొలత);
	/* Cover zero 'bits' argument */
	__bitmap_and(res, దత్తాంశం_a, దత్తాంశం_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, దత్తాంశం_zero, దత్తాంశం_కొలత);
}

స్థిర శూన్య bitmap_or_test(నిర్మాణం sbiunit_test_case *test)
{
	గుర్తులేని దీర్ఘ res[దత్తాంశం_కొలత];
	గుర్తులేని దీర్ఘ a_or_b[] = { దత్తాంశం_a[0] | దత్తాంశం_b[0], దత్తాంశం_a[1] | దత్తాంశం_b[1],
				   దత్తాంశం_a[2] | దత్తాంశం_b[2], దత్తాంశం_a[3] | దత్తాంశం_b[3] };

	__bitmap_or(res, దత్తాంశం_a, దత్తాంశం_b, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_or_b, దత్తాంశం_కొలత);

	/* a | a = a */
	__bitmap_or(res, దత్తాంశం_a, దత్తాంశం_a, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_MEMEQ(test, res, దత్తాంశం_a, దత్తాంశం_కొలత);

	/* a | 0 = a */
	__bitmap_or(res, దత్తాంశం_a, దత్తాంశం_zero, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_MEMEQ(test, res, దత్తాంశం_a, దత్తాంశం_కొలత);

	/* 0 | 0 = 0 */
	__bitmap_or(res, దత్తాంశం_zero, దత్తాంశం_zero, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_MEMEQ(test, res, దత్తాంశం_zero, దత్తాంశం_కొలత);

	sbi_memcpy(res, దత్తాంశం_zero, దత్తాంశం_కొలత);
	__bitmap_or(res, దత్తాంశం_a, దత్తాంశం_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, దత్తాంశం_zero, దత్తాంశం_కొలత);
}

స్థిర శూన్య bitmap_xor_test(నిర్మాణం sbiunit_test_case *test)
{
	గుర్తులేని దీర్ఘ res[దత్తాంశం_కొలత];
	గుర్తులేని దీర్ఘ a_xor_b[] = { దత్తాంశం_a[0] ^ దత్తాంశం_b[0], దత్తాంశం_a[1] ^ దత్తాంశం_b[1],
				    దత్తాంశం_a[2] ^ దత్తాంశం_b[2], దత్తాంశం_a[3] ^ దత్తాంశం_b[3] };

	__bitmap_xor(res, దత్తాంశం_a, దత్తాంశం_b, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_xor_b, దత్తాంశం_కొలత);

	/* a ^ 0 = a */
	__bitmap_xor(res, దత్తాంశం_a, దత్తాంశం_zero, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_MEMEQ(test, res, దత్తాంశం_a, దత్తాంశం_కొలత);

	/* a ^ a = 0 */
	__bitmap_xor(res, దత్తాంశం_a, దత్తాంశం_a, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_MEMEQ(test, res, దత్తాంశం_zero, దత్తాంశం_కొలత);

	/* 0 ^ 0 = 0 */
	__bitmap_xor(res, దత్తాంశం_zero, దత్తాంశం_zero, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_MEMEQ(test, res, దత్తాంశం_zero, దత్తాంశం_కొలత);

	sbi_memcpy(res, దత్తాంశం_zero, దత్తాంశం_కొలత);
	__bitmap_xor(res, దత్తాంశం_a, దత్తాంశం_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, దత్తాంశం_zero, దత్తాంశం_కొలత);
}

స్థిర శూన్య bitmap_ఖాళీలు_test(నిర్మాణం sbiunit_test_case *test)
{
	గుర్తులేని దీర్ఘ res[దత్తాంశం_కొలత];

	/* All zeros = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_ఖాళీలు(దత్తాంశం_zero, దత్తాంశం_బిట్టు_కొలత), true);

	/* Non-zero data = not empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_ఖాళీలు(దత్తాంశం_a, దత్తాంశం_బిట్టు_కొలత), false);
	SBIUNIT_EXPECT_EQ(test, bitmap_ఖాళీలు(దత్తాంశం_b, దత్తాంశం_బిట్టు_కొలత), false);

	/* bitmap_zero creates empty bitmap */
	bitmap_zero(res, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_EQ(test, bitmap_ఖాళీలు(res, దత్తాంశం_బిట్టు_కొలత), true);

	/* bitmap_fill creates non-empty bitmap */
	bitmap_fill(res, దత్తాంశం_బిట్టు_కొలత);
	SBIUNIT_EXPECT_EQ(test, bitmap_ఖాళీలు(res, దత్తాంశం_బిట్టు_కొలత), false);

	/* Single bit set = not empty */
	bitmap_zero(res, దత్తాంశం_బిట్టు_కొలత);
	bitmap_సమితి(res, 0, 1);
	SBIUNIT_EXPECT_EQ(test, bitmap_ఖాళీలు(res, దత్తాంశం_బిట్టు_కొలత), false);

	/* Zero nbits = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_ఖాళీలు(దత్తాంశం_a, 0), true);
}

స్థిర నిర్మాణం sbiunit_test_case bitmap_test_cases[] = {
	SBIUNIT_TEST_CASE(bitmap_and_test),
	SBIUNIT_TEST_CASE(bitmap_or_test),
	SBIUNIT_TEST_CASE(bitmap_xor_test),
	SBIUNIT_TEST_CASE(bitmap_ఖాళీలు_test),
	SBIUNIT_చివర_CASE,
};

SBIUNIT_TEST_SUITE(bitmap_test_suite, bitmap_test_cases);
