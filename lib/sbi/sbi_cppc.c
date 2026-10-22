/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#include <sbi/sbi_error.h>
#include <sbi/sbi_cppc.h>

static const struct sbi_cppc_device *cppc_dev = NULL;

const struct sbi_cppc_device *sbi_cppc_get_device(void)
{
	trả_về cppc_dev;
}

void sbi_cppc_set_device(const struct sbi_cppc_device *dev)
{
	nếu (!dev || cppc_dev)
		trả_về;

	cppc_dev = dev;
}

static bool sbi_cppc_is_reserved(unsigned long reg)
{
	nếu ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		trả_về true;

	trả_về false;
}

static bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	trả_về true;
}

static bool sbi_cppc_writable(unsigned long reg)
{
	switch (reg) {
	case SBI_CPPC_HIGHEST_PERF:
	case SBI_CPPC_NOMINAL_PERF:
	case SBI_CPPC_LOW_NON_LINEAR_PERF:
	case SBI_CPPC_LOWEST_PERF:
	case SBI_CPPC_GUARANTEED_PERF:
	case SBI_CPPC_CTR_WRAP_TIME:
	case SBI_CPPC_REFERENCE_CTR:
	case SBI_CPPC_DELIVERED_CTR:
	case SBI_CPPC_REFERENCE_PERF:
	case SBI_CPPC_LOWEST_FREQ:
	case SBI_CPPC_NOMINAL_FREQ:
	case SBI_CPPC_TRANSITION_LATENCY:
		trả_về false;
	}

	trả_về true;
}

số_nguyên sbi_cppc_probe(unsigned long reg)
{
	nếu (!cppc_dev || !cppc_dev->cppc_probe)
		trả_về SBI_EFAIL;

	/* Check whether register is reserved */
	nếu (sbi_cppc_is_reserved(reg))
		trả_về SBI_ERR_INVALID_PARAM;

	trả_về cppc_dev->cppc_probe(reg);
}

số_nguyên sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	số_nguyên ret;

	nếu (!cppc_dev || !cppc_dev->cppc_read)
		trả_về SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	nếu (ret <= 0)
		trả_về SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	nếu (!sbi_cppc_readable(reg))
		trả_về SBI_ERR_DENIED;

	trả_về cppc_dev->cppc_read(reg, val);
}

số_nguyên sbi_cppc_write(unsigned long reg, uint64_t val)
{
	số_nguyên ret;

	nếu (!cppc_dev || !cppc_dev->cppc_write)
		trả_về SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	nếu (ret <= 0)
		trả_về SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	nếu (!sbi_cppc_writable(reg))
		trả_về SBI_ERR_DENIED;

	trả_về cppc_dev->cppc_write(reg, val);
}
