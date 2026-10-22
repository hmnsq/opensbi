#అన్తర్భావ <sbi/sbi_unit_test.h>
#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>

స్థిర శూన్య test_sbi_ecall_సంస్కరణ(నిర్మాణం sbiunit_test_case *test)
{
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_సంస్కరణ_major(), SBI_ECALL_సంస్కరణ_MAJOR);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_సంస్కరణ_minor(), SBI_ECALL_సంస్కరణ_MINOR);
}

స్థిర శూన్య test_sbi_ecall_impid(నిర్మాణం sbiunit_test_case *test)
{
	గుర్తులేని దీర్ఘ old_impid = sbi_ecall_get_impid();
	sbi_ecall_సమితి_impid(42);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_get_impid(), 42);
	sbi_ecall_సమితి_impid(old_impid);
}

స్థిర పూర్ణాంకం dummy_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
			 నిర్మాణం sbi_trap_regs *regs,
			 నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	తిరిగి 0;
}

స్థిర శూన్య test_sbi_ecall_రిజిస్టరు_కనుగొను_extension(నిర్మాణం sbiunit_test_case *test)
{
	నిర్మాణం sbi_ecall_extension test_ext = {
		/* Use experimental extension space for no overlap */
		.extid_ప్రారంభం = SBI_EXT_EXPERIMENTAL_ప్రారంభం,
		.extid_చివర = SBI_EXT_EXPERIMENTAL_ప్రారంభం,
		.name = "TestExt",
		.handle = dummy_handler,
	};

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_రిజిస్టరు_extension(&test_ext), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_కనుగొను_extension(SBI_EXT_EXPERIMENTAL_ప్రారంభం), &test_ext);

	sbi_ecall_unregister_extension(&test_ext);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_కనుగొను_extension(SBI_EXT_EXPERIMENTAL_ప్రారంభం), ఖాళీ);
}

స్థిర శూన్య test_sbi_ecall_get_extensions_str_bounds(నిర్మాణం sbiunit_test_case *test)
{
	నిర్మాణం sbi_ecall_extension e1 = {
		.extid_ప్రారంభం = SBI_EXT_EXPERIMENTAL_ప్రారంభం,
		.extid_చివర = SBI_EXT_EXPERIMENTAL_ప్రారంభం,
		.name = "Alpha",
		.handle = dummy_handler,
		.experimental = false,
	};
	నిర్మాణం sbi_ecall_extension e2 = {
		.extid_ప్రారంభం = SBI_EXT_EXPERIMENTAL_ప్రారంభం + 1,
		.extid_చివర = SBI_EXT_EXPERIMENTAL_ప్రారంభం + 1,
		.name = "Bravo",
		.handle = dummy_handler,
		.experimental = false,
	};
	నిర్మాణం sbi_ecall_extension e3 = {
		.extid_ప్రారంభం = SBI_EXT_EXPERIMENTAL_ప్రారంభం + 2,
		.extid_చివర = SBI_EXT_EXPERIMENTAL_ప్రారంభం + 2,
		.name = "Charli",
		.handle = dummy_handler,
		.experimental = false,
	};
	అక్షరం storage[16 + 16];
	అక్షరం *buf = storage;
	అక్షరం big[128];
	పూర్ణాంకం i;
	పూర్ణాంకం దొరికింది_alpha = 0;

	SBIUNIT_EXPECT_EQ(test, sbi_ecall_రిజిస్టరు_extension(&e1), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_రిజిస్టరు_extension(&e2), 0);
	SBIUNIT_EXPECT_EQ(test, sbi_ecall_రిజిస్టరు_extension(&e3), 0);

	కొరకు (i = 16; i < 32; i++)
		storage[i] = (అక్షరం)0xA5;

	/* Undersized buffer must not write past the caller-provided size. */
	sbi_ecall_get_extensions_str(buf, 16, false);
	SBIUNIT_EXPECT_EQ(test, buf[15], '\0');
	కొరకు (i = 16; i < 32; i++)
		SBIUNIT_EXPECT_EQ(test, (గుర్తులేని అక్షరం)storage[i], 0xA5);

	/* Negative control: room for the full list, including registered names. */
	sbi_ecall_get_extensions_str(big, పరిమాణం(big), false);
	SBIUNIT_EXPECT_NE(test, sbi_strlen(big), 0);
	కొరకు (i = 0; big[i] != '\0'; i++) {
		ఒకవేళ (sbi_strncmp(&big[i], "Alpha", 5) == 0) {
			దొరికింది_alpha = 1;
			ఆపు;
		}
	}
	SBIUNIT_EXPECT_EQ(test, దొరికింది_alpha, 1);

	sbi_ecall_unregister_extension(&e1);
	sbi_ecall_unregister_extension(&e2);
	sbi_ecall_unregister_extension(&e3);
}

స్థిర నిర్మాణం sbiunit_test_case ecall_tests[] = {
	SBIUNIT_TEST_CASE(test_sbi_ecall_సంస్కరణ),
	SBIUNIT_TEST_CASE(test_sbi_ecall_impid),
	SBIUNIT_TEST_CASE(test_sbi_ecall_రిజిస్టరు_కనుగొను_extension),
	SBIUNIT_TEST_CASE(test_sbi_ecall_get_extensions_str_bounds),
	SBIUNIT_చివర_CASE,
};

SBIUNIT_TEST_SUITE(ecall_test_suite, ecall_tests);
