/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/cache/fdt_cache.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/cache/fdt_cmo_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flc_ಸ್ಥಾನಾಂತರ;

#ವ್ಯಾಖ್ಯಾನಿಸು get_hart_flc(_s) \
	sbi_scratch_read_type(_s, ರಚನೆ cache_device *, flc_ಸ್ಥಾನಾಂತರ)
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, ರಚನೆ cache_device *, flc_ಸ್ಥಾನಾಂತರ, _p)

ಪೂರ್ಣಾಂಕ fdt_cmo_ಖಾಸಗಿ_flc_flush_all(ಶೂನ್ಯ)
{
	ರಚನೆ cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ಆದರೆ (!flc || !flc->cpu_ಖಾಸಗಿ)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಹಿಂತಿರುಗಿ cache_flush_all(flc);
}

ಪೂರ್ಣಾಂಕ fdt_cmo_llc_flush_all(ಶೂನ್ಯ)
{
	ರಚನೆ cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ಆದರೆ (!llc)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಆಗ (llc->ಮುಂದಿನ)
		llc = llc->ಮುಂದಿನ;

	ಹಿಂತಿರುಗಿ cache_flush_all(llc);
}

ಪೂರ್ಣಾಂಕ fdt_cmo_ಖಾಸಗಿ_flc_enable(bool enable)
{
	ರಚನೆ cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ಆದರೆ (!flc || !flc->cpu_ಖಾಸಗಿ)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಹಿಂತಿರುಗಿ cache_enable(flc, enable);
}

ಪೂರ್ಣಾಂಕ fdt_cmo_llc_enable(bool enable)
{
	ರಚನೆ cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ಆದರೆ (!llc)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಆಗ (llc->ಮುಂದಿನ)
		llc = llc->ಮುಂದಿನ;

	ಹಿಂತಿರುಗಿ cache_enable(llc, enable);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_cmo_cold_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt)
{
	ರಚನೆ sbi_scratch *scratch;
	ರಚನೆ cache_device *dev;
	ಪೂರ್ಣಾಂಕ cpu_ಸ್ಥಾನಾಂತರ, cpus_ಸ್ಥಾನಾಂತರ, rc;
	u32 hartid;

	cpus_ಸ್ಥಾನಾಂತರ = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/cpus");
	ಆದರೆ (cpus_ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	fdt_for_each_subnode(cpu_ಸ್ಥಾನಾಂತರ, fdt, cpus_ಸ್ಥಾನಾಂತರ) {
		rc = fdt_parse_hart_id(fdt, cpu_ಸ್ಥಾನಾಂತರ, &hartid);
		ಆದರೆ (rc)
			ಮುಂದುವರಿ;

		scratch = sbi_hartid_ಗಮ್ಯ_scratch(hartid);
		ಆದರೆ (!scratch)
			ಮುಂದುವರಿ;

		rc = fdt_ಮುಂದಿನ_cache_get(fdt, cpu_ಸ್ಥಾನಾಂತರ, &dev);
		ಆದರೆ (rc && rc != SBI_ENOENT)
			ಹಿಂತಿರುಗಿ rc;
		ಆದರೆ (rc == SBI_ENOENT)
			dev = ಖಾಲಿ;

		ಗಣ_hart_flc(scratch, dev);
	}

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_cmo_warm_init(ಶೂನ್ಯ)
{
	ರಚನೆ cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	ಪೂರ್ಣಾಂಕ rc;

	ಆಗ (cur) {
		ಆದರೆ (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			ಆದರೆ (rc)
				ಹಿಂತಿರುಗಿ rc;
		}

		cur = cur->ಮುಂದಿನ;
	}

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಪೂರ್ಣಾಂಕ fdt_cmo_init(bool cold_boot)
{
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt = fdt_get_ವಿಳಾಸ();
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (cold_boot) {
		flc_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ರಚನೆ cache_device *);
		ಆದರೆ (!flc_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	rc = fdt_cmo_warm_init();
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ SBI_OK;
}
