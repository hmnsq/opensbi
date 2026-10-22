#ಅನ್ತರ್ಭಾವ <sbi/sbi_unit_test.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>

ಸ್ಥಿರ ಶೂನ್ಯ test_sbi_ecall_ಆವೃತ್ತಿ(ರಚನೆ sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ಆವೃತ್ತಿ_major(), SBI_ECALL_ಆವೃತ್ತಿ_MAJOR);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ಆವೃತ್ತಿ_minor(), SBI_ECALL_ಆವೃತ್ತಿ_MINOR);
}

ಸ್ಥಿರ ಶೂನ್ಯ test_sbi_ecall_impid(ರಚನೆ sbiunit_test_case *test)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ old_impid = sbi_ecall_get_impid();
	sbi_ecall_ಗಣ_impid(42);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_get_impid(), 42);
	sbi_ecall_ಗಣ_impid(old_impid);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dummy_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
			 ರಚನೆ sbi_trap_regs *regs,
			 ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ test_sbi_ecall_ರಿಜಿಸ್ಟರ್_ಕಂಡುಹಿಡಿ_extension(ರಚನೆ sbiunit_test_case *test)
{
	ರಚನೆ sbi_ecall_extension test_ext = {
		/* Use experimental extension space for no overlap */
		.extid_ಆರಂಭ = SBI_EXT_EXPERIMENTAL_ಆರಂಭ,
		.extid_ಕೊನೆ = SBI_EXT_EXPERIMENTAL_ಆರಂಭ,
		.name = "TestExt",
		.handle = dummy_handler,
	};

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&test_ext), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ಕಂಡುಹಿಡಿ_extension(SBI_EXT_EXPERIMENTAL_ಆರಂಭ), &test_ext);

	sbi_ecall_unregister_extension(&test_ext);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ಕಂಡುಹಿಡಿ_extension(SBI_EXT_EXPERIMENTAL_ಆರಂಭ), ಖಾಲಿ);
}

ಸ್ಥಿರ ಶೂನ್ಯ test_sbi_ecall_get_extensions_str_bounds(ರಚನೆ sbiunit_test_case *test)
{
	ರಚನೆ sbi_ecall_extension e1 = {
		.extid_ಆರಂಭ = SBI_EXT_EXPERIMENTAL_ಆರಂಭ,
		.extid_ಕೊನೆ = SBI_EXT_EXPERIMENTAL_ಆರಂಭ,
		.name = "Alpha",
		.handle = dummy_handler,
		.experimental = false,
	};
	ರಚನೆ sbi_ecall_extension e2 = {
		.extid_ಆರಂಭ = SBI_EXT_EXPERIMENTAL_ಆರಂಭ + 1,
		.extid_ಕೊನೆ = SBI_EXT_EXPERIMENTAL_ಆರಂಭ + 1,
		.name = "Bravo",
		.handle = dummy_handler,
		.experimental = false,
	};
	ರಚನೆ sbi_ecall_extension e3 = {
		.extid_ಆರಂಭ = SBI_EXT_EXPERIMENTAL_ಆರಂಭ + 2,
		.extid_ಕೊನೆ = SBI_EXT_EXPERIMENTAL_ಆರಂಭ + 2,
		.name = "Charli",
		.handle = dummy_handler,
		.experimental = false,
	};
	ಅಕ್ಷರ storage[16 + 16];
	ಅಕ್ಷರ *buf = storage;
	ಅಕ್ಷರ big[128];
	ಪೂರ್ಣಾಂಕ i;
	ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು_alpha = 0;

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&e1), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&e2), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&e3), 0);

	ಗಾಗಿ (i = 16; i < 32; i++)
		storage[i] = (ಅಕ್ಷರ)0xA5;

	/* Undersized buffer must not write past the caller-provided size. */
	sbi_ecall_get_extensions_str(buf, 16, false);
	SBIUNIT_EXPECT_EQ(test, buf[15], '\0');
	ಗಾಗಿ (i = 16; i < 32; i++)
		SBIUNIT_EXPECT_EQ(test, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)storage[i], 0xA5);

	/* Negative control: room for the full list, including registered names. */
	sbi_ecall_get_extensions_str(big, ಗಾತ್ರ(big), false);
	SBIUNIT_EXPECT_NE(test, sbi_strlen(big), 0);
	ಗಾಗಿ (i = 0; big[i] != '\0'; i++) {
		ಆದರೆ (sbi_strncmp(&big[i], "Alpha", 5) == 0) {
			ಸಿಕ್ಕಿತು_alpha = 1;
			ನಿಲ್ಲಿಸು;
		}
	}
	SBIUNIT_EXPECT_EQ(test, ಸಿಕ್ಕಿತು_alpha, 1);

	sbi_ecall_unregister_extension(&e1);
	sbi_ecall_unregister_extension(&e2);
	sbi_ecall_unregister_extension(&e3);
}

ಸ್ಥಿರ ರಚನೆ sbiunit_test_case ecall_tests[] = {
	SBIUNIT_TEST_CASE(test_sbi_ecall_ಆವೃತ್ತಿ),
	SBIUNIT_TEST_CASE(test_sbi_ecall_impid),
	SBIUNIT_TEST_CASE(test_sbi_ecall_ರಿಜಿಸ್ಟರ್_ಕಂಡುಹಿಡಿ_extension),
	SBIUNIT_TEST_CASE(test_sbi_ecall_get_extensions_str_bounds),
	SBIUNIT_ಕೊನೆ_CASE,
};

SBIUNIT_TEST_SUITE(ecall_test_suite, ecall_tests);
