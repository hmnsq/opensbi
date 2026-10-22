/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ضمن <sbi/sbi_unit_test.h>
#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_ubsan.h>

#تعریف_کن UBSAN_EXPECT_FIRES(test, stmt)                                    \
	افعل {                                                              \
		بیعلامت دراز _before = sbi_ubsan_report_شمار;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_شمار, _before); \
	} طالما (0)

ساكن فراغ test_ubsan_جمع_کن_سرریز(بنية sbiunit_test_case *test)
{
	متطاير صحيح a = 0x7FFFFFFF; //INT_MAx
	متطاير صحيح b = 1;
	متطاير صحيح c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(فراغ)c;
}

ساكن فراغ test_ubsan_sub_سرریز(بنية sbiunit_test_case *test)
{
	متطاير صحيح a = 0x80000000; //INT_MIN
	متطاير صحيح b = 1;
	متطاير صحيح c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(فراغ)c;
}

ساكن فراغ test_ubsan_mul_سرریز(بنية sbiunit_test_case *test)
{
	متطاير صحيح a = 0x7FFFFFFF;
	متطاير صحيح b = 2;
	متطاير صحيح c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(فراغ)c;
}

ساكن فراغ test_ubsan_divrem(بنية sbiunit_test_case *test)
{
	متطاير صحيح a = 10;
	متطاير صحيح b = 0;
	متطاير صحيح c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(فراغ)c;
}

ساكن فراغ test_ubsan_oob(بنية sbiunit_test_case *test)
{
	متطاير صحيح idx = 5;
	صحيح arr[3]	 = { 1, 2, 3 };
	متطاير صحيح val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(فراغ)val;
}

ساكن فراغ test_ubsan_لغزش_too_large(بنية sbiunit_test_case *test)
{
	متطاير بیعلامت دراز val = 1;
	متطاير صحيح لغزش	   = 64;
	متطاير بیعلامت دراز res;
	UBSAN_EXPECT_FIRES(test, res = val << لغزش);
	(فراغ)res;
}

ساكن فراغ test_ubsan_لغزش_negative(بنية sbiunit_test_case *test)
{
	متطاير صحيح val   = 1;
	متطاير صحيح لغزش = -1;
	متطاير صحيح res;

	UBSAN_EXPECT_FIRES(test, res = val << لغزش);
	(فراغ)res;
}

ساكن فراغ test_ubsan_بارگیری_invalid_bool(بنية sbiunit_test_case *test)
{
	متطاير حرف bool_val = 5;
	متطاير bool *b_ptr   = (bool *)&bool_val;
	متطاير صحيح گرفته     = 0;
	UBSAN_EXPECT_FIRES(test, إذا (*b_ptr) گرفته = 1);
	(فراغ)گرفته;
}

ساكن فراغ test_ubsan_اشاره_گر_سرریز(بنية sbiunit_test_case *test)
{
	متطاير uintptr_t پایه_2 = 0xFFFFFFFFFFFFFFFEUL;
	متطاير حرف *ptr	= (حرف *)پایه_2;
	متطاير حرف *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(فراغ)res;
}

ساكن بنية sbiunit_test_case ubsan_tests[] = {
	SBIUNIT_TEST_CASE(test_ubsan_جمع_کن_سرریز),
	SBIUNIT_TEST_CASE(test_ubsan_sub_سرریز),
	SBIUNIT_TEST_CASE(test_ubsan_mul_سرریز),
	SBIUNIT_TEST_CASE(test_ubsan_divrem),
	SBIUNIT_TEST_CASE(test_ubsan_oob),
	SBIUNIT_TEST_CASE(test_ubsan_لغزش_too_large),
	SBIUNIT_TEST_CASE(test_ubsan_لغزش_negative),
	SBIUNIT_TEST_CASE(test_ubsan_بارگیری_invalid_bool),
	SBIUNIT_TEST_CASE(test_ubsan_اشاره_گر_سرریز),
	SBIUNIT_انتها_CASE,
};

SBIUNIT_TEST_SUITE(ubsan_test_suite, ubsan_tests);
