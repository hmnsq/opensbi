#ضمن <sbi/sbi_unit_test.h>
#ضمن <sbi/riscv_locks.h>

ساكن spinlock_t test_جندرا = SPIN_جندرا_INITIALIZER;

ساكن فراغ spin_جندرا_test(بنية sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_جندرا_check(&test_جندرا));

	spin_جندرا(&test_جندرا);
	SBIUNIT_EXPECT(test, spin_جندرا_check(&test_جندرا));
	spin_unlock(&test_جندرا);

	SBIUNIT_ASSERT(test, !spin_جندرا_check(&test_جندرا));
}

ساكن فراغ spin_trylock_fail(بنية sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_جندرا_check(&test_جندرا));

	spin_جندرا(&test_جندرا);
	SBIUNIT_EXPECT(test, !spin_trylock(&test_جندرا));
	spin_unlock(&test_جندرا);
}

ساكن فراغ spin_trylock_success(بنية sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	SBIUNIT_EXPECT(test, spin_trylock(&test_جندرا));
	spin_unlock(&test_جندرا);
}

ساكن بنية sbiunit_test_case locks_test_cases[] = {
	SBIUNIT_TEST_CASE(spin_جندرا_test),
	SBIUNIT_TEST_CASE(spin_trylock_fail),
	SBIUNIT_TEST_CASE(spin_trylock_success),
	SBIUNIT_سرا_CASE,
};

SBIUNIT_TEST_SUITE(locks_test_suite, locks_test_cases);
