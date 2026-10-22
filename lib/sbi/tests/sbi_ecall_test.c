#ضمن <sbi/sbi_unit_test.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>

ساكن فراغ test_sbi_ecall_نسخہ(بنية sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_نسخہ_major(), SBI_ECALL_نسخہ_MAJOR);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_نسخہ_minor(), SBI_ECALL_نسخہ_MINOR);
}

ساكن فراغ test_sbi_ecall_impid(بنية sbiunit_test_case *test)
{
	بنانشانی لما old_impid = sbi_ecall_get_impid();
	sbi_ecall_سیٹ_impid(42);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_get_impid(), 42);
	sbi_ecall_سیٹ_impid(old_impid);
}

ساكن صحيح dummy_handler(بنانشانی لما extid, بنانشانی لما funcid,
			 بنية sbi_trap_regs *regs,
			 بنية sbi_ecall_return *باہرلا)
{
	أرجع 0;
}

ساكن فراغ test_sbi_ecall_رجسٹر_لبھو_extension(بنية sbiunit_test_case *test)
{
	بنية sbi_ecall_extension test_ext = {
		/* Use experimental extension space for no overlap */
		.extid_شروع = SBI_EXT_EXPERIMENTAL_شروع,
		.extid_سرا = SBI_EXT_EXPERIMENTAL_شروع,
		.name = "TestExt",
		.handle = dummy_handler,
	};

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_رجسٹر_extension(&test_ext), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_لبھو_extension(SBI_EXT_EXPERIMENTAL_شروع), &test_ext);

	sbi_ecall_unregister_extension(&test_ext);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_لبھو_extension(SBI_EXT_EXPERIMENTAL_شروع), عدم);
}

ساكن فراغ test_sbi_ecall_get_extensions_str_bounds(بنية sbiunit_test_case *test)
{
	بنية sbi_ecall_extension e1 = {
		.extid_شروع = SBI_EXT_EXPERIMENTAL_شروع,
		.extid_سرا = SBI_EXT_EXPERIMENTAL_شروع,
		.name = "Alpha",
		.handle = dummy_handler,
		.experimental = false,
	};
	بنية sbi_ecall_extension e2 = {
		.extid_شروع = SBI_EXT_EXPERIMENTAL_شروع + 1,
		.extid_سرا = SBI_EXT_EXPERIMENTAL_شروع + 1,
		.name = "Bravo",
		.handle = dummy_handler,
		.experimental = false,
	};
	بنية sbi_ecall_extension e3 = {
		.extid_شروع = SBI_EXT_EXPERIMENTAL_شروع + 2,
		.extid_سرا = SBI_EXT_EXPERIMENTAL_شروع + 2,
		.name = "Charli",
		.handle = dummy_handler,
		.experimental = false,
	};
	حرف storage[16 + 16];
	حرف *buf = storage;
	حرف big[128];
	صحيح i;
	صحيح لبھیا_alpha = 0;

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_رجسٹر_extension(&e1), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_رجسٹر_extension(&e2), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_رجسٹر_extension(&e3), 0);

	لكل (i = 16; i < 32; i++)
		storage[i] = (حرف)0xA5;

	/* Undersized buffer must not write past the caller-provided size. */
	sbi_ecall_get_extensions_str(buf, 16, false);
	SBIUNIT_EXPECT_EQ(test, buf[15], '\0');
	لكل (i = 16; i < 32; i++)
		SBIUNIT_EXPECT_EQ(test, (بنانشانی حرف)storage[i], 0xA5);

	/* Negative control: room for the full list, including registered names. */
	sbi_ecall_get_extensions_str(big, حجم(big), false);
	SBIUNIT_EXPECT_NE(test, sbi_strlen(big), 0);
	لكل (i = 0; big[i] != '\0'; i++) {
		إذا (sbi_strncmp(&big[i], "Alpha", 5) == 0) {
			لبھیا_alpha = 1;
			اكسر;
		}
	}
	SBIUNIT_EXPECT_EQ(test, لبھیا_alpha, 1);

	sbi_ecall_unregister_extension(&e1);
	sbi_ecall_unregister_extension(&e2);
	sbi_ecall_unregister_extension(&e3);
}

ساكن بنية sbiunit_test_case ecall_tests[] = {
	SBIUNIT_TEST_CASE(test_sbi_ecall_نسخہ),
	SBIUNIT_TEST_CASE(test_sbi_ecall_impid),
	SBIUNIT_TEST_CASE(test_sbi_ecall_رجسٹر_لبھو_extension),
	SBIUNIT_TEST_CASE(test_sbi_ecall_get_extensions_str_bounds),
	SBIUNIT_سرا_CASE,
};

SBIUNIT_TEST_SUITE(ecall_test_suite, ecall_tests);
