#شامل <sbi/sbi_unit_test.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>

ساکن خالی test_sbi_ecall_نسخہ(ساخت sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_نسخہ_major(), SBI_ECALL_نسخہ_MAJOR);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_نسخہ_minor(), SBI_ECALL_نسخہ_MINOR);
}

ساکن خالی test_sbi_ecall_impid(ساخت sbiunit_test_case *test)
{
	غیرعلامتی طویل old_impid = sbi_ecall_get_impid();
	sbi_ecall_سیٹ_impid(42);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_get_impid(), 42);
	sbi_ecall_سیٹ_impid(old_impid);
}

ساکن صحیح dummy_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
			 ساخت sbi_trap_regs *regs,
			 ساخت sbi_ecall_return *برآمد)
{
	واپس 0;
}

ساکن خالی test_sbi_ecall_رجسٹر_ڈھونڈو_extension(ساخت sbiunit_test_case *test)
{
	ساخت sbi_ecall_extension test_ext = {
		/* Use experimental extension space for no overlap */
		.extid_آغاز = SBI_EXT_EXPERIMENTAL_آغاز,
		.extid_انتہا = SBI_EXT_EXPERIMENTAL_آغاز,
		.name = "TestExt",
		.handle = dummy_handler,
	};

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_رجسٹر_extension(&test_ext), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ڈھونڈو_extension(SBI_EXT_EXPERIMENTAL_آغاز), &test_ext);

	sbi_ecall_unregister_extension(&test_ext);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_ڈھونڈو_extension(SBI_EXT_EXPERIMENTAL_آغاز), تہی);
}

ساکن خالی test_sbi_ecall_get_extensions_str_bounds(ساخت sbiunit_test_case *test)
{
	ساخت sbi_ecall_extension e1 = {
		.extid_آغاز = SBI_EXT_EXPERIMENTAL_آغاز,
		.extid_انتہا = SBI_EXT_EXPERIMENTAL_آغاز,
		.name = "Alpha",
		.handle = dummy_handler,
		.experimental = false,
	};
	ساخت sbi_ecall_extension e2 = {
		.extid_آغاز = SBI_EXT_EXPERIMENTAL_آغاز + 1,
		.extid_انتہا = SBI_EXT_EXPERIMENTAL_آغاز + 1,
		.name = "Bravo",
		.handle = dummy_handler,
		.experimental = false,
	};
	ساخت sbi_ecall_extension e3 = {
		.extid_آغاز = SBI_EXT_EXPERIMENTAL_آغاز + 2,
		.extid_انتہا = SBI_EXT_EXPERIMENTAL_آغاز + 2,
		.name = "Charli",
		.handle = dummy_handler,
		.experimental = false,
	};
	حرف storage[16 + 16];
	حرف *buf = storage;
	حرف big[128];
	صحیح i;
	صحیح ملا_alpha = 0;

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_رجسٹر_extension(&e1), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_رجسٹر_extension(&e2), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_رجسٹر_extension(&e3), 0);

	برائے (i = 16; i < 32; i++)
		storage[i] = (حرف)0xA5;

	/* Undersized buffer must not write past the caller-provided size. */
	sbi_ecall_get_extensions_str(buf, 16, false);
	SBIUNIT_EXPECT_EQ(test, buf[15], '\0');
	برائے (i = 16; i < 32; i++)
		SBIUNIT_EXPECT_EQ(test, (غیرعلامتی حرف)storage[i], 0xA5);

	/* Negative control: room for the full list, including registered names. */
	sbi_ecall_get_extensions_str(big, حجم(big), false);
	SBIUNIT_EXPECT_NE(test, sbi_strlen(big), 0);
	برائے (i = 0; big[i] != '\0'; i++) {
		اگر (sbi_strncmp(&big[i], "Alpha", 5) == 0) {
			ملا_alpha = 1;
			توڑو;
		}
	}
	SBIUNIT_EXPECT_EQ(test, ملا_alpha, 1);

	sbi_ecall_unregister_extension(&e1);
	sbi_ecall_unregister_extension(&e2);
	sbi_ecall_unregister_extension(&e3);
}

ساکن ساخت sbiunit_test_case ecall_tests[] = {
	SBIUNIT_TEST_CASE(test_sbi_ecall_نسخہ),
	SBIUNIT_TEST_CASE(test_sbi_ecall_impid),
	SBIUNIT_TEST_CASE(test_sbi_ecall_رجسٹر_ڈھونڈو_extension),
	SBIUNIT_TEST_CASE(test_sbi_ecall_get_extensions_str_bounds),
	SBIUNIT_انتہا_CASE,
};

SBIUNIT_TEST_SUITE(ecall_test_suite, ecall_tests);
