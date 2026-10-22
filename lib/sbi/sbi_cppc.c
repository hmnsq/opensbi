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
	ပွၵ်ႈ cppc_dev;
}

void sbi_cppc_set_device(const sa_nracanaa sbi_cppc_device *dev)
{
	ပေႃး (!dev || cppc_dev)
		ပွၵ်ႈ;

	cppc_dev = dev;
}

static bool sbi_cppc_is_reserved(unsigned long reg)
{
	ပေႃး ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		ပွၵ်ႈ true;

	ပွၵ်ႈ false;
}

static bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	ပွၵ်ႈ true;
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
		ပွၵ်ႈ false;
	}

	ပွၵ်ႈ true;
}

puur_naa_nka sbi_cppc_probe(unsigned long reg)
{
	ပေႃး (!cppc_dev || !cppc_dev->cppc_probe)
		ပွၵ်ႈ SBI_EFAIL;

	/* Check whether register is reserved */
	ပေႃး (sbi_cppc_is_reserved(reg))
		ပွၵ်ႈ SBI_ERR_INVALID_PARAM;

	ပွၵ်ႈ cppc_dev->cppc_probe(reg);
}

puur_naa_nka sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	puur_naa_nka ret;

	ပေႃး (!cppc_dev || !cppc_dev->cppc_read)
		ပွၵ်ႈ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ပေႃး (ret <= 0)
		ပွၵ်ႈ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	ပေႃး (!sbi_cppc_readable(reg))
		ပွၵ်ႈ SBI_ERR_DENIED;

	ပွၵ်ႈ cppc_dev->cppc_read(reg, val);
}

puur_naa_nka sbi_cppc_write(unsigned long reg, uint64_t val)
{
	puur_naa_nka ret;

	ပေႃး (!cppc_dev || !cppc_dev->cppc_write)
		ပွၵ်ႈ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ပေႃး (ret <= 0)
		ပွၵ်ႈ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	ပေႃး (!sbi_cppc_writable(reg))
		ပွၵ်ႈ SBI_ERR_DENIED;

	ပွၵ်ႈ cppc_dev->cppc_write(reg, val);
}
