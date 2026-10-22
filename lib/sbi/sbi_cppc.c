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
	върни cppc_dev;
}

void sbi_cppc_set_device(const struct sbi_cppc_device *dev)
{
	ако (!dev || cppc_dev)
		върни;

	cppc_dev = dev;
}

static bool sbi_cppc_is_reserved(unsigned long reg)
{
	ако ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		върни true;

	върни false;
}

static bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	върни true;
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
		върни false;
	}

	върни true;
}

цяло sbi_cppc_probe(unsigned long reg)
{
	ако (!cppc_dev || !cppc_dev->cppc_probe)
		върни SBI_EFAIL;

	/* Check whether register is reserved */
	ако (sbi_cppc_is_reserved(reg))
		върни SBI_ERR_INVALID_PARAM;

	върни cppc_dev->cppc_probe(reg);
}

цяло sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	цяло ret;

	ако (!cppc_dev || !cppc_dev->cppc_read)
		върни SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ако (ret <= 0)
		върни SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	ако (!sbi_cppc_readable(reg))
		върни SBI_ERR_DENIED;

	върни cppc_dev->cppc_read(reg, val);
}

цяло sbi_cppc_write(unsigned long reg, uint64_t val)
{
	цяло ret;

	ако (!cppc_dev || !cppc_dev->cppc_write)
		върни SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ако (ret <= 0)
		върни SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	ако (!sbi_cppc_writable(reg))
		върни SBI_ERR_DENIED;

	върни cppc_dev->cppc_write(reg, val);
}
