#അന്തര്ഭാവ <sbi/sbi_unit_test.h>
#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>

സ്ഥിതം ശൂന്യം test_sbi_ecall_version(ഘടന sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_version_major(), SBI_ECALL_VERSION_MAJOR);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_version_minor(), SBI_ECALL_VERSION_MINOR);
}

സ്ഥിതം ശൂന്യം test_sbi_ecall_impid(ഘടന sbiunit_test_case *test)
{
	unsigned long old_impid = sbi_ecall_get_impid();
	sbi_ecall_set_impid(42);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_get_impid(), 42);
	sbi_ecall_set_impid(old_impid);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ dummy_handler(unsigned long extid, unsigned long funcid,
			 ഘടന sbi_trap_regs *regs,
			 ഘടന sbi_ecall_return *out)
{
	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം test_sbi_ecall_register_find_extension(ഘടന sbiunit_test_case *test)
{
	ഘടന sbi_ecall_extension test_ext = {
		/* Use experimental extension space for no overlap */
		.extid_start = SBI_EXT_EXPERIMENTAL_START,
		.extid_end = SBI_EXT_EXPERIMENTAL_START,
		.name = "TestExt",
		.handle = dummy_handler,
	};

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&test_ext), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_find_extension(SBI_EXT_EXPERIMENTAL_START), &test_ext);

	sbi_ecall_unregister_extension(&test_ext);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_find_extension(SBI_EXT_EXPERIMENTAL_START), ശൂന്യ);
}

സ്ഥിതം ശൂന്യം test_sbi_ecall_get_extensions_str_bounds(ഘടന sbiunit_test_case *test)
{
	ഘടന sbi_ecall_extension e1 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START,
		.extid_end = SBI_EXT_EXPERIMENTAL_START,
		.name = "Alpha",
		.handle = dummy_handler,
		.experimental = false,
	};
	ഘടന sbi_ecall_extension e2 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START + 1,
		.extid_end = SBI_EXT_EXPERIMENTAL_START + 1,
		.name = "Bravo",
		.handle = dummy_handler,
		.experimental = false,
	};
	ഘടന sbi_ecall_extension e3 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START + 2,
		.extid_end = SBI_EXT_EXPERIMENTAL_START + 2,
		.name = "Charli",
		.handle = dummy_handler,
		.experimental = false,
	};
	അക്ഷരം storage[16 + 16];
	അക്ഷരം *buf = storage;
	അക്ഷരം big[128];
	പൂർണ്ണസംഖ്യ i;
	പൂർണ്ണസംഖ്യ found_alpha = 0;

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&e1), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&e2), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&e3), 0);

	വേണ്ടി (i = 16; i < 32; i++)
		storage[i] = (അക്ഷരം)0xA5;

	/* Undersized buffer must not write past the caller-provided size. */
	sbi_ecall_get_extensions_str(buf, 16, false);
	SBIUNIT_EXPECT_EQ(test, buf[15], '\0');
	വേണ്ടി (i = 16; i < 32; i++)
		SBIUNIT_EXPECT_EQ(test, (unsigned അക്ഷരം)storage[i], 0xA5);

	/* Negative control: room for the full list, including registered names. */
	sbi_ecall_get_extensions_str(big, വലുപ്പം(big), false);
	SBIUNIT_EXPECT_NE(test, sbi_strlen(big), 0);
	വേണ്ടി (i = 0; big[i] != '\0'; i++) {
		എങ്കിൽ (sbi_strncmp(&big[i], "Alpha", 5) == 0) {
			found_alpha = 1;
			നിർത്തുക;
		}
	}
	SBIUNIT_EXPECT_EQ(test, found_alpha, 1);

	sbi_ecall_unregister_extension(&e1);
	sbi_ecall_unregister_extension(&e2);
	sbi_ecall_unregister_extension(&e3);
}

സ്ഥിതം ഘടന sbiunit_test_case ecall_tests[] = {
	SBIUNIT_TEST_CASE(test_sbi_ecall_version),
	SBIUNIT_TEST_CASE(test_sbi_ecall_impid),
	SBIUNIT_TEST_CASE(test_sbi_ecall_register_find_extension),
	SBIUNIT_TEST_CASE(test_sbi_ecall_get_extensions_str_bounds),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(ecall_test_suite, ecall_tests);
