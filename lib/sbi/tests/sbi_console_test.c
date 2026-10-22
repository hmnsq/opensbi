/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#അന്തര്ഭാവ <sbi/riscv_locks.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_unit_test.h>

#define TEST_CONSOLE_BUF_LEN 1024

സ്ഥിതം സ്ഥിരം ഘടന sbi_console_device *old_dev;
സ്ഥിതം അക്ഷരം test_console_buf[TEST_CONSOLE_BUF_LEN];
സ്ഥിതം u32 test_console_buf_pos;
സ്ഥിതം spinlock_t test_console_lock = SPIN_LOCK_INITIALIZER;

സ്ഥിതം ശൂന്യം test_console_putc(അക്ഷരം c)
{
	test_console_buf[test_console_buf_pos] = c;
	test_console_buf_pos = (test_console_buf_pos + 1) % TEST_CONSOLE_BUF_LEN;
}

സ്ഥിതം ശൂന്യം clear_test_console_buf(ശൂന്യം)
{
	test_console_buf_pos = 0;
	test_console_buf[0] = '\0';
}

സ്ഥിതം സ്ഥിരം ഘടന sbi_console_device test_console_dev = {
	.name = "Test console device",
	.console_putc = test_console_putc,
};

/* Mock the console device */
സ്ഥിതം വരിയിൽ ശൂന്യം test_console_begin(സ്ഥിരം ഘടന sbi_console_device *device)
{
	old_dev = sbi_console_get_device();
	sbi_console_set_device(device);
}

സ്ഥിതം വരിയിൽ ശൂന്യം test_console_end(ശൂന്യം)
{
	sbi_console_set_device(old_dev);
}

സ്ഥിതം ശൂന്യം putc_test(ഘടന sbiunit_test_case *test)
{
	clear_test_console_buf();
	test_console_begin(&test_console_dev);
	sbi_putc('a');
	test_console_end();
	SBIUNIT_ASSERT_EQ(test, test_console_buf[0], 'a');
}

#define PUTS_TEST(test, expected, str) ചെയ്യുക {			\
	spin_lock(&test_console_lock);				\
	clear_test_console_buf();				\
	test_console_begin(&test_console_dev);			\
	sbi_puts(str);						\
	test_console_end();					\
	SBIUNIT_ASSERT_STREQ(test, test_console_buf, expected,	\
			     sbi_strlen(expected));		\
	spin_unlock(&test_console_lock);			\
} ആകുമ്പോൾ (0)

സ്ഥിതം ശൂന്യം puts_test(ഘടന sbiunit_test_case *test)
{
	PUTS_TEST(test, "Hello, OpenSBI!", "Hello, OpenSBI!");
	PUTS_TEST(test, "Hello,\r\nOpenSBI!", "Hello,\nOpenSBI!");
}

#define PRINTF_TEST(test, expected, format, ...) ചെയ്യുക {		\
	spin_lock(&test_console_lock);				\
	clear_test_console_buf();				\
	test_console_begin(&test_console_dev);			\
	size_t __res = sbi_printf(format, ##__VA_ARGS__);	\
	test_console_end();					\
	SBIUNIT_ASSERT_EQ(test, __res, sbi_strlen(expected));	\
	SBIUNIT_ASSERT_STREQ(test, test_console_buf, expected,	\
			     sbi_strlen(expected));		\
	spin_unlock(&test_console_lock);			\
} ആകുമ്പോൾ (0)

സ്ഥിതം ശൂന്യം printf_test(ഘടന sbiunit_test_case *test)
{
	PRINTF_TEST(test, "Hello", "Hello");
	PRINTF_TEST(test, "3 5 7", "%d %d %d", 3, 5, 7);
	PRINTF_TEST(test, "Hello", "%s", "Hello");
	PRINTF_TEST(test, "-1", "%d", -1);
	PRINTF_TEST(test, "FF", "%X", 255);
	PRINTF_TEST(test, "ff", "%x", 255);
	PRINTF_TEST(test, "A", "%c", 'A');
	PRINTF_TEST(test, "1fe", "%p", (ശൂന്യം *)0x1fe);
	PRINTF_TEST(test, "4294967295", "%u", 4294967295U);
	PRINTF_TEST(test, "-2147483647", "%ld", -2147483647l);
	PRINTF_TEST(test, "-9223372036854775807", "%lld", -9223372036854775807LL);
	PRINTF_TEST(test, "18446744073709551615", "%llu", 18446744073709551615ULL);
}

സ്ഥിതം ഘടന sbiunit_test_case console_test_cases[] = {
	SBIUNIT_TEST_CASE(putc_test),
	SBIUNIT_TEST_CASE(puts_test),
	SBIUNIT_TEST_CASE(printf_test),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(console_test_suite, console_test_cases);
