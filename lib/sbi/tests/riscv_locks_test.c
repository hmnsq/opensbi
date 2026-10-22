#अन्तर्भाव <sbi/sbi_unit_test.h>
#अन्तर्भाव <sbi/riscv_locks.h>

स्थिर spinlock_t test_lock = SPIN_LOCK_INITIALIZER;

स्थिर शून्य spin_lock_test(संरचना sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_lock_check(&test_lock));

	spin_lock(&test_lock);
	SBIUNIT_EXPECT(test, spin_lock_check(&test_lock));
	spin_unlock(&test_lock);

	SBIUNIT_ASSERT(test, !spin_lock_check(&test_lock));
}

स्थिर शून्य spin_trylock_fail(संरचना sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_lock_check(&test_lock));

	spin_lock(&test_lock);
	SBIUNIT_EXPECT(test, !spin_trylock(&test_lock));
	spin_unlock(&test_lock);
}

स्थिर शून्य spin_trylock_success(संरचना sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	SBIUNIT_EXPECT(test, spin_trylock(&test_lock));
	spin_unlock(&test_lock);
}

स्थिर संरचना sbiunit_test_case locks_test_cases[] = {
	SBIUNIT_TEST_CASE(spin_lock_test),
	SBIUNIT_TEST_CASE(spin_trylock_fail),
	SBIUNIT_TEST_CASE(spin_trylock_success),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(locks_test_suite, locks_test_cases);
