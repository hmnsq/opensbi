#অন্তর্ভাৱ <sbi/sbi_unit_test.h>
#অন্তর্ভাৱ <sbi/sbi_ecall.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>

স্থির শূন্য test_sbi_ecall_version(সংরচনা sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_version_major(), SBI_ECALL_VERSION_MAJOR);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_version_minor(), SBI_ECALL_VERSION_MINOR);
}

স্থির শূন্য test_sbi_ecall_impid(সংরচনা sbiunit_test_case *test)
{
	unsigned long old_impid = sbi_ecall_get_impid();
	sbi_ecall_set_impid(42);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_get_impid(), 42);
	sbi_ecall_set_impid(old_impid);
}

স্থির পূর্ণাঙ্ক dummy_handler(unsigned long extid, unsigned long funcid,
			 সংরচনা sbi_trap_regs *regs,
			 সংরচনা sbi_ecall_return *out)
{
	ঘূৰাই 0;
}

স্থির শূন্য test_sbi_ecall_register_find_extension(সংরচনা sbiunit_test_case *test)
{
	সংরচনা sbi_ecall_extension test_ext = {
		/* Use experimental extension space for no overlap */
		.extid_start = SBI_EXT_EXPERIMENTAL_START,
		.extid_end = SBI_EXT_EXPERIMENTAL_START,
		.name = "TestExt",
		.handle = dummy_handler,
	};

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&test_ext), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_find_extension(SBI_EXT_EXPERIMENTAL_START), &test_ext);

	sbi_ecall_unregister_extension(&test_ext);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_find_extension(SBI_EXT_EXPERIMENTAL_START), রিক্ত);
}

স্থির শূন্য test_sbi_ecall_get_extensions_str_bounds(সংরচনা sbiunit_test_case *test)
{
	সংরচনা sbi_ecall_extension e1 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START,
		.extid_end = SBI_EXT_EXPERIMENTAL_START,
		.name = "Alpha",
		.handle = dummy_handler,
		.experimental = false,
	};
	সংরচনা sbi_ecall_extension e2 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START + 1,
		.extid_end = SBI_EXT_EXPERIMENTAL_START + 1,
		.name = "Bravo",
		.handle = dummy_handler,
		.experimental = false,
	};
	সংরচনা sbi_ecall_extension e3 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START + 2,
		.extid_end = SBI_EXT_EXPERIMENTAL_START + 2,
		.name = "Charli",
		.handle = dummy_handler,
		.experimental = false,
	};
	ৱর্ণ storage[16 + 16];
	ৱর্ণ *buf = storage;
	ৱর্ণ big[128];
	পূর্ণাঙ্ক i;
	পূর্ণাঙ্ক found_alpha = 0;

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&e1), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&e2), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&e3), 0);

	বাবে (i = 16; i < 32; i++)
		storage[i] = (ৱর্ণ)0xA5;

	/* Undersized buffer must not write past the caller-provided size. */
	sbi_ecall_get_extensions_str(buf, 16, false);
	SBIUNIT_EXPECT_EQ(test, buf[15], '\0');
	বাবে (i = 16; i < 32; i++)
		SBIUNIT_EXPECT_EQ(test, (unsigned ৱর্ণ)storage[i], 0xA5);

	/* Negative control: room for the full list, including registered names. */
	sbi_ecall_get_extensions_str(big, পরিমাণ(big), false);
	SBIUNIT_EXPECT_NE(test, sbi_strlen(big), 0);
	বাবে (i = 0; big[i] != '\0'; i++) {
		যদি (sbi_strncmp(&big[i], "Alpha", 5) == 0) {
			found_alpha = 1;
			ভঙ্গ;
		}
	}
	SBIUNIT_EXPECT_EQ(test, found_alpha, 1);

	sbi_ecall_unregister_extension(&e1);
	sbi_ecall_unregister_extension(&e2);
	sbi_ecall_unregister_extension(&e3);
}

স্থির সংরচনা sbiunit_test_case ecall_tests[] = {
	SBIUNIT_TEST_CASE(test_sbi_ecall_version),
	SBIUNIT_TEST_CASE(test_sbi_ecall_impid),
	SBIUNIT_TEST_CASE(test_sbi_ecall_register_find_extension),
	SBIUNIT_TEST_CASE(test_sbi_ecall_get_extensions_str_bounds),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(ecall_test_suite, ecall_tests);
