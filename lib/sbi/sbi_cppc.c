/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_cppc.h>

ساکن ثابت ساخت sbi_cppc_device *cppc_dev = تہی;

ثابت ساخت sbi_cppc_device *sbi_cppc_get_device(خالی)
{
	واپس cppc_dev;
}

خالی sbi_cppc_سیٹ_device(ثابت ساخت sbi_cppc_device *dev)
{
	اگر (!dev || cppc_dev)
		واپس;

	cppc_dev = dev;
}

ساکن bool sbi_cppc_is_reserved(غیرعلامتی طویل reg)
{
	اگر ((reg > SBI_CPPC_ACPI_آخری && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_آخری)
		واپس true;

	واپس false;
}

ساکن bool sbi_cppc_readable(غیرعلامتی طویل reg)
{
	/* there are no write-only cppc registers currently */
	واپس true;
}

ساکن bool sbi_cppc_writable(غیرعلامتی طویل reg)
{
	انتخاب (reg) {
	حالت SBI_CPPC_HIGHEST_PERF:
	حالت SBI_CPPC_NOMINAL_PERF:
	حالت SBI_CPPC_کم_NON_خطی_PERF:
	حالت SBI_CPPC_سب_سے_نیچا_PERF:
	حالت SBI_CPPC_GUARANTEED_PERF:
	حالت SBI_CPPC_CTR_WRAP_وقت:
	حالت SBI_CPPC_REFERENCE_CTR:
	حالت SBI_CPPC_DELIVERED_CTR:
	حالت SBI_CPPC_REFERENCE_PERF:
	حالت SBI_CPPC_سب_سے_نیچا_FREQ:
	حالت SBI_CPPC_NOMINAL_FREQ:
	حالت SBI_CPPC_TRANSITION_LATENCY:
		واپس false;
	}

	واپس true;
}

صحیح sbi_cppc_probe(غیرعلامتی طویل reg)
{
	اگر (!cppc_dev || !cppc_dev->cppc_probe)
		واپس SBI_EFAIL;

	/* Check whether register is reserved */
	اگر (sbi_cppc_is_reserved(reg))
		واپس SBI_ERR_INVALID_PARAM;

	واپس cppc_dev->cppc_probe(reg);
}

صحیح sbi_cppc_read(غیرعلامتی طویل reg, uint64_t *val)
{
	صحیح ret;

	اگر (!cppc_dev || !cppc_dev->cppc_read)
		واپس SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	اگر (ret <= 0)
		واپس SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	اگر (!sbi_cppc_readable(reg))
		واپس SBI_ERR_DENIED;

	واپس cppc_dev->cppc_read(reg, val);
}

صحیح sbi_cppc_write(غیرعلامتی طویل reg, uint64_t val)
{
	صحیح ret;

	اگر (!cppc_dev || !cppc_dev->cppc_write)
		واپس SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	اگر (ret <= 0)
		واپس SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	اگر (!sbi_cppc_writable(reg))
		واپس SBI_ERR_DENIED;

	واپس cppc_dev->cppc_write(reg, val);
}
