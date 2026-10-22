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
	вернуть cppc_dev;
}

void sbi_cppc_set_device(const struct sbi_cppc_device *dev)
{
	если (!dev || cppc_dev)
		вернуть;

	cppc_dev = dev;
}

static bool sbi_cppc_is_reserved(unsigned long reg)
{
	если ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		вернуть true;

	вернуть false;
}

static bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	вернуть true;
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
		вернуть false;
	}

	вернуть true;
}

целое sbi_cppc_probe(unsigned long reg)
{
	если (!cppc_dev || !cppc_dev->cppc_probe)
		вернуть SBI_EFAIL;

	/* Check whether register is reserved */
	если (sbi_cppc_is_reserved(reg))
		вернуть SBI_ERR_INVALID_PARAM;

	вернуть cppc_dev->cppc_probe(reg);
}

целое sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	целое ret;

	если (!cppc_dev || !cppc_dev->cppc_read)
		вернуть SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	если (ret <= 0)
		вернуть SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	если (!sbi_cppc_readable(reg))
		вернуть SBI_ERR_DENIED;

	вернуть cppc_dev->cppc_read(reg, val);
}

целое sbi_cppc_write(unsigned long reg, uint64_t val)
{
	целое ret;

	если (!cppc_dev || !cppc_dev->cppc_write)
		вернуть SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	если (ret <= 0)
		вернуть SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	если (!sbi_cppc_writable(reg))
		вернуть SBI_ERR_DENIED;

	вернуть cppc_dev->cppc_write(reg, val);
}
