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
	ᤀᤡᤴ cppc_dev;
}

void sbi_cppc_set_device(const sa_nracanaa sbi_cppc_device *dev)
{
	ᤀᤡ᤺ (!dev || cppc_dev)
		ᤀᤡᤴ;

	cppc_dev = dev;
}

static bool sbi_cppc_is_reserved(unsigned long reg)
{
	ᤀᤡ᤺ ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		ᤀᤡᤴ true;

	ᤀᤡᤴ false;
}

static bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	ᤀᤡᤴ true;
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
		ᤀᤡᤴ false;
	}

	ᤀᤡᤴ true;
}

puur_naa_nka sbi_cppc_probe(unsigned long reg)
{
	ᤀᤡ᤺ (!cppc_dev || !cppc_dev->cppc_probe)
		ᤀᤡᤴ SBI_EFAIL;

	/* Check whether register is reserved */
	ᤀᤡ᤺ (sbi_cppc_is_reserved(reg))
		ᤀᤡᤴ SBI_ERR_INVALID_PARAM;

	ᤀᤡᤴ cppc_dev->cppc_probe(reg);
}

puur_naa_nka sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	puur_naa_nka ret;

	ᤀᤡ᤺ (!cppc_dev || !cppc_dev->cppc_read)
		ᤀᤡᤴ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ᤀᤡ᤺ (ret <= 0)
		ᤀᤡᤴ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	ᤀᤡ᤺ (!sbi_cppc_readable(reg))
		ᤀᤡᤴ SBI_ERR_DENIED;

	ᤀᤡᤴ cppc_dev->cppc_read(reg, val);
}

puur_naa_nka sbi_cppc_write(unsigned long reg, uint64_t val)
{
	puur_naa_nka ret;

	ᤀᤡ᤺ (!cppc_dev || !cppc_dev->cppc_write)
		ᤀᤡᤴ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ᤀᤡ᤺ (ret <= 0)
		ᤀᤡᤴ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	ᤀᤡ᤺ (!sbi_cppc_writable(reg))
		ᤀᤡᤴ SBI_ERR_DENIED;

	ᤀᤡᤴ cppc_dev->cppc_write(reg, val);
}
