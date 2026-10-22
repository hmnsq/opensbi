#अन्तर्भाव <sbi/sbi_unit_test.h>
#अन्तर्भाव <sbi/riscv_locks.h>

स्थिर spinlock_t test_तालकम् = SPIN_तालकम्_INITIALIZER;

स्थिर शून्य spin_तालकम्_test(संरचना sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_तालकम्_check(&test_तालकम्));

	spin_तालकम्(&test_तालकम्);
	SBIUNIT_EXPECT(test, spin_तालकम्_check(&test_तालकम्));
	spin_unlock(&test_तालकम्);

	SBIUNIT_ASSERT(test, !spin_तालकम्_check(&test_तालकम्));
}

स्थिर शून्य spin_trylock_fail(संरचना sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_तालकम्_check(&test_तालकम्));

	spin_तालकम्(&test_तालकम्);
	SBIUNIT_EXPECT(test, !spin_trylock(&test_तालकम्));
	spin_unlock(&test_तालकम्);
}

स्थिर शून्य spin_trylock_success(संरचना sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	SBIUNIT_EXPECT(test, spin_trylock(&test_तालकम्));
	spin_unlock(&test_तालकम्);
}

स्थिर संरचना sbiunit_test_case locks_test_cases[] = {
	SBIUNIT_TEST_CASE(spin_तालकम्_test),
	SBIUNIT_TEST_CASE(spin_trylock_fail),
	SBIUNIT_TEST_CASE(spin_trylock_success),
	SBIUNIT_अन्तः_CASE,
};

SBIUNIT_TEST_SUITE(locks_test_suite, locks_test_cases);
