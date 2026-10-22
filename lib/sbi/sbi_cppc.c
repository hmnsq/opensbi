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
	ပြန်ပေး cppc_dev;
}

void sbi_cppc_set_device(const sa_nracanaa sbi_cppc_device *dev)
{
	အကယ်၍ (!dev || cppc_dev)
		ပြန်ပေး;

	cppc_dev = dev;
}

static bool sbi_cppc_is_reserved(unsigned long reg)
{
	အကယ်၍ ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		ပြန်ပေး true;

	ပြန်ပေး false;
}

static bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	ပြန်ပေး true;
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
		ပြန်ပေး false;
	}

	ပြန်ပေး true;
}

ကိန်းပြည့် sbi_cppc_probe(unsigned long reg)
{
	အကယ်၍ (!cppc_dev || !cppc_dev->cppc_probe)
		ပြန်ပေး SBI_EFAIL;

	/* Check whether register is reserved */
	အကယ်၍ (sbi_cppc_is_reserved(reg))
		ပြန်ပေး SBI_ERR_INVALID_PARAM;

	ပြန်ပေး cppc_dev->cppc_probe(reg);
}

ကိန်းပြည့် sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	ကိန်းပြည့် ret;

	အကယ်၍ (!cppc_dev || !cppc_dev->cppc_read)
		ပြန်ပေး SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	အကယ်၍ (ret <= 0)
		ပြန်ပေး SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	အကယ်၍ (!sbi_cppc_readable(reg))
		ပြန်ပေး SBI_ERR_DENIED;

	ပြန်ပေး cppc_dev->cppc_read(reg, val);
}

ကိန်းပြည့် sbi_cppc_write(unsigned long reg, uint64_t val)
{
	ကိန်းပြည့် ret;

	အကယ်၍ (!cppc_dev || !cppc_dev->cppc_write)
		ပြန်ပေး SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	အကယ်၍ (ret <= 0)
		ပြန်ပေး SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	အကယ်၍ (!sbi_cppc_writable(reg))
		ပြန်ပေး SBI_ERR_DENIED;

	ပြန်ပေး cppc_dev->cppc_write(reg, val);
}
