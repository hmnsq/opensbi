/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_cppc.h>

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sbi_cppc_device *cppc_dev = ರಿಕ್ತ;

ಧ್ರುವ ಸಂರಚನಾ sbi_cppc_device *sbi_cppc_get_device(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ cppc_dev;
}

ಶೂನ್ಯ sbi_cppc_set_device(ಧ್ರುವ ಸಂರಚನಾ sbi_cppc_device *dev)
{
	ಯದಿ (!dev || cppc_dev)
		ಪ್ರತ್ಯಾಗಮ;

	cppc_dev = dev;
}

ಸ್ಥಿರ bool sbi_cppc_is_reserved(unsigned long reg)
{
	ಯದಿ ((reg > SBI_CPPC_ACPI_LAST && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_LAST)
		ಪ್ರತ್ಯಾಗಮ true;

	ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ bool sbi_cppc_readable(unsigned long reg)
{
	/* there are no write-only cppc registers currently */
	ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ bool sbi_cppc_writable(unsigned long reg)
{
	ವಿಕಲ್ಪ (reg) {
	ಪಕ್ಷ SBI_CPPC_HIGHEST_PERF:
	ಪಕ್ಷ SBI_CPPC_NOMINAL_PERF:
	ಪಕ್ಷ SBI_CPPC_ಕೆಳಗಿನ_NON_LINEAR_PERF:
	ಪಕ್ಷ SBI_CPPC_LOWEST_PERF:
	ಪಕ್ಷ SBI_CPPC_GUARANTEED_PERF:
	ಪಕ್ಷ SBI_CPPC_CTR_WRAP_TIME:
	ಪಕ್ಷ SBI_CPPC_REFERENCE_CTR:
	ಪಕ್ಷ SBI_CPPC_DELIVERED_CTR:
	ಪಕ್ಷ SBI_CPPC_REFERENCE_PERF:
	ಪಕ್ಷ SBI_CPPC_LOWEST_FREQ:
	ಪಕ್ಷ SBI_CPPC_NOMINAL_FREQ:
	ಪಕ್ಷ SBI_CPPC_TRANSITION_LATENCY:
		ಪ್ರತ್ಯಾಗಮ false;
	}

	ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ sbi_cppc_probe(unsigned long reg)
{
	ಯದಿ (!cppc_dev || !cppc_dev->cppc_probe)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	/* Check whether register is reserved */
	ಯದಿ (sbi_cppc_is_reserved(reg))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

	ಪ್ರತ್ಯಾಗಮ cppc_dev->cppc_probe(reg);
}

ಪೂರ್ಣಾಙ್ಕ sbi_cppc_read(unsigned long reg, uint64_t *val)
{
	ಪೂರ್ಣಾಙ್ಕ ret;

	ಯದಿ (!cppc_dev || !cppc_dev->cppc_read)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ಯದಿ (ret <= 0)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	ಯದಿ (!sbi_cppc_readable(reg))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_DENIED;

	ಪ್ರತ್ಯಾಗಮ cppc_dev->cppc_read(reg, val);
}

ಪೂರ್ಣಾಙ್ಕ sbi_cppc_write(unsigned long reg, uint64_t val)
{
	ಪೂರ್ಣಾಙ್ಕ ret;

	ಯದಿ (!cppc_dev || !cppc_dev->cppc_write)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ಯದಿ (ret <= 0)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	ಯದಿ (!sbi_cppc_writable(reg))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_DENIED;

	ಪ್ರತ್ಯಾಗಮ cppc_dev->cppc_write(reg, val);
}
