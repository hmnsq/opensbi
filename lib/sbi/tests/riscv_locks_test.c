#ضمن <sbi/sbi_unit_test.h>
#ضمن <sbi/riscv_locks.h>

ساكن spinlock_t test_قلف = SPIN_قلف_INITIALIZER;

ساكن فراغ spin_قلف_test(بنية sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_قلف_check(&test_قلف));

	spin_قلف(&test_قلف);
	SBIUNIT_EXPECT(test, spin_قلف_check(&test_قلف));
	spin_unlock(&test_قلف);

	SBIUNIT_ASSERT(test, !spin_قلف_check(&test_قلف));
}

ساكن فراغ spin_trylock_fail(بنية sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_قلف_check(&test_قلف));

	spin_قلف(&test_قلف);
	SBIUNIT_EXPECT(test, !spin_trylock(&test_قلف));
	spin_unlock(&test_قلف);
}

ساكن فراغ spin_trylock_success(بنية sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	SBIUNIT_EXPECT(test, spin_trylock(&test_قلف));
	spin_unlock(&test_قلف);
}

ساكن بنية sbiunit_test_case locks_test_cases[] = {
	SBIUNIT_TEST_CASE(spin_قلف_test),
	SBIUNIT_TEST_CASE(spin_trylock_fail),
	SBIUNIT_TEST_CASE(spin_trylock_success),
	SBIUNIT_پای_ټکی_CASE,
};

SBIUNIT_TEST_SUITE(locks_test_suite, locks_test_cases);
