/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#अन्तर्भाव <sbi/sbi_unit_test.h>
#अन्तर्भाव <sbi/sbi_types.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_ubsan.h>

#define UBSAN_EXPECT_FIRES(test, stmt)                                    \
	गर {                                                              \
		अचिन्हित दीर्घ _before = sbi_ubsan_report_गन्ती;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_गन्ती, _before); \
	} जबसम्म (0)

स्थिर शून्य test_ubsan_add_overflow(संरचना sbiunit_test_case *test)
{
	चञ्चल पूर्णाङ्क a = 0x7FFFFFFF; //INT_MAx
	चञ्चल पूर्णाङ्क b = 1;
	चञ्चल पूर्णाङ्क c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(शून्य)c;
}

स्थिर शून्य test_ubsan_sub_overflow(संरचना sbiunit_test_case *test)
{
	चञ्चल पूर्णाङ्क a = 0x80000000; //INT_MIN
	चञ्चल पूर्णाङ्क b = 1;
	चञ्चल पूर्णाङ्क c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(शून्य)c;
}

स्थिर शून्य test_ubsan_mul_overflow(संरचना sbiunit_test_case *test)
{
	चञ्चल पूर्णाङ्क a = 0x7FFFFFFF;
	चञ्चल पूर्णाङ्क b = 2;
	चञ्चल पूर्णाङ्क c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(शून्य)c;
}

स्थिर शून्य test_ubsan_divrem(संरचना sbiunit_test_case *test)
{
	चञ्चल पूर्णाङ्क a = 10;
	चञ्चल पूर्णाङ्क b = 0;
	चञ्चल पूर्णाङ्क c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(शून्य)c;
}

स्थिर शून्य test_ubsan_oob(संरचना sbiunit_test_case *test)
{
	चञ्चल पूर्णाङ्क idx = 5;
	पूर्णाङ्क arr[3]	 = { 1, 2, 3 };
	चञ्चल पूर्णाङ्क val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(शून्य)val;
}

स्थिर शून्य test_ubsan_shift_too_large(संरचना sbiunit_test_case *test)
{
	चञ्चल अचिन्हित दीर्घ val = 1;
	चञ्चल पूर्णाङ्क shift	   = 64;
	चञ्चल अचिन्हित दीर्घ res;
	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(शून्य)res;
}

स्थिर शून्य test_ubsan_shift_negative(संरचना sbiunit_test_case *test)
{
	चञ्चल पूर्णाङ्क val   = 1;
	चञ्चल पूर्णाङ्क shift = -1;
	चञ्चल पूर्णाङ्क res;

	UBSAN_EXPECT_FIRES(test, res = val << shift);
	(शून्य)res;
}

स्थिर शून्य test_ubsan_load_invalid_bool(संरचना sbiunit_test_case *test)
{
	चञ्चल वर्ण bool_val = 5;
	चञ्चल bool *b_ptr   = (bool *)&bool_val;
	चञ्चल पूर्णाङ्क taken     = 0;
	UBSAN_EXPECT_FIRES(test, यदि (*b_ptr) taken = 1);
	(शून्य)taken;
}

स्थिर शून्य test_ubsan_pointer_overflow(संरचना sbiunit_test_case *test)
{
	चञ्चल uintptr_t base = 0xFFFFFFFFFFFFFFFEUL;
	चञ्चल वर्ण *ptr	= (वर्ण *)base;
	चञ्चल वर्ण *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(शून्य)res;
}

स्थिर संरचना sbiunit_test_case ubsan_tests[] = {
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
