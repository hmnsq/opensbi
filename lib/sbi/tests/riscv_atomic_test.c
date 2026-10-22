#ضمن <sbi/sbi_unit_test.h>
#ضمن <sbi/riscv_atomic.h>
#ضمن <sbi/sbi_bitops.h>

#وصف_ڪريو ATOMIC_TEST_VAL1 239l
#وصف_ڪريو ATOMIC_TEST_VAL2 30l
#وصف_ڪريو ATOMIC_TEST_VAL3 2024l

#وصف_ڪريو ATOMIC_TEST_ٻٽو_انگ_انگ 3

#وصف_ڪريو ATOMIC_TEST_RAW_ٻٽو_انگ_CELL 1
#وصف_ڪريو ATOMIC_TEST_RAW_ٻٽو_انگ_انگ 15

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

ساكن فراغ جوڙيو_return_test(بنية sbiunit_test_case *test)
{
	atomic_write(&test_atomic, ATOMIC_TEST_VAL1);
	SBIUNIT_EXPECT_EQ(test, atomic_جوڙيو_return(&test_atomic, ATOMIC_TEST_VAL2),
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

ساكن فراغ atomic_raw_سيٽ_ٻٽو_انگ_test(بنية sbiunit_test_case *test)
{
	بنانشان ڊگهو معلومات[] = {0, 0, 0};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_ٻٽو_انگ_CELL * بٽ_في_LONG + ATOMIC_TEST_RAW_ٻٽو_انگ_انگ;

	/* check if the bit we set actually gets set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_سيٽ_ٻٽو_انگ(bitpos, معلومات), 0);
	SBIUNIT_EXPECT_EQ(test, معلومات[ATOMIC_TEST_RAW_ٻٽو_انگ_CELL], 1 << ATOMIC_TEST_RAW_ٻٽو_انگ_انگ);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, معلومات[0], 0);
	SBIUNIT_EXPECT_EQ(test, معلومات[2], 0);

	/* check that if we set the bit twice it stays set */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_سيٽ_ٻٽو_انگ(bitpos, معلومات), 1);
	SBIUNIT_EXPECT_EQ(test, معلومات[ATOMIC_TEST_RAW_ٻٽو_انگ_CELL], 1 << ATOMIC_TEST_RAW_ٻٽو_انگ_انگ);
}

ساكن فراغ atomic_raw_clear_ٻٽو_انگ_test(بنية sbiunit_test_case *test)
{
	بنانشان ڊگهو معلومات[] = {~1UL, 1 << ATOMIC_TEST_RAW_ٻٽو_انگ_انگ, ~1UL};
	/* the bitpos points to the bit of one of the elements of the `data` array */
	size_t bitpos = ATOMIC_TEST_RAW_ٻٽو_انگ_CELL * بٽ_في_LONG + ATOMIC_TEST_RAW_ٻٽو_انگ_انگ;

	/* check if the bit we clear actually gets cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_ٻٽو_انگ(bitpos, معلومات), 1);
	SBIUNIT_EXPECT_EQ(test, معلومات[ATOMIC_TEST_RAW_ٻٽو_انگ_CELL], 0);

	/* Other elements of the `data` array should stay untouched */
	SBIUNIT_EXPECT_EQ(test, معلومات[0], ~1UL);
	SBIUNIT_EXPECT_EQ(test, معلومات[2], ~1UL);

	/* check that if we clear the bit twice it stays cleared */
	SBIUNIT_EXPECT_EQ(test, atomic_raw_clear_ٻٽو_انگ(bitpos, معلومات), 0);
	SBIUNIT_EXPECT_EQ(test, معلومات[ATOMIC_TEST_RAW_ٻٽو_انگ_CELL], 0);
}

ساكن فراغ atomic_سيٽ_ٻٽو_انگ_test(بنية sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 0);
	SBIUNIT_EXPECT_EQ(test, atomic_سيٽ_ٻٽو_انگ(ATOMIC_TEST_ٻٽو_انگ_انگ, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_ٻٽو_انگ_انگ);
	/* If we set the bit twice, it stays 1 */
	SBIUNIT_EXPECT_EQ(test, atomic_سيٽ_ٻٽو_انگ(ATOMIC_TEST_ٻٽو_انگ_انگ, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 1 << ATOMIC_TEST_ٻٽو_انگ_انگ);
}

ساكن فراغ atomic_clear_ٻٽو_انگ_test(بنية sbiunit_test_case *test)
{
	atomic_write(&test_atomic, 1 << ATOMIC_TEST_ٻٽو_انگ_انگ);
	SBIUNIT_EXPECT_EQ(test, atomic_clear_ٻٽو_انگ(ATOMIC_TEST_ٻٽو_انگ_انگ, &test_atomic), 1);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
	/* if we clear the bit twice, it stays 0 */
	SBIUNIT_EXPECT_EQ(test, atomic_clear_ٻٽو_انگ(ATOMIC_TEST_ٻٽو_انگ_انگ, &test_atomic), 0);
	SBIUNIT_EXPECT_EQ(test, atomic_read(&test_atomic), 0);
}

ساكن بنية sbiunit_test_case atomic_test_cases[] = {
	SBIUNIT_TEST_CASE(atomic_rw_test),
	SBIUNIT_TEST_CASE(جوڙيو_return_test),
	SBIUNIT_TEST_CASE(sub_return_test),
	SBIUNIT_TEST_CASE(cmpxchg_test),
	SBIUNIT_TEST_CASE(atomic_xchg_test),
	SBIUNIT_TEST_CASE(atomic_raw_سيٽ_ٻٽو_انگ_test),
	SBIUNIT_TEST_CASE(atomic_raw_clear_ٻٽو_انگ_test),
	SBIUNIT_TEST_CASE(atomic_سيٽ_ٻٽو_انگ_test),
	SBIUNIT_TEST_CASE(atomic_clear_ٻٽو_انگ_test),
	SBIUNIT_پڇاڙي_CASE,
};

ثابت بنية sbiunit_test_suite atomic_test_suite = {
	.name = "atomic_test_suite",
	.cases = atomic_test_cases,
	.init = atomic_test_suite_init
};
