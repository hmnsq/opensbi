/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#شامل <sbi/sbi_unit_test.h>
#شامل <sbi/sbi_types.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_ubsan.h>

#تعریف_کرو UBSAN_EXPECT_FIRES(test, stmt)                                    \
	کرو {                                                              \
		غیرعلامتی طویل _before = sbi_ubsan_report_گنتی;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_گنتی, _before); \
	} جب (0)

ساکن خالی test_ubsan_جوڑو_حد_سے_تجاوز(ساخت sbiunit_test_case *test)
{
	متلون صحیح a = 0x7FFFFFFF; //INT_MAx
	متلون صحیح b = 1;
	متلون صحیح c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(خالی)c;
}

ساکن خالی test_ubsan_sub_حد_سے_تجاوز(ساخت sbiunit_test_case *test)
{
	متلون صحیح a = 0x80000000; //INT_MIN
	متلون صحیح b = 1;
	متلون صحیح c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(خالی)c;
}

ساکن خالی test_ubsan_mul_حد_سے_تجاوز(ساخت sbiunit_test_case *test)
{
	متلون صحیح a = 0x7FFFFFFF;
	متلون صحیح b = 2;
	متلون صحیح c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(خالی)c;
}

ساکن خالی test_ubsan_divrem(ساخت sbiunit_test_case *test)
{
	متلون صحیح a = 10;
	متلون صحیح b = 0;
	متلون صحیح c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(خالی)c;
}

ساکن خالی test_ubsan_oob(ساخت sbiunit_test_case *test)
{
	متلون صحیح idx = 5;
	صحیح arr[3]	 = { 1, 2, 3 };
	متلون صحیح val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(خالی)val;
}

ساکن خالی test_ubsan_سرکاؤ_too_large(ساخت sbiunit_test_case *test)
{
	متلون غیرعلامتی طویل val = 1;
	متلون صحیح سرکاؤ	   = 64;
	متلون غیرعلامتی طویل res;
	UBSAN_EXPECT_FIRES(test, res = val << سرکاؤ);
	(خالی)res;
}

ساکن خالی test_ubsan_سرکاؤ_negative(ساخت sbiunit_test_case *test)
{
	متلون صحیح val   = 1;
	متلون صحیح سرکاؤ = -1;
	متلون صحیح res;

	UBSAN_EXPECT_FIRES(test, res = val << سرکاؤ);
	(خالی)res;
}

ساکن خالی test_ubsan_بھرو_invalid_bool(ساخت sbiunit_test_case *test)
{
	متلون حرف bool_val = 5;
	متلون bool *b_ptr   = (bool *)&bool_val;
	متلون صحیح لیا_گیا     = 0;
	UBSAN_EXPECT_FIRES(test, اگر (*b_ptr) لیا_گیا = 1);
	(خالی)لیا_گیا;
}

ساکن خالی test_ubsan_اشارہ_گر_حد_سے_تجاوز(ساخت sbiunit_test_case *test)
{
	متلون uintptr_t بنیاد_2 = 0xFFFFFFFFFFFFFFFEUL;
	متلون حرف *ptr	= (حرف *)بنیاد_2;
	متلون حرف *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(خالی)res;
}

ساکن ساخت sbiunit_test_case ubsan_tests[] = {
	SBIUNIT_TEST_CASE(test_ubsan_جوڑو_حد_سے_تجاوز),
	SBIUNIT_TEST_CASE(test_ubsan_sub_حد_سے_تجاوز),
	SBIUNIT_TEST_CASE(test_ubsan_mul_حد_سے_تجاوز),
	SBIUNIT_TEST_CASE(test_ubsan_divrem),
	SBIUNIT_TEST_CASE(test_ubsan_oob),
	SBIUNIT_TEST_CASE(test_ubsan_سرکاؤ_too_large),
	SBIUNIT_TEST_CASE(test_ubsan_سرکاؤ_negative),
	SBIUNIT_TEST_CASE(test_ubsan_بھرو_invalid_bool),
	SBIUNIT_TEST_CASE(test_ubsan_اشارہ_گر_حد_سے_تجاوز),
	SBIUNIT_انتہا_CASE,
};

SBIUNIT_TEST_SUITE(ubsan_test_suite, ubsan_tests);
