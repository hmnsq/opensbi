#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unit_test.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_atomic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATOMIC_TEST_VAL1 239l
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATOMIC_TEST_VAL2 30l
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATOMIC_TEST_VAL3 2024l

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATOMIC_TEST_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ 3

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_CELL 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ 15

ਸਥਿਰ atomic_t test_atomic;

ਸਥਿਰ ਖਾਲੀ atomic_test_suite_init(ਖਾਲੀ)
{
	ATOMIC_INIT(&test_atomic, 0);
}

ਸਥਿਰ ਖਾਲੀ atomic_rw_test(ਬਣਤਰ sbiunit_test_case *test)
{
	/* We should read the same value as we've written */
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1);
	/* Negative value should also work */
	atomic_write(&test_atomic, -ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), -ATOMIC_TEST_VAL1);
}

ਸਥਿਰ ਖਾਲੀ ਜੋੜੋ_return_test(ਬਣਤਰ sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_ਜੋੜੋ_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
	/* The atomic value should be updated as well */
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
}

ਸਥਿਰ ਖਾਲੀ sub_return_test(ਬਣਤਰ sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_sub_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
}

ਸਥਿਰ ਖਾਲੀ cmpxchg_test(ਬਣਤਰ sbiunit_test_case *test)
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

ਸਥਿਰ ਖਾਲੀ atomic_xchg_test(ਬਣਤਰ sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_xchg(&test_atomic, ATOMIC_TEST_VAL2), ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL2);
}

ਸਥਿਰ ਖਾਲੀ atomic_raw_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_test(ਬਣਤਰ sbiunit_test_case *test)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅੰਕੜੇ[] = {0, 0, 0};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_CELL * ਬਿੱਟ_ਪ੍ਰਤੀ_LONG + ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ;

	/* check if the bit we set actually gets set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(bitpos, ਅੰਕੜੇ), 0);
	SBIUNIT_EXPECT_EQ(test, ਅੰਕੜੇ[ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_CELL], 1 << ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, ਅੰਕੜੇ[0], 0);
	SBIUNIT_EXPECT_EQ(test, ਅੰਕੜੇ[2], 0);

	/* check that if we set the bit twice it stays set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(bitpos, ਅੰਕੜੇ), 1);
	SBIUNIT_EXPECT_EQ(test, ਅੰਕੜੇ[ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_CELL], 1 << ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ);
}

ਸਥਿਰ ਖਾਲੀ atomic_raw_clear_ਦੋਆਧਾਰੀ_ਅੰਕ_test(ਬਣਤਰ sbiunit_test_case *test)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅੰਕੜੇ[] = {~1UL, 1 << ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ, ~1UL};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_CELL * ਬਿੱਟ_ਪ੍ਰਤੀ_LONG + ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ;

	/* check if the bit we clear actually gets cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_ਦੋਆਧਾਰੀ_ਅੰਕ(bitpos, ਅੰਕੜੇ), 1);
	SBIUNIT_EXPECT_EQ(test, ਅੰਕੜੇ[ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_CELL], 0);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, ਅੰਕੜੇ[0], ~1UL);
	SBIUNIT_EXPECT_EQ(test, ਅੰਕੜੇ[2], ~1UL);

	/* check that if we clear the bit twice it stays cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_ਦੋਆਧਾਰੀ_ਅੰਕ(bitpos, ਅੰਕੜੇ), 0);
	SBIUNIT_EXPECT_EQ(test, ਅੰਕੜੇ[ATOMIC_TEST_RAW_ਦੋਆਧਾਰੀ_ਅੰਕ_CELL], 0);
}

ਸਥਿਰ ਖਾਲੀ atomic_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_test(ਬਣਤਰ sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 0);
	SBIUNIT_EXPECT_EQ(test, atomic_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ATOMIC_TEST_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ);
	/* If we set the bit twice, it stays 1 */
	SBIUNIT_EXPECT_EQ(test, atomic_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ATOMIC_TEST_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ);
}

ਸਥਿਰ ਖਾਲੀ atomic_clear_ਦੋਆਧਾਰੀ_ਅੰਕ_test(ਬਣਤਰ sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 1 << ATOMIC_TEST_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ);
	SBIUNIT_EXPECT_EQ(test, atomic_clear_ਦੋਆਧਾਰੀ_ਅੰਕ(ATOMIC_TEST_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
	/* if we clear the bit twice, it stays 0 */
	SBIUNIT_EXPECT_EQ(test, atomic_clear_ਦੋਆਧਾਰੀ_ਅੰਕ(ATOMIC_TEST_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅੰਕ, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
}

ਸਥਿਰ ਬਣਤਰ sbiunit_test_case atomic_test_cases[] = {
	SBIUNIT_TEST_CASE(atomic_rw_test),
	SBIUNIT_TEST_CASE(ਜੋੜੋ_return_test),
	SBIUNIT_TEST_CASE(sub_return_test),
	SBIUNIT_TEST_CASE(cmpxchg_test),
	SBIUNIT_TEST_CASE(atomic_xchg_test),
	SBIUNIT_TEST_CASE(atomic_raw_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_test),
	SBIUNIT_TEST_CASE(atomic_raw_clear_ਦੋਆਧਾਰੀ_ਅੰਕ_test),
	SBIUNIT_TEST_CASE(atomic_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_test),
	SBIUNIT_TEST_CASE(atomic_clear_ਦੋਆਧਾਰੀ_ਅੰਕ_test),
	SBIUNIT_ਸਿਰਾ_CASE,
};

ਅਚਲ ਬਣਤਰ sbiunit_test_suite atomic_test_suite = {
	.name = "atomic_test_suite",
	.cases = atomic_test_cases,
	.init = atomic_test_suite_init
};
