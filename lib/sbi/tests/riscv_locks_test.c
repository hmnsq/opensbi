#ضمن <sbi/sbi_unit_test.h>
#ضمن <sbi/riscv_locks.h>

ساكن spinlock_t test_قفل = SPIN_قفل_INITIALIZER;

ساكن فراغ spin_قفل_test(بنية sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_قفل_check(&test_قفل));

	spin_قفل(&test_قفل);
	SBIUNIT_EXPECT(test, spin_قفل_check(&test_قفل));
	spin_unlock(&test_قفل);

	SBIUNIT_ASSERT(test, !spin_قفل_check(&test_قفل));
}

ساكن فراغ spin_trylock_fail(بنية sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_قفل_check(&test_قفل));

	spin_قفل(&test_قفل);
	SBIUNIT_EXPECT(test, !spin_trylock(&test_قفل));
	spin_unlock(&test_قفل);
}

ساكن فراغ spin_trylock_success(بنية sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	SBIUNIT_EXPECT(test, spin_trylock(&test_قفل));
	spin_unlock(&test_قفل);
}

ساكن بنية sbiunit_test_case locks_test_cases[] = {
	SBIUNIT_TEST_CASE(spin_قفل_test),
	SBIUNIT_TEST_CASE(spin_trylock_fail),
	SBIUNIT_TEST_CASE(spin_trylock_success),
	SBIUNIT_انتها_CASE,
};

SBIUNIT_TEST_SUITE(locks_test_suite, locks_test_cases);
