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
	返回 cppc_dev;
}

void sbi_cppc_set_device(const struct sbi_cppc_device *dev)
{
	如果 (!dev || cppc_dev)
		返回;

	cppc_dev = dev;
}

static bool sbi_cppc_is_reserved(unsigned long reg)
{
	如果 ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		返回 true;

	返回 false;
}

static bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	返回 true;
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
		返回 false;
	}

	返回 true;
}

整数 sbi_cppc_probe(unsigned long reg)
{
	如果 (!cppc_dev || !cppc_dev->cppc_probe)
		返回 SBI_EFAIL;

	/* Check whether register is reserved */
	如果 (sbi_cppc_is_reserved(reg))
		返回 SBI_ERR_INVALID_PARAM;

	返回 cppc_dev->cppc_probe(reg);
}

整数 sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	整数 ret;

	如果 (!cppc_dev || !cppc_dev->cppc_read)
		返回 SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	如果 (ret <= 0)
		返回 SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	如果 (!sbi_cppc_readable(reg))
		返回 SBI_ERR_DENIED;

	返回 cppc_dev->cppc_read(reg, val);
}

整数 sbi_cppc_write(unsigned long reg, uint64_t val)
{
	整数 ret;

	如果 (!cppc_dev || !cppc_dev->cppc_write)
		返回 SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	如果 (ret <= 0)
		返回 SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	如果 (!sbi_cppc_writable(reg))
		返回 SBI_ERR_DENIED;

	返回 cppc_dev->cppc_write(reg, val);
}
