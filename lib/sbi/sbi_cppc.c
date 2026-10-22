/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_cppc.h>

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_cppc_device *cppc_dev = ಖಾಲಿ;

ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_cppc_device *sbi_cppc_get_device(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ cppc_dev;
}

ಶೂನ್ಯ sbi_cppc_ಗಣ_device(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_cppc_device *dev)
{
	ಆದರೆ (!dev || cppc_dev)
		ಹಿಂತಿರುಗಿ;

	cppc_dev = dev;
}

ಸ್ಥಿರ bool sbi_cppc_is_reserved(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg)
{
	ಆದರೆ ((reg > SBI_CPPC_ACPI_ಕೊನೆಯ && reg < SBI_CPPC_TRANSITION_LATENCY) ||
	    reg > SBI_CPPC_NON_ACPI_ಕೊನೆಯ)
		ಹಿಂತಿರುಗಿ true;

	ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ bool sbi_cppc_readable(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg)
{
	/* there are no write-only cppc registers currently */
	ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ bool sbi_cppc_writable(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg)
{
	ಆಯ್ಕೆ (reg) {
	ಸ್ಥಿತಿ SBI_CPPC_HIGHEST_PERF:
	ಸ್ಥಿತಿ SBI_CPPC_NOMINAL_PERF:
	ಸ್ಥಿತಿ SBI_CPPC_ಕೆಳಗಿನ_NON_ರೇಖೀಯ_PERF:
	ಸ್ಥಿತಿ SBI_CPPC_ಅತಿಕಡಿಮೆ_PERF:
	ಸ್ಥಿತಿ SBI_CPPC_GUARANTEED_PERF:
	ಸ್ಥಿತಿ SBI_CPPC_CTR_WRAP_ಸಮಯ:
	ಸ್ಥಿತಿ SBI_CPPC_REFERENCE_CTR:
	ಸ್ಥಿತಿ SBI_CPPC_DELIVERED_CTR:
	ಸ್ಥಿತಿ SBI_CPPC_REFERENCE_PERF:
	ಸ್ಥಿತಿ SBI_CPPC_ಅತಿಕಡಿಮೆ_FREQ:
	ಸ್ಥಿತಿ SBI_CPPC_NOMINAL_FREQ:
	ಸ್ಥಿತಿ SBI_CPPC_TRANSITION_LATENCY:
		ಹಿಂತಿರುಗಿ false;
	}

	ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ sbi_cppc_probe(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg)
{
	ಆದರೆ (!cppc_dev || !cppc_dev->cppc_probe)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	/* Check whether register is reserved */
	ಆದರೆ (sbi_cppc_is_reserved(reg))
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	ಹಿಂತಿರುಗಿ cppc_dev->cppc_probe(reg);
}

ಪೂರ್ಣಾಂಕ sbi_cppc_read(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg, uint64_t *val)
{
	ಪೂರ್ಣಾಂಕ ret;

	ಆದರೆ (!cppc_dev || !cppc_dev->cppc_read)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ಆದರೆ (ret <= 0)
		ಹಿಂತಿರುಗಿ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is write-only */
	ಆದರೆ (!sbi_cppc_readable(reg))
		ಹಿಂತಿರುಗಿ SBI_ERR_DENIED;

	ಹಿಂತಿರುಗಿ cppc_dev->cppc_read(reg, val);
}

ಪೂರ್ಣಾಂಕ sbi_cppc_write(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg, uint64_t val)
{
	ಪೂರ್ಣಾಂಕ ret;

	ಆದರೆ (!cppc_dev || !cppc_dev->cppc_write)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	/* Check whether register is implemented */
	ret = sbi_cppc_probe(reg);
	ಆದರೆ (ret <= 0)
		ಹಿಂತಿರುಗಿ SBI_ERR_NOT_SUPPORTED;

	/* Check whether the register is read-only */
	ಆದರೆ (!sbi_cppc_writable(reg))
		ಹಿಂತಿರುಗಿ SBI_ERR_DENIED;

	ಹಿಂತಿರುಗಿ cppc_dev->cppc_write(reg, val);
}
