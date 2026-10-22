#അന്തര്ഭാവ <sbi/sbi_unit_test.h>
#അന്തര്ഭാവ <sbi/riscv_atomic.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>

#define ATOMIC_TEST_VAL1 239l
#define ATOMIC_TEST_VAL2 30l
#define ATOMIC_TEST_VAL3 2024l

#define ATOMIC_TEST_BIT_അക്കം 3

#define ATOMIC_TEST_RAW_BIT_CELL 1
#define ATOMIC_TEST_RAW_BIT_അക്കം 15

സ്ഥിതം atomic_t test_atomic;

സ്ഥിതം ശൂന്യം atomic_test_suite_init(ശൂന്യം)
{
	ATOMIC_INIT(&test_atomic, 0);
}

സ്ഥിതം ശൂന്യം atomic_rw_test(ഘടന sbiunit_test_case *test)
{
	/* We should read the same value as we've written */
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1);
	/* Negative value should also work */
	atomic_write(&test_atomic, -ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), -ATOMIC_TEST_VAL1);
}

സ്ഥിതം ശൂന്യം add_return_test(ഘടന sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_add_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
	/* The atomic value should be updated as well */
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
}

സ്ഥിതം ശൂന്യം sub_return_test(ഘടന sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_sub_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
}

സ്ഥിതം ശൂന്യം cmpxchg_test(ഘടന sbiunit_test_case *test)
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

സ്ഥിതം ശൂന്യം atomic_xchg_test(ഘടന sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_xchg(&test_atomic, ATOMIC_TEST_VAL2), ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL2);
}

സ്ഥിതം ശൂന്യം atomic_raw_set_bit_test(ഘടന sbiunit_test_case *test)
{
	unsigned long data[] = {0, 0, 0};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_BIT_CELL * BITS_PER_LONG + ATOMIC_TEST_RAW_BIT_അക്കം;

	/* check if the bit we set actually gets set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_set_bit(bitpos, data), 0);
	SBIUNIT_EXPECT_EQ(test, data[ATOMIC_TEST_RAW_BIT_CELL], 1 << ATOMIC_TEST_RAW_BIT_അക്കം);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, data[0], 0);
	SBIUNIT_EXPECT_EQ(test, data[2], 0);

	/* check that if we set the bit twice it stays set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_set_bit(bitpos, data), 1);
	SBIUNIT_EXPECT_EQ(test, data[ATOMIC_TEST_RAW_BIT_CELL], 1 << ATOMIC_TEST_RAW_BIT_അക്കം);
}

സ്ഥിതം ശൂന്യം atomic_raw_clear_bit_test(ഘടന sbiunit_test_case *test)
{
	unsigned long data[] = {~1UL, 1 << ATOMIC_TEST_RAW_BIT_അക്കം, ~1UL};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_BIT_CELL * BITS_PER_LONG + ATOMIC_TEST_RAW_BIT_അക്കം;

	/* check if the bit we clear actually gets cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_bit(bitpos, data), 1);
	SBIUNIT_EXPECT_EQ(test, data[ATOMIC_TEST_RAW_BIT_CELL], 0);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, data[0], ~1UL);
	SBIUNIT_EXPECT_EQ(test, data[2], ~1UL);

	/* check that if we clear the bit twice it stays cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_bit(bitpos, data), 0);
	SBIUNIT_EXPECT_EQ(test, data[ATOMIC_TEST_RAW_BIT_CELL], 0);
}

സ്ഥിതം ശൂന്യം atomic_set_bit_test(ഘടന sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 0);
	SBIUNIT_EXPECT_EQ(test, atomic_set_bit(ATOMIC_TEST_BIT_അക്കം, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_BIT_അക്കം);
	/* If we set the bit twice, it stays 1 */
	SBIUNIT_EXPECT_EQ(test, atomic_set_bit(ATOMIC_TEST_BIT_അക്കം, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_BIT_അക്കം);
}

സ്ഥിതം ശൂന്യം atomic_clear_bit_test(ഘടന sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 1 << ATOMIC_TEST_BIT_അക്കം);
	SBIUNIT_EXPECT_EQ(test, atomic_clear_bit(ATOMIC_TEST_BIT_അക്കം, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
	/* if we clear the bit twice, it stays 0 */
	SBIUNIT_EXPECT_EQ(test, atomic_clear_bit(ATOMIC_TEST_BIT_അക്കം, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
}

സ്ഥിതം ഘടന sbiunit_test_case atomic_test_cases[] = {
	SBIUNIT_TEST_CASE(atomic_rw_test),
	SBIUNIT_TEST_CASE(add_return_test),
	SBIUNIT_TEST_CASE(sub_return_test),
	SBIUNIT_TEST_CASE(cmpxchg_test),
	SBIUNIT_TEST_CASE(atomic_xchg_test),
	SBIUNIT_TEST_CASE(atomic_raw_set_bit_test),
	SBIUNIT_TEST_CASE(atomic_raw_clear_bit_test),
	SBIUNIT_TEST_CASE(atomic_set_bit_test),
	SBIUNIT_TEST_CASE(atomic_clear_bit_test),
	SBIUNIT_END_CASE,
};

സ്ഥിരം ഘടന sbiunit_test_suite atomic_test_suite = {
	.name = "atomic_test_suite",
	.cases = atomic_test_cases,
	.init = atomic_test_suite_init
};
