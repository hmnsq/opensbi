/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_cppc.h>

स्थिर अचर संरचना sbi_cppc_device *cppc_dev = रिक्त;

अचर संरचना sbi_cppc_device *sbi_cppc_get_device(शून्य)
{
	फर्काऊ cppc_dev;
}

शून्य sbi_cppc_set_device(अचर संरचना sbi_cppc_device *dev)
{
	यदि (!dev || cppc_dev)
		फर्काऊ;

	cppc_dev = dev;
}

स्थिर bool sbi_cppc_is_reserved(अचिन्हित दीर्घ reg)
{
	यदि ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		फर्काऊ true;

	फर्काऊ false;
}

स्थिर bool sbi_cppc_readable(अचिन्हित दीर्घ reg)
{
	/* there are no write-only cppc registers currently */
	फर्काऊ true;
}

स्थिर bool sbi_cppc_writable(अचिन्हित दीर्घ reg)
{
	विकल्प (reg) {
	पक्ष SBI_CPPC_HIGHEST_PERF:
	पक्ष SBI_CPPC_NOMINAL_PERF:
	पक्ष SBI_CPPC_तल्लो_NON_LINEAR_PERF:
	पक्ष SBI_CPPC_LOWEST_PERF:
	पक्ष SBI_CPPC_GUARANTEED_PERF:
	पक्ष SBI_CPPC_CTR_WRAP_TIME:
	पक्ष SBI_CPPC_REFERENCE_CTR:
	पक्ष SBI_CPPC_DELIVERED_CTR:
	पक्ष SBI_CPPC_REFERENCE_PERF:
	पक्ष SBI_CPPC_LOWEST_FREQ:
	पक्ष SBI_CPPC_NOMINAL_FREQ:
	पक्ष SBI_CPPC_TRANSITION_LATENCY:
		फर्काऊ false;
	}

	फर्काऊ true;
}

पूर्णाङ्क sbi_cppc_probe(अचिन्हित दीर्घ reg)
{
	यदि (!cppc_dev || !cppc_dev->cppc_probe)
		फर्काऊ SBI_EFAIL;

	/* Check whether register is reserved */
	यदि (sbi_cppc_is_reserved(reg))
		फर्काऊ SBI_ERR_INVALID_PARAM;

	फर्काऊ cppc_dev->cppc_probe(reg);
}

पूर्णाङ्क sbi_cppc_read(अचिन्हित दीर्घ reg, uint64_t *val)
{
	पूर्णाङ्क ret;

	यदि (!cppc_dev || !cppc_dev->cppc_read)
		फर्काऊ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	यदि (ret <= 0)
		फर्काऊ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	यदि (!sbi_cppc_readable(reg))
		फर्काऊ SBI_ERR_DENIED;

	फर्काऊ cppc_dev->cppc_read(reg, val);
}

पूर्णाङ्क sbi_cppc_write(अचिन्हित दीर्घ reg, uint64_t val)
{
	पूर्णाङ्क ret;

	यदि (!cppc_dev || !cppc_dev->cppc_write)
		फर्काऊ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	यदि (ret <= 0)
		फर्काऊ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	यदि (!sbi_cppc_writable(reg))
		फर्काऊ SBI_ERR_DENIED;

	फर्काऊ cppc_dev->cppc_write(reg, val);
}
