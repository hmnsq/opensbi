#شامل <sbi/sbi_unit_test.h>
#شامل <sbi/riscv_atomic.h>
#شامل <sbi/sbi_bitops.h>

#تعریف_کرو ATOMIC_TEST_VAL1 239l
#تعریف_کرو ATOMIC_TEST_VAL2 30l
#تعریف_کرو ATOMIC_TEST_VAL3 2024l

#تعریف_کرو ATOMIC_TEST_ثنائی_ہندسہ_عدد 3

#تعریف_کرو ATOMIC_TEST_RAW_ثنائی_ہندسہ_CELL 1
#تعریف_کرو ATOMIC_TEST_RAW_ثنائی_ہندسہ_عدد 15

ساکن atomic_t test_atomic;

ساکن خالی atomic_test_suite_init(خالی)
{
	ATOMIC_INIT(&test_atomic, 0);
}

ساکن خالی atomic_rw_test(ساخت sbiunit_test_case *test)
{
	/* We should read the same value as we've written */
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1);
	/* Negative value should also work */
	atomic_write(&test_atomic, -ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), -ATOMIC_TEST_VAL1);
}

ساکن خالی جوڑو_return_test(ساخت sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_جوڑو_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
	/* The atomic value should be updated as well */
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 + ATOMIC_TEST_VAL2);
}

ساکن خالی sub_return_test(ساخت sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_sub_return(&test_atomic, ATOMIC_TEST_VAL2),
			  ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL1 - ATOMIC_TEST_VAL2);
}

ساکن خالی cmpxchg_test(ساخت sbiunit_test_case *test)
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

ساکن خالی atomic_xchg_test(ساخت sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_xchg(&test_atomic, ATOMIC_TEST_VAL2), ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), ATOMIC_TEST_VAL2);
}

ساکن خالی atomic_raw_سیٹ_ثنائی_ہندسہ_test(ساخت sbiunit_test_case *test)
{
	غیرعلامتی طویل ڈیٹا[] = {0, 0, 0};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_ثنائی_ہندسہ_CELL * بٹ_فی_LONG + ATOMIC_TEST_RAW_ثنائی_ہندسہ_عدد;

	/* check if the bit we set actually gets set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_سیٹ_ثنائی_ہندسہ(bitpos, ڈیٹا), 0);
	SBIUNIT_EXPECT_EQ(test, ڈیٹا[ATOMIC_TEST_RAW_ثنائی_ہندسہ_CELL], 1 << ATOMIC_TEST_RAW_ثنائی_ہندسہ_عدد);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, ڈیٹا[0], 0);
	SBIUNIT_EXPECT_EQ(test, ڈیٹا[2], 0);

	/* check that if we set the bit twice it stays set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_سیٹ_ثنائی_ہندسہ(bitpos, ڈیٹا), 1);
	SBIUNIT_EXPECT_EQ(test, ڈیٹا[ATOMIC_TEST_RAW_ثنائی_ہندسہ_CELL], 1 << ATOMIC_TEST_RAW_ثنائی_ہندسہ_عدد);
}

ساکن خالی atomic_raw_clear_ثنائی_ہندسہ_test(ساخت sbiunit_test_case *test)
{
	غیرعلامتی طویل ڈیٹا[] = {~1UL, 1 << ATOMIC_TEST_RAW_ثنائی_ہندسہ_عدد, ~1UL};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_ثنائی_ہندسہ_CELL * بٹ_فی_LONG + ATOMIC_TEST_RAW_ثنائی_ہندسہ_عدد;

	/* check if the bit we clear actually gets cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_ثنائی_ہندسہ(bitpos, ڈیٹا), 1);
	SBIUNIT_EXPECT_EQ(test, ڈیٹا[ATOMIC_TEST_RAW_ثنائی_ہندسہ_CELL], 0);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, ڈیٹا[0], ~1UL);
	SBIUNIT_EXPECT_EQ(test, ڈیٹا[2], ~1UL);

	/* check that if we clear the bit twice it stays cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_ثنائی_ہندسہ(bitpos, ڈیٹا), 0);
	SBIUNIT_EXPECT_EQ(test, ڈیٹا[ATOMIC_TEST_RAW_ثنائی_ہندسہ_CELL], 0);
}

ساکن خالی atomic_سیٹ_ثنائی_ہندسہ_test(ساخت sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 0);
	SBIUNIT_EXPECT_EQ(test, atomic_سیٹ_ثنائی_ہندسہ(ATOMIC_TEST_ثنائی_ہندسہ_عدد, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_ثنائی_ہندسہ_عدد);
	/* If we set the bit twice, it stays 1 */
	SBIUNIT_EXPECT_EQ(test, atomic_سیٹ_ثنائی_ہندسہ(ATOMIC_TEST_ثنائی_ہندسہ_عدد, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_ثنائی_ہندسہ_عدد);
}

ساکن خالی atomic_clear_ثنائی_ہندسہ_test(ساخت sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 1 << ATOMIC_TEST_ثنائی_ہندسہ_عدد);
	SBIUNIT_EXPECT_EQ(test, atomic_clear_ثنائی_ہندسہ(ATOMIC_TEST_ثنائی_ہندسہ_عدد, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
	/* if we clear the bit twice, it stays 0 */
	SBIUNIT_EXPECT_EQ(test, atomic_clear_ثنائی_ہندسہ(ATOMIC_TEST_ثنائی_ہندسہ_عدد, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
}

ساکن ساخت sbiunit_test_case atomic_test_cases[] = {
	SBIUNIT_TEST_CASE(atomic_rw_test),
	SBIUNIT_TEST_CASE(جوڑو_return_test),
	SBIUNIT_TEST_CASE(sub_return_test),
	SBIUNIT_TEST_CASE(cmpxchg_test),
	SBIUNIT_TEST_CASE(atomic_xchg_test),
	SBIUNIT_TEST_CASE(atomic_raw_سیٹ_ثنائی_ہندسہ_test),
	SBIUNIT_TEST_CASE(atomic_raw_clear_ثنائی_ہندسہ_test),
	SBIUNIT_TEST_CASE(atomic_سیٹ_ثنائی_ہندسہ_test),
	SBIUNIT_TEST_CASE(atomic_clear_ثنائی_ہندسہ_test),
	SBIUNIT_انتہا_CASE,
};

ثابت ساخت sbiunit_test_suite atomic_test_suite = {
	.name = "atomic_test_suite",
	.cases = atomic_test_cases,
	.init = atomic_test_suite_init
};
