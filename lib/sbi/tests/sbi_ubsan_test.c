/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#समावेश <sbi/sbi_unit_test.h>
#समावेश <sbi/sbi_types.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_ubsan.h>

#घोषणा UBSAN_EXPECT_FIRES(test, stmt)                                    \
	करो {                                                              \
		अचिन्हित दीर्घ _before = sbi_ubsan_report_गिनती;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_गिनती, _before); \
	} जबतक (0)

अटल व्योम test_ubsan_add_overflow(काष्ठा sbiunit_test_case *test)
{
	अस्थिर पूर्णांक a = 0x7FFFFFFF; //INT_MAx
	अस्थिर पूर्णांक b = 1;
	अस्थिर पूर्णांक c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(व्योम)c;
}

अटल व्योम test_ubsan_sub_overflow(काष्ठा sbiunit_test_case *test)
{
	अस्थिर पूर्णांक a = 0x80000000; //INT_MIN
	अस्थिर पूर्णांक b = 1;
	अस्थिर पूर्णांक c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(व्योम)c;
}

अटल व्योम test_ubsan_mul_overflow(काष्ठा sbiunit_test_case *test)
{
	अस्थिर पूर्णांक a = 0x7FFFFFFF;
	अस्थिर पूर्णांक b = 2;
	अस्थिर पूर्णांक c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(व्योम)c;
}

अटल व्योम test_ubsan_divrem(काष्ठा sbiunit_test_case *test)
{
	अस्थिर पूर्णांक a = 10;
	अस्थिर पूर्णांक b = 0;
	अस्थिर पूर्णांक c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(व्योम)c;
}

अटल व्योम test_ubsan_oob(काष्ठा sbiunit_test_case *test)
{
	अस्थिर पूर्णांक idx = 5;
	पूर्णांक arr[3]	 = { 1, 2, 3 };
	अस्थिर पूर्णांक val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(व्योम)val;
}

अटल व्योम test_ubsan_shift_too_large(काष्ठा sbiunit_test_case *test)
{
	अस्थिर अचिन्हित दीर्घ val = 1;
	अस्थिर पूर्णांक shift	   = 64;
	अस्थिर अचिन्हित दीर्घ res;
	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(व्योम)res;
}

अटल व्योम test_ubsan_shift_negative(काष्ठा sbiunit_test_case *test)
{
	अस्थिर पूर्णांक val   = 1;
	अस्थिर पूर्णांक shift = -1;
	अस्थिर पूर्णांक res;

	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(व्योम)res;
}

अटल व्योम test_ubsan_load_invalid_bool(काष्ठा sbiunit_test_case *test)
{
	अस्थिर अक्षर bool_val = 5;
	अस्थिर bool *b_ptr   = (bool *)&bool_val;
	अस्थिर पूर्णांक taken     = 0;
	UBSAN_EXPECT_FIRES(test, अगर (*b_ptr) taken = 1);
	(व्योम)taken;
}

अटल व्योम test_ubsan_pointer_overflow(काष्ठा sbiunit_test_case *test)
{
	अस्थिर uintptr_t base = 0xFFFFFFFFFFFFFFFEUL;
	अस्थिर अक्षर *ptr	= (अक्षर *)base;
	अस्थिर अक्षर *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(व्योम)res;
}

अटल काष्ठा sbiunit_test_case ubsan_tests[] = {
	SBIUNIT_TEST_CASE(test_ubsan_add_overflow),
	SBIUNIT_TEST_CASE(test_ubsan_sub_overflow),
	SBIUNIT_TEST_CASE(test_ubsan_mul_overflow),
	SBIUNIT_TEST_CASE(test_ubsan_divrem),
	SBIUNIT_TEST_CASE(test_ubsan_oob),
	SBIUNIT_TEST_CASE(test_ubsan_shift_too_large),
	SBIUNIT_TEST_CASE(test_ubsan_shift_negative),
	SBIUNIT_TEST_CASE(test_ubsan_load_invalid_bool),
	SBIUNIT_TEST_CASE(test_ubsan_pointer_overflow),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(ubsan_test_suite, ubsan_tests);
