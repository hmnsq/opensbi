#அந்தர்பாவ <sbi/sbi_unit_test.h>
#அந்தர்பாவ <sbi/riscv_locks.h>

நிலையான spinlock_t test_lock = SPIN_LOCK_INITIALIZER;

நிலையான வெறுமை spin_lock_test(கட்டமைப்பு sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_lock_check(&test_lock));

	spin_lock(&test_lock);
	SBIUNIT_EXPECT(test, spin_lock_check(&test_lock));
	spin_unlock(&test_lock);

	SBIUNIT_ASSERT(test, !spin_lock_check(&test_lock));
}

நிலையான வெறுமை spin_trylock_fail(கட்டமைப்பு sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_lock_check(&test_lock));

	spin_lock(&test_lock);
	SBIUNIT_EXPECT(test, !spin_trylock(&test_lock));
	spin_unlock(&test_lock);
}

நிலையான வெறுமை spin_trylock_success(கட்டமைப்பு sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	SBIUNIT_EXPECT(test, spin_trylock(&test_lock));
	spin_unlock(&test_lock);
}

நிலையான கட்டமைப்பு sbiunit_test_case locks_test_cases[] = {
	SBIUNIT_TEST_CASE(spin_lock_test),
	SBIUNIT_TEST_CASE(spin_trylock_fail),
	SBIUNIT_TEST_CASE(spin_trylock_success),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(locks_test_suite, locks_test_cases);
