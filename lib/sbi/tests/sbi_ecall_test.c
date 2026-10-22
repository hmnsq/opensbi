#ضمن <sbi/sbi_unit_test.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>

ساكن فراغ test_sbi_ecall_نسخو(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_نسخو_major(), SBI_ECALL_نسخو_MAJOR);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_نسخو_minor(), SBI_ECALL_نسخو_MINOR);
}

ساكن فراغ test_sbi_ecall_impid(بنية sbiunit_test_case *test)
{
	بنانشان ڊگهو old_impid = sbi_ecall_get_impid();
	sbi_ecall_سيٽ_impid(42);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_get_impid(), 42);
	sbi_ecall_سيٽ_impid(old_impid);
}

ساكن صحيح dummy_handler(بنانشان ڊگهو extid, بنانشان ڊگهو funcid,
			 بنية sbi_trap_regs *regs,
			 بنية sbi_ecall_return *ٻاهريون)
{
	أرجع 0;
}

ساكن فراغ test_sbi_ecall_رجسٽر_ڳوليو_extension(بنية sbiunit_test_case *test)
{
	بنية sbi_ecall_extension test_ext = {
		/* Use experimental extension space for no overlap */
		.extid_شروعات = SBI_EXT_EXPERIMENTAL_شروعات,
		.extid_پڇاڙي = SBI_EXT_EXPERIMENTAL_شروعات,
		.name = "TestExt",
		.handle = dummy_handler,
	};

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_رجسٽر_extension(&test_ext), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ڳوليو_extension(SBI_EXT_EXPERIMENTAL_شروعات), &test_ext);

	sbi_ecall_unregister_extension(&test_ext);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ڳوليو_extension(SBI_EXT_EXPERIMENTAL_شروعات), عدم);
}

ساكن فراغ test_sbi_ecall_get_extensions_str_bounds(بنية sbiunit_test_case *test)
{
	بنية sbi_ecall_extension e1 = {
		.extid_شروعات = SBI_EXT_EXPERIMENTAL_شروعات,
		.extid_پڇاڙي = SBI_EXT_EXPERIMENTAL_شروعات,
		.name = "Alpha",
		.handle = dummy_handler,
		.experimental = false,
	};
	بنية sbi_ecall_extension e2 = {
		.extid_شروعات = SBI_EXT_EXPERIMENTAL_شروعات + 1,
		.extid_پڇاڙي = SBI_EXT_EXPERIMENTAL_شروعات + 1,
		.name = "Bravo",
		.handle = dummy_handler,
		.experimental = false,
	};
	بنية sbi_ecall_extension e3 = {
		.extid_شروعات = SBI_EXT_EXPERIMENTAL_شروعات + 2,
		.extid_پڇاڙي = SBI_EXT_EXPERIMENTAL_شروعات + 2,
		.name = "Charli",
		.handle = dummy_handler,
		.experimental = false,
	};
	حرف storage[16 + 16];
	حرف *buf = storage;
	حرف big[128];
	صحيح i;
	صحيح مليو_alpha = 0;

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_رجسٽر_extension(&e1), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_رجسٽر_extension(&e2), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_رجسٽر_extension(&e3), 0);

	لكل (i = 16; i < 32; i++)
		storage[i] = (حرف)0xA5;

	/* Undersized buffer must not write past the caller-provided size. */
	sbi_ecall_get_extensions_str(buf, 16, false);
	SBIUNIT_EXPECT_EQ(test, buf[15], '\0');
	لكل (i = 16; i < 32; i++)
		SBIUNIT_EXPECT_EQ(test, (بنانشان حرف)storage[i], 0xA5);

	/* Negative control: room for the full list, including registered names. */
	sbi_ecall_get_extensions_str(big, حجم(big), false);
	SBIUNIT_EXPECT_NE(test, sbi_strlen(big), 0);
	لكل (i = 0; big[i] != '\0'; i++) {
		إذا (sbi_strncmp(&big[i], "Alpha", 5) == 0) {
			مليو_alpha = 1;
			اكسر;
		}
	}
	SBIUNIT_EXPECT_EQ(test, مليو_alpha, 1);

	sbi_ecall_unregister_extension(&e1);
	sbi_ecall_unregister_extension(&e2);
	sbi_ecall_unregister_extension(&e3);
}

ساكن بنية sbiunit_test_case ecall_tests[] = {
	SBIUNIT_TEST_CASE(test_sbi_ecall_نسخو),
	SBIUNIT_TEST_CASE(test_sbi_ecall_impid),
	SBIUNIT_TEST_CASE(test_sbi_ecall_رجسٽر_ڳوليو_extension),
	SBIUNIT_TEST_CASE(test_sbi_ecall_get_extensions_str_bounds),
	SBIUNIT_پڇاڙي_CASE,
};

SBIUNIT_TEST_SUITE(ecall_test_suite, ecall_tests);
