#شامل <sbi/sbi_unit_test.h>
#شامل <sbi/riscv_locks.h>

ساکن spinlock_t test_تالا = SPIN_تالا_INITIALIZER;

ساکن خالی spin_تالا_test(ساخت sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_تالا_check(&test_تالا));

	spin_تالا(&test_تالا);
	SBIUNIT_EXPECT(test, spin_تالا_check(&test_تالا));
	spin_unlock(&test_تالا);

	SBIUNIT_ASSERT(test, !spin_تالا_check(&test_تالا));
}

ساکن خالی spin_trylock_fail(ساخت sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_تالا_check(&test_تالا));

	spin_تالا(&test_تالا);
	SBIUNIT_EXPECT(test, !spin_trylock(&test_تالا));
	spin_unlock(&test_تالا);
}

ساکن خالی spin_trylock_success(ساخت sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	SBIUNIT_EXPECT(test, spin_trylock(&test_تالا));
	spin_unlock(&test_تالا);
}

ساکن ساخت sbiunit_test_case locks_test_cases[] = {
	SBIUNIT_TEST_CASE(spin_تالا_test),
	SBIUNIT_TEST_CASE(spin_trylock_fail),
	SBIUNIT_TEST_CASE(spin_trylock_success),
	SBIUNIT_انتہا_CASE,
};

SBIUNIT_TEST_SUITE(locks_test_suite, locks_test_cases);
