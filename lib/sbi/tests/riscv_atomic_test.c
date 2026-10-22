#అన్తర్భావ <sbi/sbi_unit_test.h>
#అన్తర్భావ <sbi/riscv_atomic.h>
#అన్తర్భావ <sbi/sbi_bitops.h>

#నిర్వచించు ATOMIC_TEST_VAL1 239l
#నిర్వచించు ATOMIC_TEST_VAL2 30l
#నిర్వచించు ATOMIC_TEST_VAL3 2024l

#నిర్వచించు ATOMIC_TEST_బిట్టు_అంకె 3

#నిర్వచించు ATOMIC_TEST_RAW_బిట్టు_CELL 1
#నిర్వచించు ATOMIC_TEST_RAW_బిట్టు_అంకె 15

స్థిర atomic_t test_atomic;

స్థిర శూన్య atomic_test_suite_init(శూన్య)
{
	ATOMIC_INIT(&test_atomic, 0);
}

స్థిర శూన్య atomic_rw_test(నిర్మాణం sbiunit_test_case *test)
{
	/* We should read the same value as we've written */
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1);
	/* Negative value should also work */
	atomic_write(&test_atomic, -ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), -ATOMIC_TEST_VAL1);
}

స్థిర శూన్య కూడు_return_test(నిర్మాణం sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_కూడు_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
	/* The atomic value should be updated as well */
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
}

స్థిర శూన్య sub_return_test(నిర్మాణం sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_sub_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
}

స్థిర శూన్య cmpxchg_test(నిర్మాణం sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	/* if current value != expected, it stays the same */
	SBIUNIT_EXPECT_EQ(test, atomic_cmpxchg(&test_atomic, ATOMIC_TEST_VAL2, ATOMIC_TEST_VAL3),
			  ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1);
	/* if current value == expected, it gets updated */
	SBIUNIT_EXPECT_EQ(test, atomic_cmpxchg(&test_atomic, ATOMIC_TEST_VAL1, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL2);
}

స్థిర శూన్య atomic_xchg_test(నిర్మాణం sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_xchg(&test_atomic, ATOMIC_TEST_VAL2), ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL2);
}

స్థిర శూన్య atomic_raw_సమితి_బిట్టు_test(నిర్మాణం sbiunit_test_case *test)
{
	గుర్తులేని దీర్ఘ దత్తాంశం[] = {0, 0, 0};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_బిట్టు_CELL * బిట్లు_చొప్పున_LONG + ATOMIC_TEST_RAW_బిట్టు_అంకె;

	/* check if the bit we set actually gets set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_సమితి_బిట్టు(bitpos, దత్తాంశం), 0);
	SBIUNIT_EXPECT_EQ(test, దత్తాంశం[ATOMIC_TEST_RAW_బిట్టు_CELL], 1 << ATOMIC_TEST_RAW_బిట్టు_అంకె);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, దత్తాంశం[0], 0);
	SBIUNIT_EXPECT_EQ(test, దత్తాంశం[2], 0);

	/* check that if we set the bit twice it stays set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_సమితి_బిట్టు(bitpos, దత్తాంశం), 1);
	SBIUNIT_EXPECT_EQ(test, దత్తాంశం[ATOMIC_TEST_RAW_బిట్టు_CELL], 1 << ATOMIC_TEST_RAW_బిట్టు_అంకె);
}

స్థిర శూన్య atomic_raw_clear_బిట్టు_test(నిర్మాణం sbiunit_test_case *test)
{
	గుర్తులేని దీర్ఘ దత్తాంశం[] = {~1UL, 1 << ATOMIC_TEST_RAW_బిట్టు_అంకె, ~1UL};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_బిట్టు_CELL * బిట్లు_చొప్పున_LONG + ATOMIC_TEST_RAW_బిట్టు_అంకె;

	/* check if the bit we clear actually gets cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_బిట్టు(bitpos, దత్తాంశం), 1);
	SBIUNIT_EXPECT_EQ(test, దత్తాంశం[ATOMIC_TEST_RAW_బిట్టు_CELL], 0);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, దత్తాంశం[0], ~1UL);
	SBIUNIT_EXPECT_EQ(test, దత్తాంశం[2], ~1UL);

	/* check that if we clear the bit twice it stays cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_బిట్టు(bitpos, దత్తాంశం), 0);
	SBIUNIT_EXPECT_EQ(test, దత్తాంశం[ATOMIC_TEST_RAW_బిట్టు_CELL], 0);
}

స్థిర శూన్య atomic_సమితి_బిట్టు_test(నిర్మాణం sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 0);
	SBIUNIT_EXPECT_EQ(test, atomic_సమితి_బిట్టు(ATOMIC_TEST_బిట్టు_అంకె, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_బిట్టు_అంకె);
	/* If we set the bit twice, it stays 1 */
	SBIUNIT_EXPECT_EQ(test, atomic_సమితి_బిట్టు(ATOMIC_TEST_బిట్టు_అంకె, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_బిట్టు_అంకె);
}

స్థిర శూన్య atomic_clear_బిట్టు_test(నిర్మాణం sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 1 << ATOMIC_TEST_బిట్టు_అంకె);
	SBIUNIT_EXPECT_EQ(test, atomic_clear_బిట్టు(ATOMIC_TEST_బిట్టు_అంకె, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
	/* if we clear the bit twice, it stays 0 */
	SBIUNIT_EXPECT_EQ(test, atomic_clear_బిట్టు(ATOMIC_TEST_బిట్టు_అంకె, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
}

స్థిర నిర్మాణం sbiunit_test_case atomic_test_cases[] = {
	SBIUNIT_TEST_CASE(atomic_rw_test),
	SBIUNIT_TEST_CASE(కూడు_return_test),
	SBIUNIT_TEST_CASE(sub_return_test),
	SBIUNIT_TEST_CASE(cmpxchg_test),
	SBIUNIT_TEST_CASE(atomic_xchg_test),
	SBIUNIT_TEST_CASE(atomic_raw_సమితి_బిట్టు_test),
	SBIUNIT_TEST_CASE(atomic_raw_clear_బిట్టు_test),
	SBIUNIT_TEST_CASE(atomic_సమితి_బిట్టు_test),
	SBIUNIT_TEST_CASE(atomic_clear_బిట్టు_test),
	SBIUNIT_చివర_CASE,
};

స్థిరరాశి నిర్మాణం sbiunit_test_suite atomic_test_suite = {
	.name = "atomic_test_suite",
	.cases = atomic_test_cases,
	.init = atomic_test_suite_init
};
