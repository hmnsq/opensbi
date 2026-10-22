#ଅନ୍ତର୍ଭାଵ <sbi/sbi_unit_test.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_locks.h>

ସ୍ଥିର spinlock_t test_lock = SPIN_LOCK_INITIALIZER;

ସ୍ଥିର ଶୂନ୍ୟ spin_lock_test(ସଂରଚନା sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_lock_check(&test_lock));

	spin_lock(&test_lock);
	SBIUNIT_EXPECT(test, spin_lock_check(&test_lock));
	spin_unlock(&test_lock);

	SBIUNIT_ASSERT(test, !spin_lock_check(&test_lock));
}

ସ୍ଥିର ଶୂନ୍ୟ spin_trylock_fail(ସଂରଚନା sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_lock_check(&test_lock));

	spin_lock(&test_lock);
	SBIUNIT_EXPECT(test, !spin_trylock(&test_lock));
	spin_unlock(&test_lock);
}

ସ୍ଥିର ଶୂନ୍ୟ spin_trylock_success(ସଂରଚନା sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	SBIUNIT_EXPECT(test, spin_trylock(&test_lock));
	spin_unlock(&test_lock);
}

ସ୍ଥିର ସଂରଚନା sbiunit_test_case locks_test_cases[] = {
	SBIUNIT_TEST_CASE(spin_lock_test),
	SBIUNIT_TEST_CASE(spin_trylock_fail),
	SBIUNIT_TEST_CASE(spin_trylock_success),
	SBIUNIT_END_CASE,
};

SBIUNIT_TEST_SUITE(locks_test_suite, locks_test_cases);
