/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Author: Ivan Orlov <ivan.orlov0322@gmail.com>
 */
#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_unit_test.h>

#ವ್ಯಾಖ್ಯಾನಿಸು TEST_CONSOLE_BUF_LEN 1024

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_console_device *old_dev;
ಸ್ಥಿರ ಅಕ್ಷರ test_console_buf[TEST_CONSOLE_BUF_LEN];
ಸ್ಥಿರ u32 test_console_buf_pos;
ಸ್ಥಿರ spinlock_t test_console_ಬೀಗ = SPIN_ಬೀಗ_INITIALIZER;

ಸ್ಥಿರ ಶೂನ್ಯ test_console_putc(ಅಕ್ಷರ c)
{
	test_console_buf[test_console_buf_pos] = c;
	test_console_buf_pos = (test_console_buf_pos + 1) % TEST_CONSOLE_BUF_LEN;
}

ಸ್ಥಿರ ಶೂನ್ಯ clear_test_console_buf(ಶೂನ್ಯ)
{
	test_console_buf_pos = 0;
	test_console_buf[0] = '\0';
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_console_device test_console_dev = {
	.name = "Test console device",
	.console_putc = test_console_putc,
};

/* Mock the console device */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ test_console_begin(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_console_device *device)
{
	old_dev = sbi_console_get_device();
	sbi_console_ಗಣ_device(device);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ test_console_ಕೊನೆ(ಶೂನ್ಯ)
{
	sbi_console_ಗಣ_device(old_dev);
}

ಸ್ಥಿರ ಶೂನ್ಯ putc_test(ರಚನೆ sbiunit_test_case *test)
{
	clear_test_console_buf();
	test_console_begin(&test_console_dev);
	sbi_putc('a');
	test_console_ಕೊನೆ();
	SBIUNIT_ASSERT_EQ(test, test_console_buf[0], 'a');
}

#ವ್ಯಾಖ್ಯಾನಿಸು PUTS_TEST(test, expected, str) ಮಾಡು {			\
	spin_ಬೀಗ(&test_console_ಬೀಗ);				\
	clear_test_console_buf();				\
	test_console_begin(&test_console_dev);			\
	sbi_puts(str);						\
	test_console_ಕೊನೆ();					\
	SBIUNIT_ASSERT_STREQ(test, test_console_buf, expected,	\
			     sbi_strlen(expected));		\
	spin_unlock(&test_console_ಬೀಗ);			\
} ಆಗ (0)

ಸ್ಥಿರ ಶೂನ್ಯ puts_test(ರಚನೆ sbiunit_test_case *test)
{
	PUTS_TEST(test, "Hello, OpenSBI!", "Hello, OpenSBI!");
	PUTS_TEST(test, "Hello,\r\nOpenSBI!", "Hello,\nOpenSBI!");
}

#ವ್ಯಾಖ್ಯಾನಿಸು PRINTF_TEST(test, expected, format, ...) ಮಾಡು {		\
	spin_ಬೀಗ(&test_console_ಬೀಗ);				\
	clear_test_console_buf();				\
	test_console_begin(&test_console_dev);			\
	size_t __res = sbi_printf(format, ##__VA_ARGS__);	\
	test_console_ಕೊನೆ();					\
	SBIUNIT_ASSERT_EQ(test, __res, sbi_strlen(expected));	\
	SBIUNIT_ASSERT_STREQ(test, test_console_buf, expected,	\
			     sbi_strlen(expected));		\
	spin_unlock(&test_console_ಬೀಗ);			\
} ಆಗ (0)

ಸ್ಥಿರ ಶೂನ್ಯ printf_test(ರಚನೆ sbiunit_test_case *test)
{
	PRINTF_TEST(test, "Hello", "Hello");
	PRINTF_TEST(test, "3 5 7", "%d %d %d", 3, 5, 7);
	PRINTF_TEST(test, "Hello", "%s", "Hello");
	PRINTF_TEST(test, "-1", "%d", -1);
	PRINTF_TEST(test, "FF", "%X", 255);
	PRINTF_TEST(test, "ff", "%x", 255);
	PRINTF_TEST(test, "A", "%c", 'A');
	PRINTF_TEST(test, "1fe", "%p", (ಶೂನ್ಯ *)0x1fe);
	PRINTF_TEST(test, "4294967295", "%u", 4294967295U);
	PRINTF_TEST(test, "-2147483647", "%ld", -2147483647l);
	PRINTF_TEST(test, "-9223372036854775807", "%lld", -9223372036854775807LL);
	PRINTF_TEST(test, "18446744073709551615", "%llu", 18446744073709551615ULL);
}

ಸ್ಥಿರ ರಚನೆ sbiunit_test_case console_test_cases[] = {
	SBIUNIT_TEST_CASE(putc_test),
	SBIUNIT_TEST_CASE(puts_test),
	SBIUNIT_TEST_CASE(printf_test),
	SBIUNIT_ಕೊನೆ_CASE,
};

SBIUNIT_TEST_SUITE(console_test_suite, console_test_cases);
