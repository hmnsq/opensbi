/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unit_test.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ubsan.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBSAN_EXPECT_FIRES(test, stmt)                                    \
	ਕਰੋ {                                                              \
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ _before = sbi_ubsan_report_ਗਿਣਤੀ;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_ਗਿਣਤੀ, _before); \
	} ਜਦੋਂ (0)

ਸਥਿਰ ਖਾਲੀ test_ubsan_ਜੋੜੋ_ਉਛਾਲ(ਬਣਤਰ sbiunit_test_case *test)
{
	ਅਸਥਿਰ ਪੂਰਨ a = 0x7FFFFFFF; //INT_MAx
	ਅਸਥਿਰ ਪੂਰਨ b = 1;
	ਅਸਥਿਰ ਪੂਰਨ c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(ਖਾਲੀ)c;
}

ਸਥਿਰ ਖਾਲੀ test_ubsan_sub_ਉਛਾਲ(ਬਣਤਰ sbiunit_test_case *test)
{
	ਅਸਥਿਰ ਪੂਰਨ a = 0x80000000; //INT_MIN
	ਅਸਥਿਰ ਪੂਰਨ b = 1;
	ਅਸਥਿਰ ਪੂਰਨ c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(ਖਾਲੀ)c;
}

ਸਥਿਰ ਖਾਲੀ test_ubsan_mul_ਉਛਾਲ(ਬਣਤਰ sbiunit_test_case *test)
{
	ਅਸਥਿਰ ਪੂਰਨ a = 0x7FFFFFFF;
	ਅਸਥਿਰ ਪੂਰਨ b = 2;
	ਅਸਥਿਰ ਪੂਰਨ c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(ਖਾਲੀ)c;
}

ਸਥਿਰ ਖਾਲੀ test_ubsan_divrem(ਬਣਤਰ sbiunit_test_case *test)
{
	ਅਸਥਿਰ ਪੂਰਨ a = 10;
	ਅਸਥਿਰ ਪੂਰਨ b = 0;
	ਅਸਥਿਰ ਪੂਰਨ c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(ਖਾਲੀ)c;
}

ਸਥਿਰ ਖਾਲੀ test_ubsan_oob(ਬਣਤਰ sbiunit_test_case *test)
{
	ਅਸਥਿਰ ਪੂਰਨ idx = 5;
	ਪੂਰਨ arr[3]	 = { 1, 2, 3 };
	ਅਸਥਿਰ ਪੂਰਨ val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(ਖਾਲੀ)val;
}

ਸਥਿਰ ਖਾਲੀ test_ubsan_ਖਿਸਕਾਓ_too_large(ਬਣਤਰ sbiunit_test_case *test)
{
	ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val = 1;
	ਅਸਥਿਰ ਪੂਰਨ ਖਿਸਕਾਓ	   = 64;
	ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ res;
	UBSAN_EXPECT_FIRES(test, res = val << ਖਿਸਕਾਓ);
	(ਖਾਲੀ)res;
}

ਸਥਿਰ ਖਾਲੀ test_ubsan_ਖਿਸਕਾਓ_negative(ਬਣਤਰ sbiunit_test_case *test)
{
	ਅਸਥਿਰ ਪੂਰਨ val   = 1;
	ਅਸਥਿਰ ਪੂਰਨ ਖਿਸਕਾਓ = -1;
	ਅਸਥਿਰ ਪੂਰਨ res;

	UBSAN_EXPECT_FIRES(test, res = val << ਖਿਸਕਾਓ);
	(ਖਾਲੀ)res;
}

ਸਥਿਰ ਖਾਲੀ test_ubsan_ਲੱਦੋ_invalid_bool(ਬਣਤਰ sbiunit_test_case *test)
{
	ਅਸਥਿਰ ਅੱਖਰ bool_val = 5;
	ਅਸਥਿਰ bool *b_ptr   = (bool *)&bool_val;
	ਅਸਥਿਰ ਪੂਰਨ ਲਿਆ     = 0;
	UBSAN_EXPECT_FIRES(test, ਜੇ (*b_ptr) ਲਿਆ = 1);
	(ਖਾਲੀ)ਲਿਆ;
}

ਸਥਿਰ ਖਾਲੀ test_ubsan_ਸੂਚਕ_ਉਛਾਲ(ਬਣਤਰ sbiunit_test_case *test)
{
	ਅਸਥਿਰ uintptr_t ਅਧਾਰ_2 = 0xFFFFFFFFFFFFFFFEUL;
	ਅਸਥਿਰ ਅੱਖਰ *ptr	= (ਅੱਖਰ *)ਅਧਾਰ_2;
	ਅਸਥਿਰ ਅੱਖਰ *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(ਖਾਲੀ)res;
}

ਸਥਿਰ ਬਣਤਰ sbiunit_test_case ubsan_tests[] = {
	SBIUNIT_TEST_CASE(test_ubsan_ਜੋੜੋ_ਉਛਾਲ),
	SBIUNIT_TEST_CASE(test_ubsan_sub_ਉਛਾਲ),
	SBIUNIT_TEST_CASE(test_ubsan_mul_ਉਛਾਲ),
	SBIUNIT_TEST_CASE(test_ubsan_divrem),
	SBIUNIT_TEST_CASE(test_ubsan_oob),
	SBIUNIT_TEST_CASE(test_ubsan_ਖਿਸਕਾਓ_too_large),
	SBIUNIT_TEST_CASE(test_ubsan_ਖਿਸਕਾਓ_negative),
	SBIUNIT_TEST_CASE(test_ubsan_ਲੱਦੋ_invalid_bool),
	SBIUNIT_TEST_CASE(test_ubsan_ਸੂਚਕ_ਉਛਾਲ),
	SBIUNIT_ਸਿਰਾ_CASE,
};

SBIUNIT_TEST_SUITE(ubsan_test_suite, ubsan_tests);
