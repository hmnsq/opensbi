#समावेश <sbi/sbi_unit_test.h>
#समावेश <sbi/riscv_atomic.h>
#समावेश <sbi/sbi_bitops.h>

#घोषणा ATOMIC_TEST_VAL1 239l
#घोषणा ATOMIC_TEST_VAL2 30l
#घोषणा ATOMIC_TEST_VAL3 2024l

#घोषणा ATOMIC_TEST_BIT_अंक 3

#घोषणा ATOMIC_TEST_RAW_BIT_CELL 1
#घोषणा ATOMIC_TEST_RAW_BIT_अंक 15

अटल atomic_t test_atomic;

अटल व्योम atomic_test_suite_init(व्योम)
{
	ATOMIC_INIT(&test_atomic, 0);
}

अटल व्योम atomic_rw_test(काष्ठा sbiunit_test_case *test)
{
	/* We should read the same value as we've written */
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1);
	/* Negative value should also work */
	atomic_write(&test_atomic, -ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), -ATOMIC_TEST_VAL1);
}

अटल व्योम add_return_test(काष्ठा sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_add_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
	/* The atomic value should be updated as well */
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
}

अटल व्योम sub_return_test(काष्ठा sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_sub_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
}

अटल व्योम cmpxchg_test(काष्ठा sbiunit_test_case *test)
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

अटल व्योम atomic_xchg_test(काष्ठा sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_xchg(&test_atomic, ATOMIC_TEST_VAL2), ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL2);
}

अटल व्योम atomic_raw_set_bit_test(काष्ठा sbiunit_test_case *test)
{
	अचिन्हित दीर्घ तथ्य[] = {0, 0, 0};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	माप_प्रकार bitpos = ATOMIC_TEST_RAW_BIT_CELL * BITS_PER_LONG + ATOMIC_TEST_RAW_BIT_अंक;

	/* check if the bit we set actually gets set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_set_bit(bitpos, तथ्य), 0);
	SBIUNIT_EXPECT_EQ(test, तथ्य[ATOMIC_TEST_RAW_BIT_CELL], 1 << ATOMIC_TEST_RAW_BIT_अंक);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, तथ्य[0], 0);
	SBIUNIT_EXPECT_EQ(test, तथ्य[2], 0);

	/* check that if we set the bit twice it stays set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_set_bit(bitpos, तथ्य), 1);
	SBIUNIT_EXPECT_EQ(test, तथ्य[ATOMIC_TEST_RAW_BIT_CELL], 1 << ATOMIC_TEST_RAW_BIT_अंक);
}

अटल व्योम atomic_raw_clear_bit_test(काष्ठा sbiunit_test_case *test)
{
	अचिन्हित दीर्घ तथ्य[] = {~1UL, 1 << ATOMIC_TEST_RAW_BIT_अंक, ~1UL};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	माप_प्रकार bitpos = ATOMIC_TEST_RAW_BIT_CELL * BITS_PER_LONG + ATOMIC_TEST_RAW_BIT_अंक;

	/* check if the bit we clear actually gets cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_bit(bitpos, तथ्य), 1);
	SBIUNIT_EXPECT_EQ(test, तथ्य[ATOMIC_TEST_RAW_BIT_CELL], 0);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, तथ्य[0], ~1UL);
	SBIUNIT_EXPECT_EQ(test, तथ्य[2], ~1UL);

	/* check that if we clear the bit twice it stays cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_bit(bitpos, तथ्य), 0);
	SBIUNIT_EXPECT_EQ(test, तथ्य[ATOMIC_TEST_RAW_BIT_CELL], 0);
}

अटल व्योम atomic_set_bit_test(काष्ठा sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 0);
	SBIUNIT_EXPECT_EQ(test, atomic_set_bit(ATOMIC_TEST_BIT_अंक, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_BIT_अंक);
	/* If we set the bit twice, it stays 1 */
	SBIUNIT_EXPECT_EQ(test, atomic_set_bit(ATOMIC_TEST_BIT_अंक, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_BIT_अंक);
}

अटल व्योम atomic_clear_bit_test(काष्ठा sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 1 << ATOMIC_TEST_BIT_अंक);
	SBIUNIT_EXPECT_EQ(test, atomic_clear_bit(ATOMIC_TEST_BIT_अंक, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
	/* if we clear the bit twice, it stays 0 */
	SBIUNIT_EXPECT_EQ(test, atomic_clear_bit(ATOMIC_TEST_BIT_अंक, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
}

अटल काष्ठा sbiunit_test_case atomic_test_cases[] = {
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

स्थिर काष्ठा sbiunit_test_suite atomic_test_suite = {
	.name = "atomic_test_suite",
	.cases = atomic_test_cases,
	.init = atomic_test_suite_init
};
