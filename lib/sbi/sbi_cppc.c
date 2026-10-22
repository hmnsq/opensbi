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
	प्रत्यागम cppc_dev;
}

शून्य sbi_cppc_set_device(ध्रुव संरचना sbi_cppc_device *dev)
{
	यदि (!dev || cppc_dev)
		प्रत्यागम;

	cppc_dev = dev;
}

स्थिर bool sbi_cppc_is_reserved(अचिन्हित दीर्घ reg)
{
	यदि ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		प्रत्यागम true;

	प्रत्यागम false;
}

स्थिर bool sbi_cppc_readable(अचिन्हित दीर्घ reg)
{
	/* there are no write-only cppc registers currently */
	प्रत्यागम true;
}

स्थिर bool sbi_cppc_writable(अचिन्हित दीर्घ reg)
{
	विकल्प (reg) {
	पक्ष SBI_CPPC_HIGHEST_PERF:
	पक्ष SBI_CPPC_NOMINAL_PERF:
	पक्ष SBI_CPPC_निचला_NON_LINEAR_PERF:
	पक्ष SBI_CPPC_LOWEST_PERF:
	पक्ष SBI_CPPC_GUARANTEED_PERF:
	पक्ष SBI_CPPC_CTR_WRAP_TIME:
	पक्ष SBI_CPPC_REFERENCE_CTR:
	पक्ष SBI_CPPC_DELIVERED_CTR:
	पक्ष SBI_CPPC_REFERENCE_PERF:
	पक्ष SBI_CPPC_LOWEST_FREQ:
	पक्ष SBI_CPPC_NOMINAL_FREQ:
	पक्ष SBI_CPPC_TRANSITION_LATENCY:
		प्रत्यागम false;
	}

	प्रत्यागम true;
}

पूर्णाङ्क sbi_cppc_probe(अचिन्हित दीर्घ reg)
{
	यदि (!cppc_dev || !cppc_dev->cppc_probe)
		प्रत्यागम SBI_EFAIL;

	/* Check whether register is reserved */
	यदि (sbi_cppc_is_reserved(reg))
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	प्रत्यागम cppc_dev->cppc_probe(reg);
}

पूर्णाङ्क sbi_cppc_read(अचिन्हित दीर्घ reg, uint64_t *val)
{
	पूर्णाङ्क ret;

	यदि (!cppc_dev || !cppc_dev->cppc_read)
		प्रत्यागम SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	यदि (ret <= 0)
		प्रत्यागम SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	यदि (!sbi_cppc_readable(reg))
		प्रत्यागम SBI_ERR_DENIED;

	प्रत्यागम cppc_dev->cppc_read(reg, val);
}

पूर्णाङ्क sbi_cppc_write(अचिन्हित दीर्घ reg, uint64_t val)
{
	पूर्णाङ्क ret;

	यदि (!cppc_dev || !cppc_dev->cppc_write)
		प्रत्यागम SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	यदि (ret <= 0)
		प्रत्यागम SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	यदि (!sbi_cppc_writable(reg))
		प्रत्यागम SBI_ERR_DENIED;

	प्रत्यागम cppc_dev->cppc_write(reg, val);
}
