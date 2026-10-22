/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_cppc.h>

நிலையான மாறிலி கட்டமைப்பு sbi_cppc_device *cppc_dev = வெற்று;

மாறிலி கட்டமைப்பு sbi_cppc_device *sbi_cppc_get_device(வெறுமை)
{
	திரும்பு cppc_dev;
}

வெறுமை sbi_cppc_set_device(மாறிலி கட்டமைப்பு sbi_cppc_device *dev)
{
	எனில் (!dev || cppc_dev)
		திரும்பு;

	cppc_dev = dev;
}

நிலையான bool sbi_cppc_is_reserved(unsigned long reg)
{
	எனில் ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		திரும்பு true;

	திரும்பு false;
}

நிலையான bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	திரும்பு true;
}

நிலையான bool sbi_cppc_writable(unsigned long reg)
{
	தேர்வு (reg) {
	நிலை SBI_CPPC_HIGHEST_PERF:
	நிலை SBI_CPPC_NOMINAL_PERF:
	நிலை SBI_CPPC_கீழ்_NON_LINEAR_PERF:
	நிலை SBI_CPPC_LOWEST_PERF:
	நிலை SBI_CPPC_GUARANTEED_PERF:
	நிலை SBI_CPPC_CTR_WRAP_TIME:
	நிலை SBI_CPPC_REFERENCE_CTR:
	நிலை SBI_CPPC_DELIVERED_CTR:
	நிலை SBI_CPPC_REFERENCE_PERF:
	நிலை SBI_CPPC_LOWEST_FREQ:
	நிலை SBI_CPPC_NOMINAL_FREQ:
	நிலை SBI_CPPC_TRANSITION_LATENCY:
		திரும்பு false;
	}

	திரும்பு true;
}

முழு sbi_cppc_probe(unsigned long reg)
{
	எனில் (!cppc_dev || !cppc_dev->cppc_probe)
		திரும்பு SBI_EFAIL;

	/* Check whether register is reserved */
	எனில் (sbi_cppc_is_reserved(reg))
		திரும்பு SBI_ERR_INVALID_PARAM;

	திரும்பு cppc_dev->cppc_probe(reg);
}

முழு sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	முழு ret;

	எனில் (!cppc_dev || !cppc_dev->cppc_read)
		திரும்பு SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	எனில் (ret <= 0)
		திரும்பு SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	எனில் (!sbi_cppc_readable(reg))
		திரும்பு SBI_ERR_DENIED;

	திரும்பு cppc_dev->cppc_read(reg, val);
}

முழு sbi_cppc_write(unsigned long reg, uint64_t val)
{
	முழு ret;

	எனில் (!cppc_dev || !cppc_dev->cppc_write)
		திரும்பு SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	எனில் (ret <= 0)
		திரும்பு SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	எனில் (!sbi_cppc_writable(reg))
		திரும்பு SBI_ERR_DENIED;

	திரும்பு cppc_dev->cppc_write(reg, val);
}
