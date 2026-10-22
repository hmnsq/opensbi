/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#अन्तर्भाव <sbi/sbi_bitmap.h>
#अन्तर्भाव <sbi/sbi_unit_test.h>

#परिभाषय दत्त_आकार परिमाण(दत्त_zero)
#परिभाषय दत्त_BIT_आकार (दत्त_आकार * 8)

स्थिर अचिह्नित दीर्घ दत्त_a[] = { 0xDEADBEEF, 0x00BAB10C, 0x1BADB002, 0xABADBABE };
स्थिर अचिह्नित दीर्घ दत्त_b[] = { 0xC00010FF, 0x00BAB10C, 0xBAAAAAAD, 0xBADDCAFE };
स्थिर अचिह्नित दीर्घ दत्त_zero[] = { 0, 0, 0, 0 };

स्थिर शून्य bitmap_and_test(संरचना sbiunit_test_case *test)
{
	अचिह्नित दीर्घ res[दत्त_आकार];
	अचिह्नित दीर्घ a_and_b[] = { दत्त_a[0] & दत्त_b[0], दत्त_a[1] & दत्त_b[1],
				    दत्त_a[2] & दत्त_b[2], दत्त_a[3] & दत्त_b[3] };

	__bitmap_and(res, दत्त_a, दत्त_b, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_and_b, दत्त_आकार);

	/* a & a = a */
	__bitmap_and(res, दत्त_a, दत्त_a, दत्त_BIT_आकार);
	SBIUNIT_ASSERT_MEMEQ(test, res, दत्त_a, दत्त_आकार);

	/* a & 0 = 0 */
	__bitmap_and(res, दत्त_a, दत्त_zero, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, दत्त_zero, दत्त_आकार);

	/* 0 & 0 = 0 */
	__bitmap_and(res, दत्त_zero, दत्त_zero, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, दत्त_zero, दत्त_आकार);

	sbi_memcpy(res, दत्त_zero, दत्त_आकार);
	/* Cover zero 'bits' argument */
	__bitmap_and(res, दत्त_a, दत्त_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, दत्त_zero, दत्त_आकार);
}

स्थिर शून्य bitmap_or_test(संरचना sbiunit_test_case *test)
{
	अचिह्नित दीर्घ res[दत्त_आकार];
	अचिह्नित दीर्घ a_or_b[] = { दत्त_a[0] | दत्त_b[0], दत्त_a[1] | दत्त_b[1],
				   दत्त_a[2] | दत्त_b[2], दत्त_a[3] | दत्त_b[3] };

	__bitmap_or(res, दत्त_a, दत्त_b, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_or_b, दत्त_आकार);

	/* a | a = a */
	__bitmap_or(res, दत्त_a, दत्त_a, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, दत्त_a, दत्त_आकार);

	/* a | 0 = a */
	__bitmap_or(res, दत्त_a, दत्त_zero, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, दत्त_a, दत्त_आकार);

	/* 0 | 0 = 0 */
	__bitmap_or(res, दत्त_zero, दत्त_zero, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, दत्त_zero, दत्त_आकार);

	sbi_memcpy(res, दत्त_zero, दत्त_आकार);
	__bitmap_or(res, दत्त_a, दत्त_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, दत्त_zero, दत्त_आकार);
}

स्थिर शून्य bitmap_xor_test(संरचना sbiunit_test_case *test)
{
	अचिह्नित दीर्घ res[दत्त_आकार];
	अचिह्नित दीर्घ a_xor_b[] = { दत्त_a[0] ^ दत्त_b[0], दत्त_a[1] ^ दत्त_b[1],
				    दत्त_a[2] ^ दत्त_b[2], दत्त_a[3] ^ दत्त_b[3] };

	__bitmap_xor(res, दत्त_a, दत्त_b, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_xor_b, दत्त_आकार);

	/* a ^ 0 = a */
	__bitmap_xor(res, दत्त_a, दत्त_zero, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, दत्त_a, दत्त_आकार);

	/* a ^ a = 0 */
	__bitmap_xor(res, दत्त_a, दत्त_a, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, दत्त_zero, दत्त_आकार);

	/* 0 ^ 0 = 0 */
	__bitmap_xor(res, दत्त_zero, दत्त_zero, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_MEMEQ(test, res, दत्त_zero, दत्त_आकार);

	sbi_memcpy(res, दत्त_zero, दत्त_आकार);
	__bitmap_xor(res, दत्त_a, दत्त_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, दत्त_zero, दत्त_आकार);
}

स्थिर शून्य bitmap_रिक्तानि_test(संरचना sbiunit_test_case *test)
{
	अचिह्नित दीर्घ res[दत्त_आकार];

	/* All zeros = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_रिक्तानि(दत्त_zero, दत्त_BIT_आकार), true);

	/* Non-zero data = not empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_रिक्तानि(दत्त_a, दत्त_BIT_आकार), false);
	SBIUNIT_EXPECT_EQ(test, bitmap_रिक्तानि(दत्त_b, दत्त_BIT_आकार), false);

	/* bitmap_zero creates empty bitmap */
	bitmap_zero(res, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_EQ(test, bitmap_रिक्तानि(res, दत्त_BIT_आकार), true);

	/* bitmap_fill creates non-empty bitmap */
	bitmap_fill(res, दत्त_BIT_आकार);
	SBIUNIT_EXPECT_EQ(test, bitmap_रिक्तानि(res, दत्त_BIT_आकार), false);

	/* Single bit set = not empty */
	bitmap_zero(res, दत्त_BIT_आकार);
	bitmap_समुच्चयः(res, 0, 1);
	SBIUNIT_EXPECT_EQ(test, bitmap_रिक्तानि(res, दत्त_BIT_आकार), false);

	/* Zero nbits = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_रिक्तानि(दत्त_a, 0), true);
}

स्थिर संरचना sbiunit_test_case bitmap_test_cases[] = {
	SBIUNIT_TEST_CASE(bitmap_and_test),
	SBIUNIT_TEST_CASE(bitmap_or_test),
	SBIUNIT_TEST_CASE(bitmap_xor_test),
	SBIUNIT_TEST_CASE(bitmap_रिक्तानि_test),
	SBIUNIT_अन्तः_CASE,
};

SBIUNIT_TEST_SUITE(bitmap_test_suite, bitmap_test_cases);
