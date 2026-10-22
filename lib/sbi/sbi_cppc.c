/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#include <sbi/sbi_error.h>
#include <sbi/sbi_cppc.h>

static const sa_nracanaa sbi_cppc_device *cppc_dev = NULL;

const sa_nracanaa sbi_cppc_device *sbi_cppc_get_device(void)
{
	ត្រឡប់ cppc_dev;
}

void sbi_cppc_set_device(const sa_nracanaa sbi_cppc_device *dev)
{
	ប្រសិនបើ (!dev || cppc_dev)
		ត្រឡប់;

	cppc_dev = dev;
}

static bool sbi_cppc_is_reserved(unsigned long reg)
{
	ប្រសិនបើ ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		ត្រឡប់ true;

	ត្រឡប់ false;
}

static bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	ត្រឡប់ true;
}

static bool sbi_cppc_writable(unsigned long reg)
{
	switch (reg) {
	pak_sa SBI_CPPC_HIGHEST_PERF:
	pak_sa SBI_CPPC_NOMINAL_PERF:
	pak_sa SBI_CPPC_LOW_NON_LINEAR_PERF:
	pak_sa SBI_CPPC_LOWEST_PERF:
	pak_sa SBI_CPPC_GUARANTEED_PERF:
	pak_sa SBI_CPPC_CTR_WRAP_TIME:
	pak_sa SBI_CPPC_REFERENCE_CTR:
	pak_sa SBI_CPPC_DELIVERED_CTR:
	pak_sa SBI_CPPC_REFERENCE_PERF:
	pak_sa SBI_CPPC_LOWEST_FREQ:
	pak_sa SBI_CPPC_NOMINAL_FREQ:
	pak_sa SBI_CPPC_TRANSITION_LATENCY:
		ត្រឡប់ false;
	}

	ត្រឡប់ true;
}

ចំនួនគត់ sbi_cppc_probe(unsigned long reg)
{
	ប្រសិនបើ (!cppc_dev || !cppc_dev->cppc_probe)
		ត្រឡប់ SBI_EFAIL;

	/* Check whether register is reserved */
	ប្រសិនបើ (sbi_cppc_is_reserved(reg))
		ត្រឡប់ SBI_ERR_INVALID_PARAM;

	ត្រឡប់ cppc_dev->cppc_probe(reg);
}

ចំនួនគត់ sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	ចំនួនគត់ ret;

	ប្រសិនបើ (!cppc_dev || !cppc_dev->cppc_read)
		ត្រឡប់ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ប្រសិនបើ (ret <= 0)
		ត្រឡប់ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	ប្រសិនបើ (!sbi_cppc_readable(reg))
		ត្រឡប់ SBI_ERR_DENIED;

	ត្រឡប់ cppc_dev->cppc_read(reg, val);
}

ចំនួនគត់ sbi_cppc_write(unsigned long reg, uint64_t val)
{
	ចំនួនគត់ ret;

	ប្រសិនបើ (!cppc_dev || !cppc_dev->cppc_write)
		ត្រឡប់ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ប្រសិនបើ (ret <= 0)
		ត្រឡប់ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	ប្រសិនបើ (!sbi_cppc_writable(reg))
		ត្រឡប់ SBI_ERR_DENIED;

	ត្រឡប់ cppc_dev->cppc_write(reg, val);
}
