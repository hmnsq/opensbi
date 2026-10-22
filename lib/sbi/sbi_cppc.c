/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_cppc.h>

स्थिर ध्रुव संरचना sbi_cppc_device *cppc_dev = रिक्त;

ध्रुव संरचना sbi_cppc_device *sbi_cppc_get_device(शून्य)
{
	पटिनिवत्त cppc_dev;
}

शून्य sbi_cppc_set_device(ध्रुव संरचना sbi_cppc_device *dev)
{
	सचे (!dev || cppc_dev)
		पटिनिवत्त;

	cppc_dev = dev;
}

स्थिर bool sbi_cppc_is_reserved(unsigned long reg)
{
	सचे ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		पटिनिवत्त true;

	पटिनिवत्त false;
}

स्थिर bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	पटिनिवत्त true;
}

स्थिर bool sbi_cppc_writable(unsigned long reg)
{
	विकल्प (reg) {
	पक्ष SBI_CPPC_HIGHEST_PERF:
	पक्ष SBI_CPPC_NOMINAL_PERF:
	पक्ष SBI_CPPC_नीचः_NON_LINEAR_PERF:
	पक्ष SBI_CPPC_LOWEST_PERF:
	पक्ष SBI_CPPC_GUARANTEED_PERF:
	पक्ष SBI_CPPC_CTR_WRAP_TIME:
	पक्ष SBI_CPPC_REFERENCE_CTR:
	पक्ष SBI_CPPC_DELIVERED_CTR:
	पक्ष SBI_CPPC_REFERENCE_PERF:
	पक्ष SBI_CPPC_LOWEST_FREQ:
	पक्ष SBI_CPPC_NOMINAL_FREQ:
	पक्ष SBI_CPPC_TRANSITION_LATENCY:
		पटिनिवत्त false;
	}

	पटिनिवत्त true;
}

पूर्णाङ्क sbi_cppc_probe(unsigned long reg)
{
	सचे (!cppc_dev || !cppc_dev->cppc_probe)
		पटिनिवत्त SBI_EFAIL;

	/* Check whether register is reserved */
	सचे (sbi_cppc_is_reserved(reg))
		पटिनिवत्त SBI_ERR_INVALID_PARAM;

	पटिनिवत्त cppc_dev->cppc_probe(reg);
}

पूर्णाङ्क sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	पूर्णाङ्क ret;

	सचे (!cppc_dev || !cppc_dev->cppc_read)
		पटिनिवत्त SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	सचे (ret <= 0)
		पटिनिवत्त SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	सचे (!sbi_cppc_readable(reg))
		पटिनिवत्त SBI_ERR_DENIED;

	पटिनिवत्त cppc_dev->cppc_read(reg, val);
}

पूर्णाङ्क sbi_cppc_write(unsigned long reg, uint64_t val)
{
	पूर्णाङ्क ret;

	सचे (!cppc_dev || !cppc_dev->cppc_write)
		पटिनिवत्त SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	सचे (ret <= 0)
		पटिनिवत्त SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	सचे (!sbi_cppc_writable(reg))
		पटिनिवत्त SBI_ERR_DENIED;

	पटिनिवत्त cppc_dev->cppc_write(reg, val);
}
