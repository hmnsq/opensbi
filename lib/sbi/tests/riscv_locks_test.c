#అన్తర్భావ <sbi/sbi_unit_test.h>
#అన్తర్భావ <sbi/riscv_locks.h>

స్థిర spinlock_t test_తాళం = SPIN_తాళం_INITIALIZER;

స్థిర శూన్య spin_తాళం_test(నిర్మాణం sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_తాళం_check(&test_తాళం));

	spin_తాళం(&test_తాళం);
	SBIUNIT_EXPECT(test, spin_తాళం_check(&test_తాళం));
	spin_unlock(&test_తాళం);

	SBIUNIT_ASSERT(test, !spin_తాళం_check(&test_తాళం));
}

స్థిర శూన్య spin_trylock_fail(నిర్మాణం sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	/* We don't want to accidentally get locked */
	SBIUNIT_ASSERT(test, !spin_తాళం_check(&test_తాళం));

	spin_తాళం(&test_తాళం);
	SBIUNIT_EXPECT(test, !spin_trylock(&test_తాళం));
	spin_unlock(&test_తాళం);
}

స్థిర శూన్య spin_trylock_success(నిర్మాణం sbiunit_test_case *test)
	NO_THREAD_SAFETY_ANALYSIS
{
	SBIUNIT_EXPECT(test, spin_trylock(&test_తాళం));
	spin_unlock(&test_తాళం);
}

స్థిర నిర్మాణం sbiunit_test_case locks_test_cases[] = {
	SBIUNIT_TEST_CASE(spin_తాళం_test),
	SBIUNIT_TEST_CASE(spin_trylock_fail),
	SBIUNIT_TEST_CASE(spin_trylock_success),
	SBIUNIT_చివర_CASE,
};

SBIUNIT_TEST_SUITE(locks_test_suite, locks_test_cases);
