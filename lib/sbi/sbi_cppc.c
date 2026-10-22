/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_cppc.h>

स्थिर स्थिरांक संरचना sbi_cppc_device *cppc_dev = रिक्त;

स्थिरांक संरचना sbi_cppc_device *sbi_cppc_get_device(शून्य)
{
	परत cppc_dev;
}

शून्य sbi_cppc_set_device(स्थिरांक संरचना sbi_cppc_device *dev)
{
	जर (!dev || cppc_dev)
		परत;

	cppc_dev = dev;
}

स्थिर bool sbi_cppc_is_reserved(अचिन्हित दीर्घ reg)
{
	जर ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		परत true;

	परत false;
}

स्थिर bool sbi_cppc_readable(अचिन्हित दीर्घ reg)
{
	/* there are no write-only cppc registers currently */
	परत true;
}

स्थिर bool sbi_cppc_writable(अचिन्हित दीर्घ reg)
{
	निवड (reg) {
	स्थिती SBI_CPPC_HIGHEST_PERF:
	स्थिती SBI_CPPC_NOMINAL_PERF:
	स्थिती SBI_CPPC_खालचा_NON_LINEAR_PERF:
	स्थिती SBI_CPPC_LOWEST_PERF:
	स्थिती SBI_CPPC_GUARANTEED_PERF:
	स्थिती SBI_CPPC_CTR_WRAP_TIME:
	स्थिती SBI_CPPC_REFERENCE_CTR:
	स्थिती SBI_CPPC_DELIVERED_CTR:
	स्थिती SBI_CPPC_REFERENCE_PERF:
	स्थिती SBI_CPPC_LOWEST_FREQ:
	स्थिती SBI_CPPC_NOMINAL_FREQ:
	स्थिती SBI_CPPC_TRANSITION_LATENCY:
		परत false;
	}

	परत true;
}

पूर्णांक sbi_cppc_probe(अचिन्हित दीर्घ reg)
{
	जर (!cppc_dev || !cppc_dev->cppc_probe)
		परत SBI_EFAIL;

	/* Check whether register is reserved */
	जर (sbi_cppc_is_reserved(reg))
		परत SBI_ERR_INVALID_PARAM;

	परत cppc_dev->cppc_probe(reg);
}

पूर्णांक sbi_cppc_read(अचिन्हित दीर्घ reg, uint64_t *val)
{
	पूर्णांक ret;

	जर (!cppc_dev || !cppc_dev->cppc_read)
		परत SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	जर (ret <= 0)
		परत SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	जर (!sbi_cppc_readable(reg))
		परत SBI_ERR_DENIED;

	परत cppc_dev->cppc_read(reg, val);
}

पूर्णांक sbi_cppc_write(अचिन्हित दीर्घ reg, uint64_t val)
{
	पूर्णांक ret;

	जर (!cppc_dev || !cppc_dev->cppc_write)
		परत SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	जर (ret <= 0)
		परत SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	जर (!sbi_cppc_writable(reg))
		परत SBI_ERR_DENIED;

	परत cppc_dev->cppc_write(reg, val);
}
