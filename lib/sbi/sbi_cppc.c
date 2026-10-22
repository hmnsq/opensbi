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
	врати cppc_dev;
}

void sbi_cppc_set_device(const struct sbi_cppc_device *dev)
{
	ако (!dev || cppc_dev)
		врати;

	cppc_dev = dev;
}

static bool sbi_cppc_is_reserved(unsigned long reg)
{
	ако ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		врати true;

	врати false;
}

static bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	врати true;
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
		врати false;
	}

	врати true;
}

цео sbi_cppc_probe(unsigned long reg)
{
	ако (!cppc_dev || !cppc_dev->cppc_probe)
		врати SBI_EFAIL;

	/* Check whether register is reserved */
	ако (sbi_cppc_is_reserved(reg))
		врати SBI_ERR_INVALID_PARAM;

	врати cppc_dev->cppc_probe(reg);
}

цео sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	цео ret;

	ако (!cppc_dev || !cppc_dev->cppc_read)
		врати SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ако (ret <= 0)
		врати SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	ако (!sbi_cppc_readable(reg))
		врати SBI_ERR_DENIED;

	врати cppc_dev->cppc_read(reg, val);
}

цео sbi_cppc_write(unsigned long reg, uint64_t val)
{
	цео ret;

	ако (!cppc_dev || !cppc_dev->cppc_write)
		врати SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ако (ret <= 0)
		врати SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	ако (!sbi_cppc_writable(reg))
		врати SBI_ERR_DENIED;

	врати cppc_dev->cppc_write(reg, val);
}
