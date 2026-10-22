/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_cppc.h>

സ്ഥിതം സ്ഥിരം ഘടന sbi_cppc_device *cppc_dev = ശൂന്യ;

സ്ഥിരം ഘടന sbi_cppc_device *sbi_cppc_get_device(ശൂന്യം)
{
	തിരികെ cppc_dev;
}

ശൂന്യം sbi_cppc_set_device(സ്ഥിരം ഘടന sbi_cppc_device *dev)
{
	എങ്കിൽ (!dev || cppc_dev)
		തിരികെ;

	cppc_dev = dev;
}

സ്ഥിതം bool sbi_cppc_is_reserved(unsigned long reg)
{
	എങ്കിൽ ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		തിരികെ true;

	തിരികെ false;
}

സ്ഥിതം bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	തിരികെ true;
}

സ്ഥിതം bool sbi_cppc_writable(unsigned long reg)
{
	തിരഞ്ഞെടുപ്പ് (reg) {
	അവസ്ഥ SBI_CPPC_HIGHEST_PERF:
	അവസ്ഥ SBI_CPPC_NOMINAL_PERF:
	അവസ്ഥ SBI_CPPC_താഴെയുള്ള_NON_LINEAR_PERF:
	അവസ്ഥ SBI_CPPC_LOWEST_PERF:
	അവസ്ഥ SBI_CPPC_GUARANTEED_PERF:
	അവസ്ഥ SBI_CPPC_CTR_WRAP_TIME:
	അവസ്ഥ SBI_CPPC_REFERENCE_CTR:
	അവസ്ഥ SBI_CPPC_DELIVERED_CTR:
	അവസ്ഥ SBI_CPPC_REFERENCE_PERF:
	അവസ്ഥ SBI_CPPC_LOWEST_FREQ:
	അവസ്ഥ SBI_CPPC_NOMINAL_FREQ:
	അവസ്ഥ SBI_CPPC_TRANSITION_LATENCY:
		തിരികെ false;
	}

	തിരികെ true;
}

പൂർണ്ണസംഖ്യ sbi_cppc_probe(unsigned long reg)
{
	എങ്കിൽ (!cppc_dev || !cppc_dev->cppc_probe)
		തിരികെ SBI_EFAIL;

	/* Check whether register is reserved */
	എങ്കിൽ (sbi_cppc_is_reserved(reg))
		തിരികെ SBI_ERR_INVALID_PARAM;

	തിരികെ cppc_dev->cppc_probe(reg);
}

പൂർണ്ണസംഖ്യ sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	പൂർണ്ണസംഖ്യ ret;

	എങ്കിൽ (!cppc_dev || !cppc_dev->cppc_read)
		തിരികെ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	എങ്കിൽ (ret <= 0)
		തിരികെ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	എങ്കിൽ (!sbi_cppc_readable(reg))
		തിരികെ SBI_ERR_DENIED;

	തിരികെ cppc_dev->cppc_read(reg, val);
}

പൂർണ്ണസംഖ്യ sbi_cppc_write(unsigned long reg, uint64_t val)
{
	പൂർണ്ണസംഖ്യ ret;

	എങ്കിൽ (!cppc_dev || !cppc_dev->cppc_write)
		തിരികെ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	എങ്കിൽ (ret <= 0)
		തിരികെ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	എങ്കിൽ (!sbi_cppc_writable(reg))
		തിരികെ SBI_ERR_DENIED;

	തിരികെ cppc_dev->cppc_write(reg, val);
}
