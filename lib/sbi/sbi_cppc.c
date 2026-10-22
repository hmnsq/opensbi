/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_cppc.h>

ساكن ثابت بنية sbi_cppc_device *cppc_dev = عدم;

ثابت بنية sbi_cppc_device *sbi_cppc_get_device(فراغ)
{
	החזר cppc_dev;
}

فراغ sbi_cppc_set_device(ثابت بنية sbi_cppc_device *dev)
{
	אם (!dev || cppc_dev)
		החזר;

	cppc_dev = dev;
}

ساكن bool sbi_cppc_is_reserved(unsigned long reg)
{
	אם ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		החזר true;

	החזר false;
}

ساكن bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	החזר true;
}

ساكن bool sbi_cppc_writable(unsigned long reg)
{
	بدل (reg) {
	حالة SBI_CPPC_HIGHEST_PERF:
	حالة SBI_CPPC_NOMINAL_PERF:
	حالة SBI_CPPC_LOW_NON_LINEAR_PERF:
	حالة SBI_CPPC_LOWEST_PERF:
	حالة SBI_CPPC_GUARANTEED_PERF:
	حالة SBI_CPPC_CTR_WRAP_TIME:
	حالة SBI_CPPC_REFERENCE_CTR:
	حالة SBI_CPPC_DELIVERED_CTR:
	حالة SBI_CPPC_REFERENCE_PERF:
	حالة SBI_CPPC_LOWEST_FREQ:
	حالة SBI_CPPC_NOMINAL_FREQ:
	حالة SBI_CPPC_TRANSITION_LATENCY:
		החזר false;
	}

	החזר true;
}

صحيح sbi_cppc_probe(unsigned long reg)
{
	אם (!cppc_dev || !cppc_dev->cppc_probe)
		החזר SBI_EFAIL;

	/* Check whether register is reserved */
	אם (sbi_cppc_is_reserved(reg))
		החזר SBI_ERR_INVALID_PARAM;

	החזר cppc_dev->cppc_probe(reg);
}

صحيح sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	صحيح ret;

	אם (!cppc_dev || !cppc_dev->cppc_read)
		החזר SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	אם (ret <= 0)
		החזר SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	אם (!sbi_cppc_readable(reg))
		החזר SBI_ERR_DENIED;

	החזר cppc_dev->cppc_read(reg, val);
}

صحيح sbi_cppc_write(unsigned long reg, uint64_t val)
{
	صحيح ret;

	אם (!cppc_dev || !cppc_dev->cppc_write)
		החזר SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	אם (ret <= 0)
		החזר SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	אם (!sbi_cppc_writable(reg))
		החזר SBI_ERR_DENIED;

	החזר cppc_dev->cppc_write(reg, val);
}
