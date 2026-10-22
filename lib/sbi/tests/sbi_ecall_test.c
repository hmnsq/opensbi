#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unit_test.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>

ਸਥਿਰ ਖਾਲੀ test_sbi_ecall_ਸੰਸਕਰਣ(ਬਣਤਰ sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ਸੰਸਕਰਣ_major(), SBI_ECALL_ਸੰਸਕਰਣ_MAJOR);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ਸੰਸਕਰਣ_minor(), SBI_ECALL_ਸੰਸਕਰਣ_MINOR);
}

ਸਥਿਰ ਖਾਲੀ test_sbi_ecall_impid(ਬਣਤਰ sbiunit_test_case *test)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ old_impid = sbi_ecall_get_impid();
	sbi_ecall_ਸੈੱਟ_impid(42);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_get_impid(), 42);
	sbi_ecall_ਸੈੱਟ_impid(old_impid);
}

ਸਥਿਰ ਪੂਰਨ dummy_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
			 ਬਣਤਰ sbi_trap_regs *regs,
			 ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ test_sbi_ecall_ਰਜਿਸਟਰ_ਲੱਭੋ_extension(ਬਣਤਰ sbiunit_test_case *test)
{
	ਬਣਤਰ sbi_ecall_extension test_ext = {
		/* Use experimental extension space for no overlap */
		.extid_ਸ਼ੁਰੂ = SBI_EXT_EXPERIMENTAL_ਸ਼ੁਰੂ,
		.extid_ਸਿਰਾ = SBI_EXT_EXPERIMENTAL_ਸ਼ੁਰੂ,
		.name = "TestExt",
		.handle = dummy_handler,
	};

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ਰਜਿਸਟਰ_extension(&test_ext), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ਲੱਭੋ_extension(SBI_EXT_EXPERIMENTAL_ਸ਼ੁਰੂ), &test_ext);

	sbi_ecall_unregister_extension(&test_ext);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ਲੱਭੋ_extension(SBI_EXT_EXPERIMENTAL_ਸ਼ੁਰੂ), ਨਿਰਾ);
}

ਸਥਿਰ ਖਾਲੀ test_sbi_ecall_get_extensions_str_bounds(ਬਣਤਰ sbiunit_test_case *test)
{
	ਬਣਤਰ sbi_ecall_extension e1 = {
		.extid_ਸ਼ੁਰੂ = SBI_EXT_EXPERIMENTAL_ਸ਼ੁਰੂ,
		.extid_ਸਿਰਾ = SBI_EXT_EXPERIMENTAL_ਸ਼ੁਰੂ,
		.name = "Alpha",
		.handle = dummy_handler,
		.experimental = false,
	};
	ਬਣਤਰ sbi_ecall_extension e2 = {
		.extid_ਸ਼ੁਰੂ = SBI_EXT_EXPERIMENTAL_ਸ਼ੁਰੂ + 1,
		.extid_ਸਿਰਾ = SBI_EXT_EXPERIMENTAL_ਸ਼ੁਰੂ + 1,
		.name = "Bravo",
		.handle = dummy_handler,
		.experimental = false,
	};
	ਬਣਤਰ sbi_ecall_extension e3 = {
		.extid_ਸ਼ੁਰੂ = SBI_EXT_EXPERIMENTAL_ਸ਼ੁਰੂ + 2,
		.extid_ਸਿਰਾ = SBI_EXT_EXPERIMENTAL_ਸ਼ੁਰੂ + 2,
		.name = "Charli",
		.handle = dummy_handler,
		.experimental = false,
	};
	ਅੱਖਰ storage[16 + 16];
	ਅੱਖਰ *buf = storage;
	ਅੱਖਰ big[128];
	ਪੂਰਨ i;
	ਪੂਰਨ ਮਿਲਿਆ_alpha = 0;

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ਰਜਿਸਟਰ_extension(&e1), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ਰਜਿਸਟਰ_extension(&e2), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ਰਜਿਸਟਰ_extension(&e3), 0);

	ਲਈ (i = 16; i < 32; i++)
		storage[i] = (ਅੱਖਰ)0xA5;

	/* Undersized buffer must not write past the caller-provided size. */
	sbi_ecall_get_extensions_str(buf, 16, false);
	SBIUNIT_EXPECT_EQ(test, buf[15], '\0');
	ਲਈ (i = 16; i < 32; i++)
		SBIUNIT_EXPECT_EQ(test, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)storage[i], 0xA5);

	/* Negative control: room for the full list, including registered names. */
	sbi_ecall_get_extensions_str(big, ਆਕਾਰ(big), false);
	SBIUNIT_EXPECT_NE(test, sbi_strlen(big), 0);
	ਲਈ (i = 0; big[i] != '\0'; i++) {
		ਜੇ (sbi_strncmp(&big[i], "Alpha", 5) == 0) {
			ਮਿਲਿਆ_alpha = 1;
			ਰੋਕੋ;
		}
	}
	SBIUNIT_EXPECT_EQ(test, ਮਿਲਿਆ_alpha, 1);

	sbi_ecall_unregister_extension(&e1);
	sbi_ecall_unregister_extension(&e2);
	sbi_ecall_unregister_extension(&e3);
}

ਸਥਿਰ ਬਣਤਰ sbiunit_test_case ecall_tests[] = {
	SBIUNIT_TEST_CASE(test_sbi_ecall_ਸੰਸਕਰਣ),
	SBIUNIT_TEST_CASE(test_sbi_ecall_impid),
	SBIUNIT_TEST_CASE(test_sbi_ecall_ਰਜਿਸਟਰ_ਲੱਭੋ_extension),
	SBIUNIT_TEST_CASE(test_sbi_ecall_get_extensions_str_bounds),
	SBIUNIT_ਸਿਰਾ_CASE,
};

SBIUNIT_TEST_SUITE(ecall_test_suite, ecall_tests);
