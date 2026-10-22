#अन्तर्भाव <sbi/sbi_unit_test.h>
#अन्तर्भाव <sbi/riscv_atomic.h>
#अन्तर्भाव <sbi/sbi_bitops.h>

#परिभाषय ATOMIC_TEST_VAL1 239l
#परिभाषय ATOMIC_TEST_VAL2 30l
#परिभाषय ATOMIC_TEST_VAL3 2024l

#परिभाषय ATOMIC_TEST_BIT_अङ्कः 3

#परिभाषय ATOMIC_TEST_RAW_BIT_CELL 1
#परिभाषय ATOMIC_TEST_RAW_BIT_अङ्कः 15

स्थिर atomic_t test_atomic;

स्थिर शून्य atomic_test_suite_init(शून्य)
{
	ATOMIC_INIT(&test_atomic, 0);
}

स्थिर शून्य atomic_rw_test(संरचना sbiunit_test_case *test)
{
	/* We should read the same value as we've written */
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1);
	/* Negative value should also work */
	atomic_write(&test_atomic, -ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), -ATOMIC_TEST_VAL1);
}

स्थिर शून्य सङ्कलय_return_test(संरचना sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_सङ्कलय_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
	/* The atomic value should be updated as well */
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
}

स्थिर शून्य sub_return_test(संरचना sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_sub_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
}

स्थिर शून्य cmpxchg_test(संरचना sbiunit_test_case *test)
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

स्थिर शून्य atomic_xchg_test(संरचना sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_xchg(&test_atomic, ATOMIC_TEST_VAL2), ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL2);
}

स्थिर शून्य atomic_raw_समुच्चयः_bit_test(संरचना sbiunit_test_case *test)
{
	अचिह्नित दीर्घ दत्त[] = {0, 0, 0};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_BIT_CELL * द्वयङ्काः_प्रत्येकम्_LONG + ATOMIC_TEST_RAW_BIT_अङ्कः;

	/* check if the bit we set actually gets set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_समुच्चयः_bit(bitpos, दत्त), 0);
	SBIUNIT_EXPECT_EQ(test, दत्त[ATOMIC_TEST_RAW_BIT_CELL], 1 << ATOMIC_TEST_RAW_BIT_अङ्कः);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, दत्त[0], 0);
	SBIUNIT_EXPECT_EQ(test, दत्त[2], 0);

	/* check that if we set the bit twice it stays set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_समुच्चयः_bit(bitpos, दत्त), 1);
	SBIUNIT_EXPECT_EQ(test, दत्त[ATOMIC_TEST_RAW_BIT_CELL], 1 << ATOMIC_TEST_RAW_BIT_अङ्कः);
}

स्थिर शून्य atomic_raw_clear_bit_test(संरचना sbiunit_test_case *test)
{
	अचिह्नित दीर्घ दत्त[] = {~1UL, 1 << ATOMIC_TEST_RAW_BIT_अङ्कः, ~1UL};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_BIT_CELL * द्वयङ्काः_प्रत्येकम्_LONG + ATOMIC_TEST_RAW_BIT_अङ्कः;

	/* check if the bit we clear actually gets cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_bit(bitpos, दत्त), 1);
	SBIUNIT_EXPECT_EQ(test, दत्त[ATOMIC_TEST_RAW_BIT_CELL], 0);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, दत्त[0], ~1UL);
	SBIUNIT_EXPECT_EQ(test, दत्त[2], ~1UL);

	/* check that if we clear the bit twice it stays cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_bit(bitpos, दत्त), 0);
	SBIUNIT_EXPECT_EQ(test, दत्त[ATOMIC_TEST_RAW_BIT_CELL], 0);
}

स्थिर शून्य atomic_समुच्चयः_bit_test(संरचना sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 0);
	SBIUNIT_EXPECT_EQ(test, atomic_समुच्चयः_bit(ATOMIC_TEST_BIT_अङ्कः, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_BIT_अङ्कः);
	/* If we set the bit twice, it stays 1 */
	SBIUNIT_EXPECT_EQ(test, atomic_समुच्चयः_bit(ATOMIC_TEST_BIT_अङ्कः, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_BIT_अङ्कः);
}

स्थिर शून्य atomic_clear_bit_test(संरचना sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 1 << ATOMIC_TEST_BIT_अङ्कः);
	SBIUNIT_EXPECT_EQ(test, atomic_clear_bit(ATOMIC_TEST_BIT_अङ्कः, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
	/* if we clear the bit twice, it stays 0 */
	SBIUNIT_EXPECT_EQ(test, atomic_clear_bit(ATOMIC_TEST_BIT_अङ्कः, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
}

स्थिर संरचना sbiunit_test_case atomic_test_cases[] = {
	SBIUNIT_TEST_CASE(atomic_rw_test),
	SBIUNIT_TEST_CASE(सङ्कलय_return_test),
	SBIUNIT_TEST_CASE(sub_return_test),
	SBIUNIT_TEST_CASE(cmpxchg_test),
	SBIUNIT_TEST_CASE(atomic_xchg_test),
	SBIUNIT_TEST_CASE(atomic_raw_समुच्चयः_bit_test),
	SBIUNIT_TEST_CASE(atomic_raw_clear_bit_test),
	SBIUNIT_TEST_CASE(atomic_समुच्चयः_bit_test),
	SBIUNIT_TEST_CASE(atomic_clear_bit_test),
	SBIUNIT_अन्तः_CASE,
};

ध्रुव संरचना sbiunit_test_suite atomic_test_suite = {
	.name = "atomic_test_suite",
	.cases = atomic_test_cases,
	.init = atomic_test_suite_init
};
