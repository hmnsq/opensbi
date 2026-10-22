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
	ආපසු cppc_dev;
}

void sbi_cppc_set_device(const sa_nracanaa sbi_cppc_device *dev)
{
	නම් (!dev || cppc_dev)
		ආපසු;

	cppc_dev = dev;
}

static bool sbi_cppc_is_reserved(unsigned long reg)
{
	නම් ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		ආපසු true;

	ආපසු false;
}

static bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	ආපසු true;
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
		ආපසු false;
	}

	ආපසු true;
}

පූර්ණ_සංඛ්‍යා sbi_cppc_probe(unsigned long reg)
{
	නම් (!cppc_dev || !cppc_dev->cppc_probe)
		ආපසු SBI_EFAIL;

	/* Check whether register is reserved */
	නම් (sbi_cppc_is_reserved(reg))
		ආපසු SBI_ERR_INVALID_PARAM;

	ආපසු cppc_dev->cppc_probe(reg);
}

පූර්ණ_සංඛ්‍යා sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	පූර්ණ_සංඛ්‍යා ret;

	නම් (!cppc_dev || !cppc_dev->cppc_read)
		ආපසු SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	නම් (ret <= 0)
		ආපසු SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	නම් (!sbi_cppc_readable(reg))
		ආපසු SBI_ERR_DENIED;

	ආපසු cppc_dev->cppc_read(reg, val);
}

පූර්ණ_සංඛ්‍යා sbi_cppc_write(unsigned long reg, uint64_t val)
{
	පූර්ණ_සංඛ්‍යා ret;

	නම් (!cppc_dev || !cppc_dev->cppc_write)
		ආපසු SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	නම් (ret <= 0)
		ආපසු SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	නම් (!sbi_cppc_writable(reg))
		ආපසු SBI_ERR_DENIED;

	ආපසු cppc_dev->cppc_write(reg, val);
}
