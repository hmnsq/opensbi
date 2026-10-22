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
	ສົ່ງຄືນ cppc_dev;
}

void sbi_cppc_set_device(const sa_nracanaa sbi_cppc_device *dev)
{
	ຖ້າ (!dev || cppc_dev)
		ສົ່ງຄືນ;

	cppc_dev = dev;
}

static bool sbi_cppc_is_reserved(unsigned long reg)
{
	ຖ້າ ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		ສົ່ງຄືນ true;

	ສົ່ງຄືນ false;
}

static bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	ສົ່ງຄືນ true;
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
		ສົ່ງຄືນ false;
	}

	ສົ່ງຄືນ true;
}

ຈຳນວນເຕັມ sbi_cppc_probe(unsigned long reg)
{
	ຖ້າ (!cppc_dev || !cppc_dev->cppc_probe)
		ສົ່ງຄືນ SBI_EFAIL;

	/* Check whether register is reserved */
	ຖ້າ (sbi_cppc_is_reserved(reg))
		ສົ່ງຄືນ SBI_ERR_INVALID_PARAM;

	ສົ່ງຄືນ cppc_dev->cppc_probe(reg);
}

ຈຳນວນເຕັມ sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	ຈຳນວນເຕັມ ret;

	ຖ້າ (!cppc_dev || !cppc_dev->cppc_read)
		ສົ່ງຄືນ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ຖ້າ (ret <= 0)
		ສົ່ງຄືນ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	ຖ້າ (!sbi_cppc_readable(reg))
		ສົ່ງຄືນ SBI_ERR_DENIED;

	ສົ່ງຄືນ cppc_dev->cppc_read(reg, val);
}

ຈຳນວນເຕັມ sbi_cppc_write(unsigned long reg, uint64_t val)
{
	ຈຳນວນເຕັມ ret;

	ຖ້າ (!cppc_dev || !cppc_dev->cppc_write)
		ສົ່ງຄືນ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ຖ້າ (ret <= 0)
		ສົ່ງຄືນ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	ຖ້າ (!sbi_cppc_writable(reg))
		ສົ່ງຄືນ SBI_ERR_DENIED;

	ສົ່ງຄືນ cppc_dev->cppc_write(reg, val);
}
