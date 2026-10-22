#ضمن <sbi/sbi_unit_test.h>
#ضمن <sbi/riscv_locks.h>

ساكن spinlock_t test_تالو = SPIN_تالو_INITIALIZER;

ساكن فراغ spin_تالو_test(بنية sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_تالو_check(&test_تالو));

	spin_تالو(&test_تالو);
	SBIUNIT_EXPECT(test, spin_تالو_check(&test_تالو));
	spin_unlock(&test_تالو);

	SBIUNIT_ASSERT(test, !spin_تالو_check(&test_تالو));
}

ساكن فراغ spin_trylock_fail(بنية sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_تالو_check(&test_تالو));

	spin_تالو(&test_تالو);
	SBIUNIT_EXPECT(test, !spin_trylock(&test_تالو));
	spin_unlock(&test_تالو);
}

ساكن فراغ spin_trylock_success(بنية sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	SBIUNIT_EXPECT(test, spin_trylock(&test_تالو));
	spin_unlock(&test_تالو);
}

ساكن بنية sbiunit_test_case locks_test_cases[] = {
	SBIUNIT_TEST_CASE(spin_تالو_test),
	SBIUNIT_TEST_CASE(spin_trylock_fail),
	SBIUNIT_TEST_CASE(spin_trylock_success),
	SBIUNIT_پڇاڙي_CASE,
};

SBIUNIT_TEST_SUITE(locks_test_suite, locks_test_cases);
