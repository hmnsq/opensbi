#ಅನ್ತರ್ಭಾವ <sbi/sbi_unit_test.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_atomic.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>

#ವ್ಯಾಖ್ಯಾನಿಸು ATOMIC_TEST_VAL1 239l
#ವ್ಯಾಖ್ಯಾನಿಸು ATOMIC_TEST_VAL2 30l
#ವ್ಯಾಖ್ಯಾನಿಸು ATOMIC_TEST_VAL3 2024l

#ವ್ಯಾಖ್ಯಾನಿಸು ATOMIC_TEST_ಬಿಟ್_ಅಂಕೆ 3

#ವ್ಯಾಖ್ಯಾನಿಸು ATOMIC_TEST_RAW_ಬಿಟ್_CELL 1
#ವ್ಯಾಖ್ಯಾನಿಸು ATOMIC_TEST_RAW_ಬಿಟ್_ಅಂಕೆ 15

ಸ್ಥಿರ atomic_t test_atomic;

ಸ್ಥಿರ ಶೂನ್ಯ atomic_test_suite_init(ಶೂನ್ಯ)
{
	ATOMIC_INIT(&test_atomic, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ atomic_rw_test(ರಚನೆ sbiunit_test_case *test)
{
	/* We should read the same value as we've written */
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1);
	/* Negative value should also work */
	atomic_write(&test_atomic, -ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), -ATOMIC_TEST_VAL1);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಕೂಡಿಸು_return_test(ರಚನೆ sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_ಕೂಡಿಸು_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
	/* The atomic value should be updated as well */
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
}

ಸ್ಥಿರ ಶೂನ್ಯ sub_return_test(ರಚನೆ sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_sub_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
}

ಸ್ಥಿರ ಶೂನ್ಯ cmpxchg_test(ರಚನೆ sbiunit_test_case *test)
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

ಸ್ಥಿರ ಶೂನ್ಯ atomic_xchg_test(ರಚನೆ sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_xchg(&test_atomic, ATOMIC_TEST_VAL2), ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL2);
}

ಸ್ಥಿರ ಶೂನ್ಯ atomic_raw_ಗಣ_ಬಿಟ್_test(ರಚನೆ sbiunit_test_case *test)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದತ್ತಾಂಶ[] = {0, 0, 0};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_ಬಿಟ್_CELL * ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG + ATOMIC_TEST_RAW_ಬಿಟ್_ಅಂಕೆ;

	/* check if the bit we set actually gets set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_ಗಣ_ಬಿಟ್(bitpos, ದತ್ತಾಂಶ), 0);
	SBIUNIT_EXPECT_EQ(test, ದತ್ತಾಂಶ[ATOMIC_TEST_RAW_ಬಿಟ್_CELL], 1 << ATOMIC_TEST_RAW_ಬಿಟ್_ಅಂಕೆ);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, ದತ್ತಾಂಶ[0], 0);
	SBIUNIT_EXPECT_EQ(test, ದತ್ತಾಂಶ[2], 0);

	/* check that if we set the bit twice it stays set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_ಗಣ_ಬಿಟ್(bitpos, ದತ್ತಾಂಶ), 1);
	SBIUNIT_EXPECT_EQ(test, ದತ್ತಾಂಶ[ATOMIC_TEST_RAW_ಬಿಟ್_CELL], 1 << ATOMIC_TEST_RAW_ಬಿಟ್_ಅಂಕೆ);
}

ಸ್ಥಿರ ಶೂನ್ಯ atomic_raw_clear_ಬಿಟ್_test(ರಚನೆ sbiunit_test_case *test)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದತ್ತಾಂಶ[] = {~1UL, 1 << ATOMIC_TEST_RAW_ಬಿಟ್_ಅಂಕೆ, ~1UL};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_ಬಿಟ್_CELL * ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG + ATOMIC_TEST_RAW_ಬಿಟ್_ಅಂಕೆ;

	/* check if the bit we clear actually gets cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_ಬಿಟ್(bitpos, ದತ್ತಾಂಶ), 1);
	SBIUNIT_EXPECT_EQ(test, ದತ್ತಾಂಶ[ATOMIC_TEST_RAW_ಬಿಟ್_CELL], 0);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, ದತ್ತಾಂಶ[0], ~1UL);
	SBIUNIT_EXPECT_EQ(test, ದತ್ತಾಂಶ[2], ~1UL);

	/* check that if we clear the bit twice it stays cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_ಬಿಟ್(bitpos, ದತ್ತಾಂಶ), 0);
	SBIUNIT_EXPECT_EQ(test, ದತ್ತಾಂಶ[ATOMIC_TEST_RAW_ಬಿಟ್_CELL], 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ atomic_ಗಣ_ಬಿಟ್_test(ರಚನೆ sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 0);
	SBIUNIT_EXPECT_EQ(test, atomic_ಗಣ_ಬಿಟ್(ATOMIC_TEST_ಬಿಟ್_ಅಂಕೆ, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_ಬಿಟ್_ಅಂಕೆ);
	/* If we set the bit twice, it stays 1 */
	SBIUNIT_EXPECT_EQ(test, atomic_ಗಣ_ಬಿಟ್(ATOMIC_TEST_ಬಿಟ್_ಅಂಕೆ, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_ಬಿಟ್_ಅಂಕೆ);
}

ಸ್ಥಿರ ಶೂನ್ಯ atomic_clear_ಬಿಟ್_test(ರಚನೆ sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 1 << ATOMIC_TEST_ಬಿಟ್_ಅಂಕೆ);
	SBIUNIT_EXPECT_EQ(test, atomic_clear_ಬಿಟ್(ATOMIC_TEST_ಬಿಟ್_ಅಂಕೆ, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
	/* if we clear the bit twice, it stays 0 */
	SBIUNIT_EXPECT_EQ(test, atomic_clear_ಬಿಟ್(ATOMIC_TEST_ಬಿಟ್_ಅಂಕೆ, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
}

ಸ್ಥಿರ ರಚನೆ sbiunit_test_case atomic_test_cases[] = {
	SBIUNIT_TEST_CASE(atomic_rw_test),
	SBIUNIT_TEST_CASE(ಕೂಡಿಸು_return_test),
	SBIUNIT_TEST_CASE(sub_return_test),
	SBIUNIT_TEST_CASE(cmpxchg_test),
	SBIUNIT_TEST_CASE(atomic_xchg_test),
	SBIUNIT_TEST_CASE(atomic_raw_ಗಣ_ಬಿಟ್_test),
	SBIUNIT_TEST_CASE(atomic_raw_clear_ಬಿಟ್_test),
	SBIUNIT_TEST_CASE(atomic_ಗಣ_ಬಿಟ್_test),
	SBIUNIT_TEST_CASE(atomic_clear_ಬಿಟ್_test),
	SBIUNIT_ಕೊನೆ_CASE,
};

ಸ್ಥಿರಾಂಕ ರಚನೆ sbiunit_test_suite atomic_test_suite = {
	.name = "atomic_test_suite",
	.cases = atomic_test_cases,
	.init = atomic_test_suite_init
};
