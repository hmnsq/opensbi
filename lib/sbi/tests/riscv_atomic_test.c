#ضمن <sbi/sbi_unit_test.h>
#ضمن <sbi/riscv_atomic.h>
#ضمن <sbi/sbi_bitops.h>

#تعریف_کړه ATOMIC_TEST_VAL1 239l
#تعریف_کړه ATOMIC_TEST_VAL2 30l
#تعریف_کړه ATOMIC_TEST_VAL3 2024l

#تعریف_کړه ATOMIC_TEST_بټ_عدد 3

#تعریف_کړه ATOMIC_TEST_RAW_بټ_CELL 1
#تعریف_کړه ATOMIC_TEST_RAW_بټ_عدد 15

ساكن atomic_t test_atomic;

ساكن فراغ atomic_test_suite_init(فراغ)
{
	ATOMIC_INIT(&test_atomic, 0);
}

ساكن فراغ atomic_rw_test(بنية sbiunit_test_case *test)
{
	/* We should read the same value as we've written */
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1);
	/* Negative value should also work */
	atomic_write(&test_atomic, -ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), -ATOMIC_TEST_VAL1);
}

ساكن فراغ جمع_کړه_return_test(بنية sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_جمع_کړه_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
	/* The atomic value should be updated as well */
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
}

ساكن فراغ sub_return_test(بنية sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_sub_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
}

ساكن فراغ cmpxchg_test(بنية sbiunit_test_case *test)
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

ساكن فراغ atomic_xchg_test(بنية sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_xchg(&test_atomic, ATOMIC_TEST_VAL2), ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL2);
}

ساكن فراغ atomic_raw_سېټ_بټ_test(بنية sbiunit_test_case *test)
{
	بینښه اوږد معلومات[] = {0, 0, 0};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_بټ_CELL * بټونه_فی_LONG + ATOMIC_TEST_RAW_بټ_عدد;

	/* check if the bit we set actually gets set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_سېټ_بټ(bitpos, معلومات), 0);
	SBIUNIT_EXPECT_EQ(test, معلومات[ATOMIC_TEST_RAW_بټ_CELL], 1 << ATOMIC_TEST_RAW_بټ_عدد);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, معلومات[0], 0);
	SBIUNIT_EXPECT_EQ(test, معلومات[2], 0);

	/* check that if we set the bit twice it stays set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_سېټ_بټ(bitpos, معلومات), 1);
	SBIUNIT_EXPECT_EQ(test, معلومات[ATOMIC_TEST_RAW_بټ_CELL], 1 << ATOMIC_TEST_RAW_بټ_عدد);
}

ساكن فراغ atomic_raw_clear_بټ_test(بنية sbiunit_test_case *test)
{
	بینښه اوږد معلومات[] = {~1UL, 1 << ATOMIC_TEST_RAW_بټ_عدد, ~1UL};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_بټ_CELL * بټونه_فی_LONG + ATOMIC_TEST_RAW_بټ_عدد;

	/* check if the bit we clear actually gets cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_بټ(bitpos, معلومات), 1);
	SBIUNIT_EXPECT_EQ(test, معلومات[ATOMIC_TEST_RAW_بټ_CELL], 0);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, معلومات[0], ~1UL);
	SBIUNIT_EXPECT_EQ(test, معلومات[2], ~1UL);

	/* check that if we clear the bit twice it stays cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_بټ(bitpos, معلومات), 0);
	SBIUNIT_EXPECT_EQ(test, معلومات[ATOMIC_TEST_RAW_بټ_CELL], 0);
}

ساكن فراغ atomic_سېټ_بټ_test(بنية sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 0);
	SBIUNIT_EXPECT_EQ(test, atomic_سېټ_بټ(ATOMIC_TEST_بټ_عدد, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_بټ_عدد);
	/* If we set the bit twice, it stays 1 */
	SBIUNIT_EXPECT_EQ(test, atomic_سېټ_بټ(ATOMIC_TEST_بټ_عدد, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_بټ_عدد);
}

ساكن فراغ atomic_clear_بټ_test(بنية sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 1 << ATOMIC_TEST_بټ_عدد);
	SBIUNIT_EXPECT_EQ(test, atomic_clear_بټ(ATOMIC_TEST_بټ_عدد, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
	/* if we clear the bit twice, it stays 0 */
	SBIUNIT_EXPECT_EQ(test, atomic_clear_بټ(ATOMIC_TEST_بټ_عدد, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
}

ساكن بنية sbiunit_test_case atomic_test_cases[] = {
	SBIUNIT_TEST_CASE(atomic_rw_test),
	SBIUNIT_TEST_CASE(جمع_کړه_return_test),
	SBIUNIT_TEST_CASE(sub_return_test),
	SBIUNIT_TEST_CASE(cmpxchg_test),
	SBIUNIT_TEST_CASE(atomic_xchg_test),
	SBIUNIT_TEST_CASE(atomic_raw_سېټ_بټ_test),
	SBIUNIT_TEST_CASE(atomic_raw_clear_بټ_test),
	SBIUNIT_TEST_CASE(atomic_سېټ_بټ_test),
	SBIUNIT_TEST_CASE(atomic_clear_بټ_test),
	SBIUNIT_پای_ټکی_CASE,
};

ثابت بنية sbiunit_test_suite atomic_test_suite = {
	.name = "atomic_test_suite",
	.cases = atomic_test_cases,
	.init = atomic_test_suite_init
};
