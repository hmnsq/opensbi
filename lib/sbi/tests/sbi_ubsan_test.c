/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#అన్తర్భావ <sbi/sbi_unit_test.h>
#అన్తర్భావ <sbi/sbi_types.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_ubsan.h>

#నిర్వచించు UBSAN_EXPECT_FIRES(test, stmt)                                    \
	చేయి {                                                              \
		గుర్తులేని దీర్ఘ _before = sbi_ubsan_report_లెక్క;           \
		stmt;                                                     \
		SBIUNIT_EXPECT_NE(test, sbi_ubsan_report_లెక్క, _before); \
	} అయితే (0)

స్థిర శూన్య test_ubsan_కూడు_అధికప్రవాహం(నిర్మాణం sbiunit_test_case *test)
{
	అస్థిర పూర్ణాంకం a = 0x7FFFFFFF; //INT_MAx
	అస్థిర పూర్ణాంకం b = 1;
	అస్థిర పూర్ణాంకం c;
	UBSAN_EXPECT_FIRES(test, c = a + b);
	(శూన్య)c;
}

స్థిర శూన్య test_ubsan_sub_అధికప్రవాహం(నిర్మాణం sbiunit_test_case *test)
{
	అస్థిర పూర్ణాంకం a = 0x80000000; //INT_MIN
	అస్థిర పూర్ణాంకం b = 1;
	అస్థిర పూర్ణాంకం c;
	UBSAN_EXPECT_FIRES(test, c = a - b);
	(శూన్య)c;
}

స్థిర శూన్య test_ubsan_mul_అధికప్రవాహం(నిర్మాణం sbiunit_test_case *test)
{
	అస్థిర పూర్ణాంకం a = 0x7FFFFFFF;
	అస్థిర పూర్ణాంకం b = 2;
	అస్థిర పూర్ణాంకం c;
	UBSAN_EXPECT_FIRES(test, c = a * b);
	(శూన్య)c;
}

స్థిర శూన్య test_ubsan_divrem(నిర్మాణం sbiunit_test_case *test)
{
	అస్థిర పూర్ణాంకం a = 10;
	అస్థిర పూర్ణాంకం b = 0;
	అస్థిర పూర్ణాంకం c;
	UBSAN_EXPECT_FIRES(test, c = a / b);
	(శూన్య)c;
}

స్థిర శూన్య test_ubsan_oob(నిర్మాణం sbiunit_test_case *test)
{
	అస్థిర పూర్ణాంకం idx = 5;
	పూర్ణాంకం arr[3]	 = { 1, 2, 3 };
	అస్థిర పూర్ణాంకం val;
	UBSAN_EXPECT_FIRES(test, val = arr[idx]);
	(శూన్య)val;
}

స్థిర శూన్య test_ubsan_జరుపు_too_large(నిర్మాణం sbiunit_test_case *test)
{
	అస్థిర గుర్తులేని దీర్ఘ val = 1;
	అస్థిర పూర్ణాంకం జరుపు	   = 64;
	అస్థిర గుర్తులేని దీర్ఘ res;
	UBSAN_EXPECT_FIRES(test, res = val << జరుపు);
	(శూన్య)res;
}

స్థిర శూన్య test_ubsan_జరుపు_negative(నిర్మాణం sbiunit_test_case *test)
{
	అస్థిర పూర్ణాంకం val   = 1;
	అస్థిర పూర్ణాంకం జరుపు = -1;
	అస్థిర పూర్ణాంకం res;

	UBSAN_EXPECT_FIRES(test, res = val << జరుపు);
	(శూన్య)res;
}

స్థిర శూన్య test_ubsan_ఎక్కించు_invalid_bool(నిర్మాణం sbiunit_test_case *test)
{
	అస్థిర అక్షరం bool_val = 5;
	అస్థిర bool *b_ptr   = (bool *)&bool_val;
	అస్థిర పూర్ణాంకం తీసుకున్న     = 0;
	UBSAN_EXPECT_FIRES(test, ఒకవేళ (*b_ptr) తీసుకున్న = 1);
	(శూన్య)తీసుకున్న;
}

స్థిర శూన్య test_ubsan_సూచిక_అధికప్రవాహం(నిర్మాణం sbiunit_test_case *test)
{
	అస్థిర uintptr_t పునాది_2 = 0xFFFFFFFFFFFFFFFEUL;
	అస్థిర అక్షరం *ptr	= (అక్షరం *)పునాది_2;
	అస్థిర అక్షరం *res;
	UBSAN_EXPECT_FIRES(test, res = ptr + 5);
	(శూన్య)res;
}

స్థిర నిర్మాణం sbiunit_test_case ubsan_tests[] = {
	SBIUNIT_TEST_CASE(test_ubsan_కూడు_అధికప్రవాహం),
	SBIUNIT_TEST_CASE(test_ubsan_sub_అధికప్రవాహం),
	SBIUNIT_TEST_CASE(test_ubsan_mul_అధికప్రవాహం),
	SBIUNIT_TEST_CASE(test_ubsan_divrem),
	SBIUNIT_TEST_CASE(test_ubsan_oob),
	SBIUNIT_TEST_CASE(test_ubsan_జరుపు_too_large),
	SBIUNIT_TEST_CASE(test_ubsan_జరుపు_negative),
	SBIUNIT_TEST_CASE(test_ubsan_ఎక్కించు_invalid_bool),
	SBIUNIT_TEST_CASE(test_ubsan_సూచిక_అధికప్రవాహం),
	SBIUNIT_చివర_CASE,
};

SBIUNIT_TEST_SUITE(ubsan_test_suite, ubsan_tests);
