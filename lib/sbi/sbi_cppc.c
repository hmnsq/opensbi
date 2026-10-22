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
	คืน cppc_dev;
}

void sbi_cppc_set_device(const sa_nracanaa sbi_cppc_device *dev)
{
	ถ้า (!dev || cppc_dev)
		คืน;

	cppc_dev = dev;
}

static bool sbi_cppc_is_reserved(unsigned long reg)
{
	ถ้า ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		คืน true;

	คืน false;
}

static bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	คืน true;
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
		คืน false;
	}

	คืน true;
}

จำนวนเต็ม sbi_cppc_probe(unsigned long reg)
{
	ถ้า (!cppc_dev || !cppc_dev->cppc_probe)
		คืน SBI_EFAIL;

	/* Check whether register is reserved */
	ถ้า (sbi_cppc_is_reserved(reg))
		คืน SBI_ERR_INVALID_PARAM;

	คืน cppc_dev->cppc_probe(reg);
}

จำนวนเต็ม sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	จำนวนเต็ม ret;

	ถ้า (!cppc_dev || !cppc_dev->cppc_read)
		คืน SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ถ้า (ret <= 0)
		คืน SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	ถ้า (!sbi_cppc_readable(reg))
		คืน SBI_ERR_DENIED;

	คืน cppc_dev->cppc_read(reg, val);
}

จำนวนเต็ม sbi_cppc_write(unsigned long reg, uint64_t val)
{
	จำนวนเต็ม ret;

	ถ้า (!cppc_dev || !cppc_dev->cppc_write)
		คืน SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ถ้า (ret <= 0)
		คืน SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	ถ้า (!sbi_cppc_writable(reg))
		คืน SBI_ERR_DENIED;

	คืน cppc_dev->cppc_write(reg, val);
}
