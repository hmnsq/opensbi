/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_cppc.h>

স্থির ধ্রুবক কাঠামো sbi_cppc_device *cppc_dev = ফাঁকা;

ধ্রুবক কাঠামো sbi_cppc_device *sbi_cppc_get_device(শূন্য)
{
	ফেরত cppc_dev;
}

শূন্য sbi_cppc_set_device(ধ্রুবক কাঠামো sbi_cppc_device *dev)
{
	যদি (!dev || cppc_dev)
		ফেরত;

	cppc_dev = dev;
}

স্থির bool sbi_cppc_is_reserved(unsigned long reg)
{
	যদি ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		ফেরত true;

	ফেরত false;
}

স্থির bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	ফেরত true;
}

স্থির bool sbi_cppc_writable(unsigned long reg)
{
	নির্বাচন (reg) {
	ক্ষেত্র SBI_CPPC_HIGHEST_PERF:
	ক্ষেত্র SBI_CPPC_NOMINAL_PERF:
	ক্ষেত্র SBI_CPPC_নিচের_NON_LINEAR_PERF:
	ক্ষেত্র SBI_CPPC_LOWEST_PERF:
	ক্ষেত্র SBI_CPPC_GUARANTEED_PERF:
	ক্ষেত্র SBI_CPPC_CTR_WRAP_TIME:
	ক্ষেত্র SBI_CPPC_REFERENCE_CTR:
	ক্ষেত্র SBI_CPPC_DELIVERED_CTR:
	ক্ষেত্র SBI_CPPC_REFERENCE_PERF:
	ক্ষেত্র SBI_CPPC_LOWEST_FREQ:
	ক্ষেত্র SBI_CPPC_NOMINAL_FREQ:
	ক্ষেত্র SBI_CPPC_TRANSITION_LATENCY:
		ফেরত false;
	}

	ফেরত true;
}

পূর্ণসংখ্যা sbi_cppc_probe(unsigned long reg)
{
	যদি (!cppc_dev || !cppc_dev->cppc_probe)
		ফেরত SBI_EFAIL;

	/* Check whether register is reserved */
	যদি (sbi_cppc_is_reserved(reg))
		ফেরত SBI_ERR_INVALID_PARAM;

	ফেরত cppc_dev->cppc_probe(reg);
}

পূর্ণসংখ্যা sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	পূর্ণসংখ্যা ret;

	যদি (!cppc_dev || !cppc_dev->cppc_read)
		ফেরত SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	যদি (ret <= 0)
		ফেরত SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	যদি (!sbi_cppc_readable(reg))
		ফেরত SBI_ERR_DENIED;

	ফেরত cppc_dev->cppc_read(reg, val);
}

পূর্ণসংখ্যা sbi_cppc_write(unsigned long reg, uint64_t val)
{
	পূর্ণসংখ্যা ret;

	যদি (!cppc_dev || !cppc_dev->cppc_write)
		ফেরত SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	যদি (ret <= 0)
		ফেরত SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	যদি (!sbi_cppc_writable(reg))
		ফেরত SBI_ERR_DENIED;

	ফেরত cppc_dev->cppc_write(reg, val);
}
