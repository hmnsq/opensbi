#अन्तर्भाव <sbi/sbi_unit_test.h>
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>

स्थिर शून्य test_sbi_ecall_संस्करणम्(संरचना sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_संस्करणम्_major(), SBI_ECALL_संस्करणम्_MAJOR);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_संस्करणम्_minor(), SBI_ECALL_संस्करणम्_MINOR);
}

स्थिर शून्य test_sbi_ecall_impid(संरचना sbiunit_test_case *test)
{
	अचिह्नित दीर्घ old_impid = sbi_ecall_get_impid();
	sbi_ecall_समुच्चयः_impid(42);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_get_impid(), 42);
	sbi_ecall_समुच्चयः_impid(old_impid);
}

स्थिर पूर्णाङ्क dummy_handler(अचिह्नित दीर्घ extid, अचिह्नित दीर्घ funcid,
			 संरचना sbi_trap_regs *regs,
			 संरचना sbi_ecall_return *बहिर्गमनम्)
{
	प्रत्यागम 0;
}

स्थिर शून्य test_sbi_ecall_पञ्जिकाकोशः_find_extension(संरचना sbiunit_test_case *test)
{
	संरचना sbi_ecall_extension test_ext = {
		/* Use experimental extension space for no overlap */
		.extid_start = SBI_EXT_EXPERIMENTAL_START,
		.extid_अन्तः = SBI_EXT_EXPERIMENTAL_START,
		.name = "TestExt",
		.handle = dummy_handler,
	};

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_पञ्जिकाकोशः_extension(&test_ext), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_find_extension(SBI_EXT_EXPERIMENTAL_START), &test_ext);

	sbi_ecall_unregister_extension(&test_ext);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_find_extension(SBI_EXT_EXPERIMENTAL_START), रिक्त);
}

स्थिर शून्य test_sbi_ecall_get_extensions_str_bounds(संरचना sbiunit_test_case *test)
{
	संरचना sbi_ecall_extension e1 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START,
		.extid_अन्तः = SBI_EXT_EXPERIMENTAL_START,
		.name = "Alpha",
		.handle = dummy_handler,
		.experimental = false,
	};
	संरचना sbi_ecall_extension e2 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START + 1,
		.extid_अन्तः = SBI_EXT_EXPERIMENTAL_START + 1,
		.name = "Bravo",
		.handle = dummy_handler,
		.experimental = false,
	};
	संरचना sbi_ecall_extension e3 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START + 2,
		.extid_अन्तः = SBI_EXT_EXPERIMENTAL_START + 2,
		.name = "Charli",
		.handle = dummy_handler,
		.experimental = false,
	};
	वर्ण storage[16 + 16];
	वर्ण *buf = storage;
	वर्ण big[128];
	पूर्णाङ्क i;
	पूर्णाङ्क प्राप्तम्_alpha = 0;

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_पञ्जिकाकोशः_extension(&e1), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_पञ्जिकाकोशः_extension(&e2), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_पञ्जिकाकोशः_extension(&e3), 0);

	प्रति (i = 16; i < 32; i++)
		storage[i] = (वर्ण)0xA5;

	/* Undersized buffer must not write past the caller-provided size. */
	sbi_ecall_get_extensions_str(buf, 16, false);
	SBIUNIT_EXPECT_EQ(test, buf[15], '\0');
	प्रति (i = 16; i < 32; i++)
		SBIUNIT_EXPECT_EQ(test, (अचिह्नित वर्ण)storage[i], 0xA5);

	/* Negative control: room for the full list, including registered names. */
	sbi_ecall_get_extensions_str(big, परिमाण(big), false);
	SBIUNIT_EXPECT_NE(test, sbi_strlen(big), 0);
	प्रति (i = 0; big[i] != '\0'; i++) {
		यदि (sbi_strncmp(&big[i], "Alpha", 5) == 0) {
			प्राप्तम्_alpha = 1;
			भङ्ग;
		}
	}
	SBIUNIT_EXPECT_EQ(test, प्राप्तम्_alpha, 1);

	sbi_ecall_unregister_extension(&e1);
	sbi_ecall_unregister_extension(&e2);
	sbi_ecall_unregister_extension(&e3);
}

स्थिर संरचना sbiunit_test_case ecall_tests[] = {
	SBIUNIT_TEST_CASE(test_sbi_ecall_संस्करणम्),
	SBIUNIT_TEST_CASE(test_sbi_ecall_impid),
	SBIUNIT_TEST_CASE(test_sbi_ecall_पञ्जिकाकोशः_find_extension),
	SBIUNIT_TEST_CASE(test_sbi_ecall_get_extensions_str_bounds),
	SBIUNIT_अन्तः_CASE,
};

SBIUNIT_TEST_SUITE(ecall_test_suite, ecall_tests);
