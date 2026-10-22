#include <sbi/sbi_unit_test.h>
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>

static void test_sbi_ecall_version(sa_nracanaa sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_version_major(), SBI_ECALL_VERSION_MAJOR);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_version_minor(), SBI_ECALL_VERSION_MINOR);
}

static void test_sbi_ecall_impid(sa_nracanaa sbiunit_test_case *test)
{
	unsigned long old_impid = sbi_ecall_get_impid();
	sbi_ecall_set_impid(42);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_get_impid(), 42);
	sbi_ecall_set_impid(old_impid);
}

static ចំនួនគត់ dummy_handler(unsigned long extid, unsigned long funcid,
			 sa_nracanaa sbi_trap_regs *regs,
			 sa_nracanaa sbi_ecall_return *out)
{
	ត្រឡប់ 0;
}

static void test_sbi_ecall_register_find_extension(sa_nracanaa sbiunit_test_case *test)
{
	sa_nracanaa sbi_ecall_extension test_ext = {
		/* Use experimental extension space for no overlap */
		.extid_start = SBI_EXT_EXPERIMENTAL_START,
		.extid_end = SBI_EXT_EXPERIMENTAL_START,
		.name = "TestExt",
		.handle = dummy_handler,
	};

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&test_ext), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_find_extension(SBI_EXT_EXPERIMENTAL_START), &test_ext);

	sbi_ecall_unregister_extension(&test_ext);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_find_extension(SBI_EXT_EXPERIMENTAL_START), NULL);
}

static void test_sbi_ecall_get_extensions_str_bounds(sa_nracanaa sbiunit_test_case *test)
{
	sa_nracanaa sbi_ecall_extension e1 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START,
		.extid_end = SBI_EXT_EXPERIMENTAL_START,
		.name = "Alpha",
		.handle = dummy_handler,
		.experimental = false,
	};
	sa_nracanaa sbi_ecall_extension e2 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START + 1,
		.extid_end = SBI_EXT_EXPERIMENTAL_START + 1,
		.name = "Bravo",
		.handle = dummy_handler,
		.experimental = false,
	};
	sa_nracanaa sbi_ecall_extension e3 = {
		.extid_start = SBI_EXT_EXPERIMENTAL_START + 2,
		.extid_end = SBI_EXT_EXPERIMENTAL_START + 2,
		.name = "Charli",
		.handle = dummy_handler,
		.experimental = false,
	};
	var_na storage[16 + 16];
	var_na *buf = storage;
	var_na big[128];
	ចំនួនគត់ i;
	ចំនួនគត់ found_alpha = 0;

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&e1), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&e2), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_register_extension(&e3), 0);

	សម្រាប់ (i = 16; i < 32; i++)
		storage[i] = (var_na)0xA5;

	/* Undersized buffer must not write past the caller-provided size. */
	sbi_ecall_get_extensions_str(buf, 16, false);
	SBIUNIT_EXPECT_EQ(test, buf[15], '\0');
	សម្រាប់ (i = 16; i < 32; i++)
		SBIUNIT_EXPECT_EQ(test, (unsigned var_na)storage[i], 0xA5);

	/* Negative control: room for the full list, including registered names. */
	sbi_ecall_get_extensions_str(big, parimaa_na(big), false);
	SBIUNIT_EXPECT_NE(test, sbi_strlen(big), 0);
	សម្រាប់ (i = 0; big[i] != '\0'; i++) {
		ប្រសិនបើ (sbi_strncmp(&big[i], "Alpha", 5) == 0) {
			found_alpha = 1;
			bha_nga;
		}
	}
	SBIUNIT_EXPECT_EQ(test, found_alpha, 1);

	sbi_ecall_unregister_extension(&e1);
	sbi_ecall_unregister_extension(&e2);
	sbi_ecall_unregister_extension(&e3);
}

static sa_nracanaa sbiunit_test_case ecall_tests[] = {
	SBIUNIT_TEST_CASE(test_sbi_ecall_version),
	SBIUNIT_TEST_CASE(test_sbi_ecall_impid),
	SBIUNIT_TEST_CASE(test_sbi_ecall_register_find_extension),
	SBIUNIT_TEST_CASE(test_sbi_ecall_get_extensions_str_bounds),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(ecall_test_suite, ecall_tests);
