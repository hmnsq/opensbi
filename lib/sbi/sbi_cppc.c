/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_cppc.h>

స్థిర స్థిరరాశి నిర్మాణం sbi_cppc_device *cppc_dev = ఖాళీ;

స్థిరరాశి నిర్మాణం sbi_cppc_device *sbi_cppc_get_device(శూన్య)
{
	తిరిగి cppc_dev;
}

శూన్య sbi_cppc_సమితి_device(స్థిరరాశి నిర్మాణం sbi_cppc_device *dev)
{
	ఒకవేళ (!dev || cppc_dev)
		తిరిగి;

	cppc_dev = dev;
}

స్థిర bool sbi_cppc_is_reserved(గుర్తులేని దీర్ఘ reg)
{
	ఒకవేళ ((reg > SBI_CPPC_ACPI_చివరి && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_చివరి)
		తిరిగి true;

	తిరిగి false;
}

స్థిర bool sbi_cppc_readable(గుర్తులేని దీర్ఘ reg)
{
	/* there are no write-only cppc registers currently */
	తిరిగి true;
}

స్థిర bool sbi_cppc_writable(గుర్తులేని దీర్ఘ reg)
{
	ఎంపిక (reg) {
	స్థితి SBI_CPPC_HIGHEST_PERF:
	స్థితి SBI_CPPC_NOMINAL_PERF:
	స్థితి SBI_CPPC_దిగువ_NON_రేఖీయ_PERF:
	స్థితి SBI_CPPC_అత్యల్ప_PERF:
	స్థితి SBI_CPPC_GUARANTEED_PERF:
	స్థితి SBI_CPPC_CTR_WRAP_సమయం:
	స్థితి SBI_CPPC_REFERENCE_CTR:
	స్థితి SBI_CPPC_DELIVERED_CTR:
	స్థితి SBI_CPPC_REFERENCE_PERF:
	స్థితి SBI_CPPC_అత్యల్ప_FREQ:
	స్థితి SBI_CPPC_NOMINAL_FREQ:
	స్థితి SBI_CPPC_TRANSITION_LATENCY:
		తిరిగి false;
	}

	తిరిగి true;
}

పూర్ణాంకం sbi_cppc_probe(గుర్తులేని దీర్ఘ reg)
{
	ఒకవేళ (!cppc_dev || !cppc_dev->cppc_probe)
		తిరిగి SBI_EFAIL;

	/* Check whether register is reserved */
	ఒకవేళ (sbi_cppc_is_reserved(reg))
		తిరిగి SBI_ERR_INVALID_PARAM;

	తిరిగి cppc_dev->cppc_probe(reg);
}

పూర్ణాంకం sbi_cppc_read(గుర్తులేని దీర్ఘ reg, uint64_t *val)
{
	పూర్ణాంకం ret;

	ఒకవేళ (!cppc_dev || !cppc_dev->cppc_read)
		తిరిగి SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ఒకవేళ (ret <= 0)
		తిరిగి SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	ఒకవేళ (!sbi_cppc_readable(reg))
		తిరిగి SBI_ERR_DENIED;

	తిరిగి cppc_dev->cppc_read(reg, val);
}

పూర్ణాంకం sbi_cppc_write(గుర్తులేని దీర్ఘ reg, uint64_t val)
{
	పూర్ణాంకం ret;

	ఒకవేళ (!cppc_dev || !cppc_dev->cppc_write)
		తిరిగి SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ఒకవేళ (ret <= 0)
		తిరిగి SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	ఒకవేళ (!sbi_cppc_writable(reg))
		తిరిగి SBI_ERR_DENIED;

	తిరిగి cppc_dev->cppc_write(reg, val);
}
