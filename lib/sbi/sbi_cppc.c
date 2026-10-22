/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_cppc.h>

ایستا ثابت ساختار sbi_cppc_device *cppc_dev = پوچ;

ثابت ساختار sbi_cppc_device *sbi_cppc_get_device(تهی)
{
	بازگشت cppc_dev;
}

تهی sbi_cppc_set_device(ثابت ساختار sbi_cppc_device *dev)
{
	اگر (!dev || cppc_dev)
		بازگشت;

	cppc_dev = dev;
}

ایستا bool sbi_cppc_is_reserved(unsigned long reg)
{
	اگر ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		بازگشت true;

	بازگشت false;
}

ایستا bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	بازگشت true;
}

ایستا bool sbi_cppc_writable(unsigned long reg)
{
	انتخاب (reg) {
	حالت SBI_CPPC_HIGHEST_PERF:
	حالت SBI_CPPC_NOMINAL_PERF:
	حالت SBI_CPPC_کم_NON_LINEAR_PERF:
	حالت SBI_CPPC_LOWEST_PERF:
	حالت SBI_CPPC_GUARANTEED_PERF:
	حالت SBI_CPPC_CTR_WRAP_TIME:
	حالت SBI_CPPC_REFERENCE_CTR:
	حالت SBI_CPPC_DELIVERED_CTR:
	حالت SBI_CPPC_REFERENCE_PERF:
	حالت SBI_CPPC_LOWEST_FREQ:
	حالت SBI_CPPC_NOMINAL_FREQ:
	حالت SBI_CPPC_TRANSITION_LATENCY:
		بازگشت false;
	}

	بازگشت true;
}

صحیح sbi_cppc_probe(unsigned long reg)
{
	اگر (!cppc_dev || !cppc_dev->cppc_probe)
		بازگشت SBI_EFAIL;

	/* Check whether register is reserved */
	اگر (sbi_cppc_is_reserved(reg))
		بازگشت SBI_ERR_INVALID_PARAM;

	بازگشت cppc_dev->cppc_probe(reg);
}

صحیح sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	صحیح ret;

	اگر (!cppc_dev || !cppc_dev->cppc_read)
		بازگشت SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	اگر (ret <= 0)
		بازگشت SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	اگر (!sbi_cppc_readable(reg))
		بازگشت SBI_ERR_DENIED;

	بازگشت cppc_dev->cppc_read(reg, val);
}

صحیح sbi_cppc_write(unsigned long reg, uint64_t val)
{
	صحیح ret;

	اگر (!cppc_dev || !cppc_dev->cppc_write)
		بازگشت SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	اگر (ret <= 0)
		بازگشت SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	اگر (!sbi_cppc_writable(reg))
		بازگشت SBI_ERR_DENIED;

	بازگشت cppc_dev->cppc_write(reg, val);
}
