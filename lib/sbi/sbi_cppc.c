/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_cppc.h>

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_cppc_device *cppc_dev = ਨਿਰਾ;

ਅਚਲ ਬਣਤਰ sbi_cppc_device *sbi_cppc_get_device(ਖਾਲੀ)
{
	ਵਾਪਸ cppc_dev;
}

ਖਾਲੀ sbi_cppc_ਸੈੱਟ_device(ਅਚਲ ਬਣਤਰ sbi_cppc_device *dev)
{
	ਜੇ (!dev || cppc_dev)
		ਵਾਪਸ;

	cppc_dev = dev;
}

ਸਥਿਰ bool sbi_cppc_is_reserved(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg)
{
	ਜੇ ((reg > SBI_CPPC_ACPI_ਆਖਰੀ && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_ਆਖਰੀ)
		ਵਾਪਸ true;

	ਵਾਪਸ false;
}

ਸਥਿਰ bool sbi_cppc_readable(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg)
{
	/* there are no write-only cppc registers currently */
	ਵਾਪਸ true;
}

ਸਥਿਰ bool sbi_cppc_writable(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg)
{
	ਚੋਣ (reg) {
	ਹਾਲਤ SBI_CPPC_HIGHEST_PERF:
	ਹਾਲਤ SBI_CPPC_NOMINAL_PERF:
	ਹਾਲਤ SBI_CPPC_ਨੀਵਾਂ_NON_ਰੇਖਿਕ_PERF:
	ਹਾਲਤ SBI_CPPC_ਸਭ_ਤੋਂ_ਨੀਵਾਂ_PERF:
	ਹਾਲਤ SBI_CPPC_GUARANTEED_PERF:
	ਹਾਲਤ SBI_CPPC_CTR_WRAP_ਸਮਾਂ:
	ਹਾਲਤ SBI_CPPC_REFERENCE_CTR:
	ਹਾਲਤ SBI_CPPC_DELIVERED_CTR:
	ਹਾਲਤ SBI_CPPC_REFERENCE_PERF:
	ਹਾਲਤ SBI_CPPC_ਸਭ_ਤੋਂ_ਨੀਵਾਂ_FREQ:
	ਹਾਲਤ SBI_CPPC_NOMINAL_FREQ:
	ਹਾਲਤ SBI_CPPC_TRANSITION_LATENCY:
		ਵਾਪਸ false;
	}

	ਵਾਪਸ true;
}

ਪੂਰਨ sbi_cppc_probe(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg)
{
	ਜੇ (!cppc_dev || !cppc_dev->cppc_probe)
		ਵਾਪਸ SBI_EFAIL;

	/* Check whether register is reserved */
	ਜੇ (sbi_cppc_is_reserved(reg))
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	ਵਾਪਸ cppc_dev->cppc_probe(reg);
}

ਪੂਰਨ sbi_cppc_read(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg, uint64_t *val)
{
	ਪੂਰਨ ret;

	ਜੇ (!cppc_dev || !cppc_dev->cppc_read)
		ਵਾਪਸ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ਜੇ (ret <= 0)
		ਵਾਪਸ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	ਜੇ (!sbi_cppc_readable(reg))
		ਵਾਪਸ SBI_ERR_DENIED;

	ਵਾਪਸ cppc_dev->cppc_read(reg, val);
}

ਪੂਰਨ sbi_cppc_write(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg, uint64_t val)
{
	ਪੂਰਨ ret;

	ਜੇ (!cppc_dev || !cppc_dev->cppc_write)
		ਵਾਪਸ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ਜੇ (ret <= 0)
		ਵਾਪਸ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	ਜੇ (!sbi_cppc_writable(reg))
		ਵਾਪਸ SBI_ERR_DENIED;

	ਵਾਪਸ cppc_dev->cppc_write(reg, val);
}
