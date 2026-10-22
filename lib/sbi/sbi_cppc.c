/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_cppc.h>

স্থির ধ্রুব সংরচনা sbi_cppc_device *cppc_dev = রিক্ত;

ধ্রুব সংরচনা sbi_cppc_device *sbi_cppc_get_device(শূন্য)
{
	প্রত্যাগম cppc_dev;
}

শূন্য sbi_cppc_set_device(ধ্রুব সংরচনা sbi_cppc_device *dev)
{
	যদি (!dev || cppc_dev)
		প্রত্যাগম;

	cppc_dev = dev;
}

স্থির bool sbi_cppc_is_reserved(unsigned long reg)
{
	যদি ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		প্রত্যাগম true;

	প্রত্যাগম false;
}

স্থির bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	প্রত্যাগম true;
}

স্থির bool sbi_cppc_writable(unsigned long reg)
{
	বিকল্প (reg) {
	পক্ষ SBI_CPPC_HIGHEST_PERF:
	পক্ষ SBI_CPPC_NOMINAL_PERF:
	পক্ষ SBI_CPPC_নিচের_NON_LINEAR_PERF:
	পক্ষ SBI_CPPC_LOWEST_PERF:
	পক্ষ SBI_CPPC_GUARANTEED_PERF:
	পক্ষ SBI_CPPC_CTR_WRAP_TIME:
	পক্ষ SBI_CPPC_REFERENCE_CTR:
	পক্ষ SBI_CPPC_DELIVERED_CTR:
	পক্ষ SBI_CPPC_REFERENCE_PERF:
	পক্ষ SBI_CPPC_LOWEST_FREQ:
	পক্ষ SBI_CPPC_NOMINAL_FREQ:
	পক্ষ SBI_CPPC_TRANSITION_LATENCY:
		প্রত্যাগম false;
	}

	প্রত্যাগম true;
}

পূর্ণাঙ্ক sbi_cppc_probe(unsigned long reg)
{
	যদি (!cppc_dev || !cppc_dev->cppc_probe)
		প্রত্যাগম SBI_EFAIL;

	/* Check whether register is reserved */
	যদি (sbi_cppc_is_reserved(reg))
		প্রত্যাগম SBI_ERR_INVALID_PARAM;

	প্রত্যাগম cppc_dev->cppc_probe(reg);
}

পূর্ণাঙ্ক sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	পূর্ণাঙ্ক ret;

	যদি (!cppc_dev || !cppc_dev->cppc_read)
		প্রত্যাগম SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	যদি (ret <= 0)
		প্রত্যাগম SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	যদি (!sbi_cppc_readable(reg))
		প্রত্যাগম SBI_ERR_DENIED;

	প্রত্যাগম cppc_dev->cppc_read(reg, val);
}

পূর্ণাঙ্ক sbi_cppc_write(unsigned long reg, uint64_t val)
{
	পূর্ণাঙ্ক ret;

	যদি (!cppc_dev || !cppc_dev->cppc_write)
		প্রত্যাগম SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	যদি (ret <= 0)
		প্রত্যাগম SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	যদি (!sbi_cppc_writable(reg))
		প্রত্যাগম SBI_ERR_DENIED;

	প্রত্যাগম cppc_dev->cppc_write(reg, val);
}
