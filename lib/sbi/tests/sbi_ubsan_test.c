/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#अन्तर्भाव <sbi/sbi_unit_test.h>
#अन्तर्भाव <sbi/sbi_types.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_ubsan.h>

#परिभाषय UBSAN_EXPECT_FIRES(test, stmt)                                    \
	कुरु {                                                              \
		अचिह्नित दीर्घ _before = sbi_ubsan_report_गणनम्;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_गणनम्, _before); \
	} यावत् (0)

स्थिर शून्य test_ubsan_सङ्कलय_अतिप्रवाहः(संरचना sbiunit_test_case *test)
{
	चञ्चल पूर्णाङ्क a = 0x7FFFFFFF; //INT_MAx
	चञ्चल पूर्णाङ्क b = 1;
	चञ्चल पूर्णाङ्क c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(शून्य)c;
}

स्थिर शून्य test_ubsan_sub_अतिप्रवाहः(संरचना sbiunit_test_case *test)
{
	चञ्चल पूर्णाङ्क a = 0x80000000; //INT_MIN
	चञ्चल पूर्णाङ्क b = 1;
	चञ्चल पूर्णाङ्क c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(शून्य)c;
}

स्थिर शून्य test_ubsan_mul_अतिप्रवाहः(संरचना sbiunit_test_case *test)
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

स्थिर शून्य test_ubsan_सरणम्_too_large(संरचना sbiunit_test_case *test)
{
	चञ्चल अचिह्नित दीर्घ val = 1;
	चञ्चल पूर्णाङ्क सरणम्	   = 64;
	चञ्चल अचिह्नित दीर्घ res;
	UBSAN_EXPECT_FIRES(test, res = val << सरणम्);
	(शून्य)res;
}

स्थिर शून्य test_ubsan_सरणम्_negative(संरचना sbiunit_test_case *test)
{
	चञ्चल पूर्णाङ्क val   = 1;
	चञ्चल पूर्णाङ्क सरणम् = -1;
	चञ्चल पूर्णाङ्क res;

	UBSAN_EXPECT_FIRES(test, res = val << सरणम्);
	(शून्य)res;
}

स्थिर शून्य test_ubsan_आरोपय_invalid_bool(संरचना sbiunit_test_case *test)
{
	चञ्चल वर्ण bool_val = 5;
	चञ्चल bool *b_ptr   = (bool *)&bool_val;
	चञ्चल पूर्णाङ्क गृहीतम्     = 0;
	UBSAN_EXPECT_FIRES(test, यदि (*b_ptr) गृहीतम् = 1);
	(शून्य)गृहीतम्;
}

स्थिर शून्य test_ubsan_pointer_अतिप्रवाहः(संरचना sbiunit_test_case *test)
{
	चञ्चल uintptr_t आधारः_2 = 0xFFFFFFFFFFFFFFFEUL;
	चञ्चल वर्ण *ptr	= (वर्ण *)आधारः_2;
	चञ्चल वर्ण *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(शून्य)res;
}

स्थिर संरचना sbiunit_test_case ubsan_tests[] = {
	SBIUNIT_TEST_CASE(test_ubsan_सङ्कलय_अतिप्रवाहः),
	SBIUNIT_TEST_CASE(test_ubsan_sub_अतिप्रवाहः),
	SBIUNIT_TEST_CASE(test_ubsan_mul_अतिप्रवाहः),
	SBIUNIT_TEST_CASE(test_ubsan_divrem),
	SBIUNIT_TEST_CASE(test_ubsan_oob),
	SBIUNIT_TEST_CASE(test_ubsan_सरणम्_too_large),
	SBIUNIT_TEST_CASE(test_ubsan_सरणम्_negative),
	SBIUNIT_TEST_CASE(test_ubsan_आरोपय_invalid_bool),
	SBIUNIT_TEST_CASE(test_ubsan_pointer_अतिप्रवाहः),
	SBIUNIT_अन्तः_CASE,
};

SBIUNIT_TEST_SUITE(ubsan_test_suite, ubsan_tests);
