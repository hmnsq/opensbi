/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#अन्तर्भाव <sbi/sbi_bitmap.h>
#अन्तर्भाव <sbi/sbi_unit_test.h>

#define DATA_SIZE परिमाण(data_zero)
#define DATA_BIT_SIZE (DATA_SIZE * 8)

स्थिर अचिन्हित दीर्घ data_a[] = { 0xDEADBEEF, 0x00BAB10C, 0x1BADB002, 0xABADBABE };
स्थिर अचिन्हित दीर्घ data_b[] = { 0xC00010FF, 0x00BAB10C, 0xBAAAAAAD, 0xBADDCAFE };
स्थिर अचिन्हित दीर्घ data_zero[] = { 0, 0, 0, 0 };

स्थिर शून्य bitmap_and_test(संरचना sbiunit_test_case *test)
{
	अचिन्हित दीर्घ res[DATA_SIZE];
	अचिन्हित दीर्घ a_and_b[] = { data_a[0] & data_b[0], data_a[1] & data_b[1],
				    data_a[2] & data_b[2], data_a[3] & data_b[3] };

	__bitmap_and(res, data_a, data_b, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_and_b, DATA_SIZE);

	/* a & a = a */
	__bitmap_and(res, data_a, data_a, DATA_BIT_SIZE);
	SBIUNIT_ASSERT_MEMEQ(test, res, data_a, DATA_SIZE);

	/* a & 0 = 0 */
	__bitmap_and(res, data_a, data_zero, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_MEMEQ(test, res, data_zero, DATA_SIZE);

	/* 0 & 0 = 0 */
	__bitmap_and(res, data_zero, data_zero, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_MEMEQ(test, res, data_zero, DATA_SIZE);

	sbi_memcpy(res, data_zero, DATA_SIZE);
	/* Cover zero 'bits' argument */
	__bitmap_and(res, data_a, data_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, data_zero, DATA_SIZE);
}

स्थिर शून्य bitmap_or_test(संरचना sbiunit_test_case *test)
{
	अचिन्हित दीर्घ res[DATA_SIZE];
	अचिन्हित दीर्घ a_or_b[] = { data_a[0] | data_b[0], data_a[1] | data_b[1],
				   data_a[2] | data_b[2], data_a[3] | data_b[3] };

	__bitmap_or(res, data_a, data_b, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_or_b, DATA_SIZE);

	/* a | a = a */
	__bitmap_or(res, data_a, data_a, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_MEMEQ(test, res, data_a, DATA_SIZE);

	/* a | 0 = a */
	__bitmap_or(res, data_a, data_zero, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_MEMEQ(test, res, data_a, DATA_SIZE);

	/* 0 | 0 = 0 */
	__bitmap_or(res, data_zero, data_zero, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_MEMEQ(test, res, data_zero, DATA_SIZE);

	sbi_memcpy(res, data_zero, DATA_SIZE);
	__bitmap_or(res, data_a, data_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, data_zero, DATA_SIZE);
}

स्थिर शून्य bitmap_xor_test(संरचना sbiunit_test_case *test)
{
	अचिन्हित दीर्घ res[DATA_SIZE];
	अचिन्हित दीर्घ a_xor_b[] = { data_a[0] ^ data_b[0], data_a[1] ^ data_b[1],
				    data_a[2] ^ data_b[2], data_a[3] ^ data_b[3] };

	__bitmap_xor(res, data_a, data_b, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_MEMEQ(test, res, a_xor_b, DATA_SIZE);

	/* a ^ 0 = a */
	__bitmap_xor(res, data_a, data_zero, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_MEMEQ(test, res, data_a, DATA_SIZE);

	/* a ^ a = 0 */
	__bitmap_xor(res, data_a, data_a, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_MEMEQ(test, res, data_zero, DATA_SIZE);

	/* 0 ^ 0 = 0 */
	__bitmap_xor(res, data_zero, data_zero, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_MEMEQ(test, res, data_zero, DATA_SIZE);

	sbi_memcpy(res, data_zero, DATA_SIZE);
	__bitmap_xor(res, data_a, data_b, 0);
	SBIUNIT_EXPECT_MEMEQ(test, res, data_zero, DATA_SIZE);
}

स्थिर शून्य bitmap_empty_test(संरचना sbiunit_test_case *test)
{
	अचिन्हित दीर्घ res[DATA_SIZE];

	/* All zeros = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(data_zero, DATA_BIT_SIZE), true);

	/* Non-zero data = not empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(data_a, DATA_BIT_SIZE), false);
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(data_b, DATA_BIT_SIZE), false);

	/* bitmap_zero creates empty bitmap */
	bitmap_zero(res, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(res, DATA_BIT_SIZE), true);

	/* bitmap_fill creates non-empty bitmap */
	bitmap_fill(res, DATA_BIT_SIZE);
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(res, DATA_BIT_SIZE), false);

	/* Single bit set = not empty */
	bitmap_zero(res, DATA_BIT_SIZE);
	bitmap_set(res, 0, 1);
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(res, DATA_BIT_SIZE), false);

	/* Zero nbits = empty */
	SBIUNIT_EXPECT_EQ(test, bitmap_empty(data_a, 0), true);
}

स्थिर संरचना sbiunit_test_case bitmap_test_cases[] = {
	SBIUNIT_TEST_CASE(bitmap_and_test),
	SBIUNIT_TEST_CASE(bitmap_or_test),
	SBIUNIT_TEST_CASE(bitmap_xor_test),
	SBIUNIT_TEST_CASE(bitmap_empty_test),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(bitmap_test_suite, bitmap_test_cases);
