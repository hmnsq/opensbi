#ಅನ್ತರ್ಭಾವ <sbi/sbi_unit_test.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>

ಸ್ಥಿರ spinlock_t test_ಬೀಗ = SPIN_ಬೀಗ_INITIALIZER;

ಸ್ಥಿರ ಶೂನ್ಯ spin_ಬೀಗ_test(ರಚನೆ sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_ಬೀಗ_check(&test_ಬೀಗ));

	spin_ಬೀಗ(&test_ಬೀಗ);
	SBIUNIT_EXPECT(test, spin_ಬೀಗ_check(&test_ಬೀಗ));
	spin_unlock(&test_ಬೀಗ);

	SBIUNIT_ASSERT(test, !spin_ಬೀಗ_check(&test_ಬೀಗ));
}

ಸ್ಥಿರ ಶೂನ್ಯ spin_trylock_fail(ರಚನೆ sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_ಬೀಗ_check(&test_ಬೀಗ));

	spin_ಬೀಗ(&test_ಬೀಗ);
	SBIUNIT_EXPECT(test, !spin_trylock(&test_ಬೀಗ));
	spin_unlock(&test_ಬೀಗ);
}

ಸ್ಥಿರ ಶೂನ್ಯ spin_trylock_success(ರಚನೆ sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	SBIUNIT_EXPECT(test, spin_trylock(&test_ಬೀಗ));
	spin_unlock(&test_ಬೀಗ);
}

ಸ್ಥಿರ ರಚನೆ sbiunit_test_case locks_test_cases[] = {
	SBIUNIT_TEST_CASE(spin_ಬೀಗ_test),
	SBIUNIT_TEST_CASE(spin_trylock_fail),
	SBIUNIT_TEST_CASE(spin_trylock_success),
	SBIUNIT_ಕೊನೆ_CASE,
};

SBIUNIT_TEST_SUITE(locks_test_suite, locks_test_cases);
