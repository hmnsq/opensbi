#समावेश <sbi/sbi_unit_test.h>
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>

अटल व्योम test_sbi_ecall_version(काष्ठा sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_version_major(), SBI_ECALL_VERSION_MAJOR);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_version_minor(), SBI_ECALL_VERSION_MINOR);
}

अटल व्योम test_sbi_ecall_impid(काष्ठा sbiunit_test_case *test)
{
	अचिन्हित दीर्घ old_impid = sbi_ecall_get_impid();
	sbi_ecall_set_impid(42);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_get_impid(), 42);
	sbi_ecall_set_impid(old_impid);
}

अटल पूर्णांक dummy_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
			 काष्ठा sbi_trap_regs *regs,
			 काष्ठा sbi_ecall_return *out)
{
	वापस 0;
}

अटल व्योम test_sbi_ecall_register_find_extension(काष्ठा sbiunit_test_case *test)
{
	काष्ठा sbi_ecall_extension test_ext = {
		/* Use experimental extension space for no overlap */
		.extid_start = SBI_EXT_EXPERIMENTAL_START,
		.extid_end = SBI_EXT_EXPERIMENTAL_START,
		.name = "TestExt",
		.handle = dummy_handler,
	};

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&test_ext), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_find_extension(SBI_EXT_EXPERIMENTAL_START), &test_ext);

	sbi_ecall_unregister_extension(&test_ext);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_find_extension(SBI_EXT_EXPERIMENTAL_START), शून्य);
}

अटल व्योम test_sbi_ecall_get_extensions_str_bounds(काष्ठा sbiunit_test_case *test)
{
	काष्ठा sbi_ecall_extension e1 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START,
		.extid_end = SBI_EXT_EXPERIMENTAL_START,
		.name = "Alpha",
		.handle = dummy_handler,
		.experimental = false,
	};
	काष्ठा sbi_ecall_extension e2 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START + 1,
		.extid_end = SBI_EXT_EXPERIMENTAL_START + 1,
		.name = "Bravo",
		.handle = dummy_handler,
		.experimental = false,
	};
	काष्ठा sbi_ecall_extension e3 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START + 2,
		.extid_end = SBI_EXT_EXPERIMENTAL_START + 2,
		.name = "Charli",
		.handle = dummy_handler,
		.experimental = false,
	};
	अक्षर storage[16 + 16];
	अक्षर *buf = storage;
	अक्षर big[128];
	पूर्णांक i;
	पूर्णांक found_alpha = 0;

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&e1), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&e2), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&e3), 0);

	क्रम (i = 16; i < 32; i++)
		storage[i] = (अक्षर)0xA5;

	/* Undersized buffer must not write past the caller-provided size. */
	sbi_ecall_get_extensions_str(buf, 16, false);
	SBIUNIT_EXPECT_EQ(test, buf[15], '\0');
	क्रम (i = 16; i < 32; i++)
		SBIUNIT_EXPECT_EQ(test, (अचिन्हित अक्षर)storage[i], 0xA5);

	/* Negative control: room for the full list, including registered names. */
	sbi_ecall_get_extensions_str(big, माप(big), false);
	SBIUNIT_EXPECT_NE(test, sbi_strlen(big), 0);
	क्रम (i = 0; big[i] != '\0'; i++) {
		अगर (sbi_strncmp(&big[i], "Alpha", 5) == 0) {
			found_alpha = 1;
			अवरोध;
		}
	}
	SBIUNIT_EXPECT_EQ(test, found_alpha, 1);

	sbi_ecall_unregister_extension(&e1);
	sbi_ecall_unregister_extension(&e2);
	sbi_ecall_unregister_extension(&e3);
}

अटल काष्ठा sbiunit_test_case ecall_tests[] = {
	SBIUNIT_TEST_CASE(test_sbi_ecall_version),
	SBIUNIT_TEST_CASE(test_sbi_ecall_impid),
	SBIUNIT_TEST_CASE(test_sbi_ecall_register_find_extension),
	SBIUNIT_TEST_CASE(test_sbi_ecall_get_extensions_str_bounds),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(ecall_test_suite, ecall_tests);
