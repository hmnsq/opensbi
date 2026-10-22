/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_cppc.h>

સ્થિર અચલ સંરચના sbi_cppc_device *cppc_dev = ખાલી;

અચલ સંરચના sbi_cppc_device *sbi_cppc_get_device(શૂન્ય)
{
	પાછા cppc_dev;
}

શૂન્ય sbi_cppc_set_device(અચલ સંરચના sbi_cppc_device *dev)
{
	જો (!dev || cppc_dev)
		પાછા;

	cppc_dev = dev;
}

સ્થિર bool sbi_cppc_is_reserved(unsigned long reg)
{
	જો ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		પાછા true;

	પાછા false;
}

સ્થિર bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	પાછા true;
}

સ્થિર bool sbi_cppc_writable(unsigned long reg)
{
	પસંદગી (reg) {
	સ્થિતિ SBI_CPPC_HIGHEST_PERF:
	સ્થિતિ SBI_CPPC_NOMINAL_PERF:
	સ્થિતિ SBI_CPPC_નીચલું_NON_LINEAR_PERF:
	સ્થિતિ SBI_CPPC_LOWEST_PERF:
	સ્થિતિ SBI_CPPC_GUARANTEED_PERF:
	સ્થિતિ SBI_CPPC_CTR_WRAP_TIME:
	સ્થિતિ SBI_CPPC_REFERENCE_CTR:
	સ્થિતિ SBI_CPPC_DELIVERED_CTR:
	સ્થિતિ SBI_CPPC_REFERENCE_PERF:
	સ્થિતિ SBI_CPPC_LOWEST_FREQ:
	સ્થિતિ SBI_CPPC_NOMINAL_FREQ:
	સ્થિતિ SBI_CPPC_TRANSITION_LATENCY:
		પાછા false;
	}

	પાછા true;
}

પૂર્ણાંક sbi_cppc_probe(unsigned long reg)
{
	જો (!cppc_dev || !cppc_dev->cppc_probe)
		પાછા SBI_EFAIL;

	/* Check whether register is reserved */
	જો (sbi_cppc_is_reserved(reg))
		પાછા SBI_ERR_INVALID_PARAM;

	પાછા cppc_dev->cppc_probe(reg);
}

પૂર્ણાંક sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	પૂર્ણાંક ret;

	જો (!cppc_dev || !cppc_dev->cppc_read)
		પાછા SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	જો (ret <= 0)
		પાછા SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	જો (!sbi_cppc_readable(reg))
		પાછા SBI_ERR_DENIED;

	પાછા cppc_dev->cppc_read(reg, val);
}

પૂર્ણાંક sbi_cppc_write(unsigned long reg, uint64_t val)
{
	પૂર્ણાંક ret;

	જો (!cppc_dev || !cppc_dev->cppc_write)
		પાછા SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	જો (ret <= 0)
		પાછા SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	જો (!sbi_cppc_writable(reg))
		પાછા SBI_ERR_DENIED;

	પાછા cppc_dev->cppc_write(reg, val);
}
