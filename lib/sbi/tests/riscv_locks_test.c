#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unit_test.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_locks.h>

ਸਥਿਰ spinlock_t test_ਤਾਲਾ = SPIN_ਤਾਲਾ_INITIALIZER;

ਸਥਿਰ ਖਾਲੀ spin_ਤਾਲਾ_test(ਬਣਤਰ sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_ਤਾਲਾ_check(&test_ਤਾਲਾ));

	spin_ਤਾਲਾ(&test_ਤਾਲਾ);
	SBIUNIT_EXPECT(test, spin_ਤਾਲਾ_check(&test_ਤਾਲਾ));
	spin_unlock(&test_ਤਾਲਾ);

	SBIUNIT_ASSERT(test, !spin_ਤਾਲਾ_check(&test_ਤਾਲਾ));
}

ਸਥਿਰ ਖਾਲੀ spin_trylock_fail(ਬਣਤਰ sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_ਤਾਲਾ_check(&test_ਤਾਲਾ));

	spin_ਤਾਲਾ(&test_ਤਾਲਾ);
	SBIUNIT_EXPECT(test, !spin_trylock(&test_ਤਾਲਾ));
	spin_unlock(&test_ਤਾਲਾ);
}

ਸਥਿਰ ਖਾਲੀ spin_trylock_success(ਬਣਤਰ sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	SBIUNIT_EXPECT(test, spin_trylock(&test_ਤਾਲਾ));
	spin_unlock(&test_ਤਾਲਾ);
}

ਸਥਿਰ ਬਣਤਰ sbiunit_test_case locks_test_cases[] = {
	SBIUNIT_TEST_CASE(spin_ਤਾਲਾ_test),
	SBIUNIT_TEST_CASE(spin_trylock_fail),
	SBIUNIT_TEST_CASE(spin_trylock_success),
	SBIUNIT_ਸਿਰਾ_CASE,
};

SBIUNIT_TEST_SUITE(locks_test_suite, locks_test_cases);
