/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#అన్తర్భావ <sbi/riscv_locks.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_unit_test.h>

#నిర్వచించు TEST_CONSOLE_BUF_LEN 1024

స్థిర స్థిరరాశి నిర్మాణం sbi_console_device *old_dev;
స్థిర అక్షరం test_console_buf[TEST_CONSOLE_BUF_LEN];
స్థిర u32 test_console_buf_pos;
స్థిర spinlock_t test_console_తాళం = SPIN_తాళం_INITIALIZER;

స్థిర శూన్య test_console_putc(అక్షరం c)
{
	test_console_buf[test_console_buf_pos] = c;
	test_console_buf_pos = (test_console_buf_pos + 1) % TEST_CONSOLE_BUF_LEN;
}

స్థిర శూన్య clear_test_console_buf(శూన్య)
{
	test_console_buf_pos = 0;
	test_console_buf[0] = '\0';
}

స్థిర స్థిరరాశి నిర్మాణం sbi_console_device test_console_dev = {
	.name = "Test console device",
	.console_putc = test_console_putc,
};

/* Mock the console device */
స్థిర అంతఃవరుస శూన్య test_console_begin(స్థిరరాశి నిర్మాణం sbi_console_device *device)
{
	old_dev = sbi_console_get_device();
	sbi_console_సమితి_device(device);
}

స్థిర అంతఃవరుస శూన్య test_console_చివర(శూన్య)
{
	sbi_console_సమితి_device(old_dev);
}

స్థిర శూన్య putc_test(నిర్మాణం sbiunit_test_case *test)
{
	clear_test_console_buf();
	test_console_begin(&test_console_dev);
	sbi_putc('a');
	test_console_చివర();
	SBIUNIT_ASSERT_EQ(test, test_console_buf[0], 'a');
}

#నిర్వచించు PUTS_TEST(test, expected, str) చేయి {			\
	spin_తాళం(&test_console_తాళం);				\
	clear_test_console_buf();				\
	test_console_begin(&test_console_dev);			\
	sbi_puts(str);						\
	test_console_చివర();					\
	SBIUNIT_ASSERT_STREQ(test, test_console_buf, expected,	\
			     sbi_strlen(expected));		\
	spin_unlock(&test_console_తాళం);			\
} అయితే (0)

స్థిర శూన్య puts_test(నిర్మాణం sbiunit_test_case *test)
{
	PUTS_TEST(test, "Hello, OpenSBI!", "Hello, OpenSBI!");
	PUTS_TEST(test, "Hello,\r\nOpenSBI!", "Hello,\nOpenSBI!");
}

#నిర్వచించు PRINTF_TEST(test, expected, format, ...) చేయి {		\
	spin_తాళం(&test_console_తాళం);				\
	clear_test_console_buf();				\
	test_console_begin(&test_console_dev);			\
	size_t __res = sbi_printf(format, ##__VA_ARGS__);	\
	test_console_చివర();					\
	SBIUNIT_ASSERT_EQ(test, __res, sbi_strlen(expected));	\
	SBIUNIT_ASSERT_STREQ(test, test_console_buf, expected,	\
			     sbi_strlen(expected));		\
	spin_unlock(&test_console_తాళం);			\
} అయితే (0)

స్థిర శూన్య printf_test(నిర్మాణం sbiunit_test_case *test)
{
	PRINTF_TEST(test, "Hello", "Hello");
	PRINTF_TEST(test, "3 5 7", "%d %d %d", 3, 5, 7);
	PRINTF_TEST(test, "Hello", "%s", "Hello");
	PRINTF_TEST(test, "-1", "%d", -1);
	PRINTF_TEST(test, "FF", "%X", 255);
	PRINTF_TEST(test, "ff", "%x", 255);
	PRINTF_TEST(test, "A", "%c", 'A');
	PRINTF_TEST(test, "1fe", "%p", (శూన్య *)0x1fe);
	PRINTF_TEST(test, "4294967295", "%u", 4294967295U);
	PRINTF_TEST(test, "-2147483647", "%ld", -2147483647l);
	PRINTF_TEST(test, "-9223372036854775807", "%lld", -9223372036854775807LL);
	PRINTF_TEST(test, "18446744073709551615", "%llu", 18446744073709551615ULL);
}

స్థిర నిర్మాణం sbiunit_test_case console_test_cases[] = {
	SBIUNIT_TEST_CASE(putc_test),
	SBIUNIT_TEST_CASE(puts_test),
	SBIUNIT_TEST_CASE(printf_test),
	SBIUNIT_చివర_CASE,
};

SBIUNIT_TEST_SUITE(console_test_suite, console_test_cases);
