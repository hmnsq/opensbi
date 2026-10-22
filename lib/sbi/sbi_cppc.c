/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_cppc.h>

ساكن ثابت بنية sbi_cppc_device *cppc_dev = عدم;

ثابت بنية sbi_cppc_device *sbi_cppc_get_device(فراغ)
{
	أرجع cppc_dev;
}

فراغ sbi_cppc_سيٽ_device(ثابت بنية sbi_cppc_device *dev)
{
	إذا (!dev || cppc_dev)
		أرجع;

	cppc_dev = dev;
}

ساكن bool sbi_cppc_is_reserved(بنانشان ڊگهو reg)
{
	إذا ((reg > SBI_CPPC_ACPI_آخري && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_آخري)
		أرجع true;

	أرجع false;
}

ساكن bool sbi_cppc_readable(بنانشان ڊگهو reg)
{
	/* there are no write-only cppc registers currently */
	أرجع true;
}

ساكن bool sbi_cppc_writable(بنانشان ڊگهو reg)
{
	بدل (reg) {
	حالة SBI_CPPC_HIGHEST_PERF:
	حالة SBI_CPPC_NOMINAL_PERF:
	حالة SBI_CPPC_هيٺيون_NON_لڪيري_PERF:
	حالة SBI_CPPC_سڀ_کان_هيٺ_PERF:
	حالة SBI_CPPC_GUARANTEED_PERF:
	حالة SBI_CPPC_CTR_WRAP_وقت:
	حالة SBI_CPPC_REFERENCE_CTR:
	حالة SBI_CPPC_DELIVERED_CTR:
	حالة SBI_CPPC_REFERENCE_PERF:
	حالة SBI_CPPC_سڀ_کان_هيٺ_FREQ:
	حالة SBI_CPPC_NOMINAL_FREQ:
	حالة SBI_CPPC_TRANSITION_LATENCY:
		أرجع false;
	}

	أرجع true;
}

صحيح sbi_cppc_probe(بنانشان ڊگهو reg)
{
	إذا (!cppc_dev || !cppc_dev->cppc_probe)
		أرجع SBI_EFAIL;

	/* Check whether register is reserved */
	إذا (sbi_cppc_is_reserved(reg))
		أرجع SBI_ERR_INVALID_PARAM;

	أرجع cppc_dev->cppc_probe(reg);
}

صحيح sbi_cppc_read(بنانشان ڊگهو reg, uint64_t *val)
{
	صحيح ret;

	إذا (!cppc_dev || !cppc_dev->cppc_read)
		أرجع SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	إذا (ret <= 0)
		أرجع SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	إذا (!sbi_cppc_readable(reg))
		أرجع SBI_ERR_DENIED;

	أرجع cppc_dev->cppc_read(reg, val);
}

صحيح sbi_cppc_write(بنانشان ڊگهو reg, uint64_t val)
{
	صحيح ret;

	إذا (!cppc_dev || !cppc_dev->cppc_write)
		أرجع SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	إذا (ret <= 0)
		أرجع SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	إذا (!sbi_cppc_writable(reg))
		أرجع SBI_ERR_DENIED;

	أرجع cppc_dev->cppc_write(reg, val);
}
