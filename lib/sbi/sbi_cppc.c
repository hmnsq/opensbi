/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_cppc.h>

ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_cppc_device *cppc_dev = ଖାଲି;

ଅଚଳ ସଂରଚନା sbi_cppc_device *sbi_cppc_get_device(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ cppc_dev;
}

ଶୂନ୍ୟ sbi_cppc_set_device(ଅଚଳ ସଂରଚନା sbi_cppc_device *dev)
{
	ଯଦି (!dev || cppc_dev)
		ଫେରନ୍ତୁ;

	cppc_dev = dev;
}

ସ୍ଥିର bool sbi_cppc_is_reserved(unsigned long reg)
{
	ଯଦି ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		ଫେରନ୍ତୁ true;

	ଫେରନ୍ତୁ false;
}

ସ୍ଥିର bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	ଫେରନ୍ତୁ true;
}

ସ୍ଥିର bool sbi_cppc_writable(unsigned long reg)
{
	ବାଛ (reg) {
	ସ୍ଥିତି SBI_CPPC_HIGHEST_PERF:
	ସ୍ଥିତି SBI_CPPC_NOMINAL_PERF:
	ସ୍ଥିତି SBI_CPPC_ତଳ_NON_LINEAR_PERF:
	ସ୍ଥିତି SBI_CPPC_LOWEST_PERF:
	ସ୍ଥିତି SBI_CPPC_GUARANTEED_PERF:
	ସ୍ଥିତି SBI_CPPC_CTR_WRAP_TIME:
	ସ୍ଥିତି SBI_CPPC_REFERENCE_CTR:
	ସ୍ଥିତି SBI_CPPC_DELIVERED_CTR:
	ସ୍ଥିତି SBI_CPPC_REFERENCE_PERF:
	ସ୍ଥିତି SBI_CPPC_LOWEST_FREQ:
	ସ୍ଥିତି SBI_CPPC_NOMINAL_FREQ:
	ସ୍ଥିତି SBI_CPPC_TRANSITION_LATENCY:
		ଫେରନ୍ତୁ false;
	}

	ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_cppc_probe(unsigned long reg)
{
	ଯଦି (!cppc_dev || !cppc_dev->cppc_probe)
		ଫେରନ୍ତୁ SBI_EFAIL;

	/* Check whether register is reserved */
	ଯଦି (sbi_cppc_is_reserved(reg))
		ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;

	ଫେରନ୍ତୁ cppc_dev->cppc_probe(reg);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ଯଦି (!cppc_dev || !cppc_dev->cppc_read)
		ଫେରନ୍ତୁ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ଯଦି (ret <= 0)
		ଫେରନ୍ତୁ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	ଯଦି (!sbi_cppc_readable(reg))
		ଫେରନ୍ତୁ SBI_ERR_DENIED;

	ଫେରନ୍ତୁ cppc_dev->cppc_read(reg, val);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_cppc_write(unsigned long reg, uint64_t val)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ଯଦି (!cppc_dev || !cppc_dev->cppc_write)
		ଫେରନ୍ତୁ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ଯଦି (ret <= 0)
		ଫେରନ୍ତୁ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	ଯଦି (!sbi_cppc_writable(reg))
		ଫେରନ୍ତୁ SBI_ERR_DENIED;

	ଫେରନ୍ତୁ cppc_dev->cppc_write(reg, val);
}
