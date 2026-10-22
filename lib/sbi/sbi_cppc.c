/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_cppc.h>

अटल स्थिर काष्ठा sbi_cppc_device *cppc_dev = शून्य;

स्थिर काष्ठा sbi_cppc_device *sbi_cppc_get_device(व्योम)
{
	वापस cppc_dev;
}

व्योम sbi_cppc_set_device(स्थिर काष्ठा sbi_cppc_device *dev)
{
	अगर (!dev || cppc_dev)
		वापस;

	cppc_dev = dev;
}

अटल bool sbi_cppc_is_reserved(अचिन्हित दीर्घ reg)
{
	अगर ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		वापस true;

	वापस false;
}

अटल bool sbi_cppc_readable(अचिन्हित दीर्घ reg)
{
	/* there are no write-only cppc registers currently */
	वापस true;
}

अटल bool sbi_cppc_writable(अचिन्हित दीर्घ reg)
{
	चयन (reg) {
	हाल SBI_CPPC_HIGHEST_PERF:
	हाल SBI_CPPC_NOMINAL_PERF:
	हाल SBI_CPPC_निचला_NON_LINEAR_PERF:
	हाल SBI_CPPC_LOWEST_PERF:
	हाल SBI_CPPC_GUARANTEED_PERF:
	हाल SBI_CPPC_CTR_WRAP_TIME:
	हाल SBI_CPPC_REFERENCE_CTR:
	हाल SBI_CPPC_DELIVERED_CTR:
	हाल SBI_CPPC_REFERENCE_PERF:
	हाल SBI_CPPC_LOWEST_FREQ:
	हाल SBI_CPPC_NOMINAL_FREQ:
	हाल SBI_CPPC_TRANSITION_LATENCY:
		वापस false;
	}

	वापस true;
}

पूर्णांक sbi_cppc_probe(अचिन्हित दीर्घ reg)
{
	अगर (!cppc_dev || !cppc_dev->cppc_probe)
		वापस SBI_EFAIL;

	/* Check whether register is reserved */
	अगर (sbi_cppc_is_reserved(reg))
		वापस SBI_ERR_INVALID_PARAM;

	वापस cppc_dev->cppc_probe(reg);
}

पूर्णांक sbi_cppc_read(अचिन्हित दीर्घ reg, uint64_t *val)
{
	पूर्णांक ret;

	अगर (!cppc_dev || !cppc_dev->cppc_read)
		वापस SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	अगर (ret <= 0)
		वापस SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	अगर (!sbi_cppc_readable(reg))
		वापस SBI_ERR_DENIED;

	वापस cppc_dev->cppc_read(reg, val);
}

पूर्णांक sbi_cppc_write(अचिन्हित दीर्घ reg, uint64_t val)
{
	पूर्णांक ret;

	अगर (!cppc_dev || !cppc_dev->cppc_write)
		वापस SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	अगर (ret <= 0)
		वापस SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	अगर (!sbi_cppc_writable(reg))
		वापस SBI_ERR_DENIED;

	वापस cppc_dev->cppc_write(reg, val);
}
