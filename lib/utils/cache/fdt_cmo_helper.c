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

ಸ್ಥಿರ unsigned long flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, ಸಂರಚನಾ cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, ಸಂರಚನಾ cache_device *, flc_offset, _p)

ಪೂರ್ಣಾಙ್ಕ fdt_cmo_private_flc_flush_all(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ಯದಿ (!flc || !flc->cpu_private)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಪ್ರತ್ಯಾಗಮ cache_flush_all(flc);
}

ಪೂರ್ಣಾಙ್ಕ fdt_cmo_llc_flush_all(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ಯದಿ (!llc)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಯಾವತ್ (llc->next)
		llc = llc->next;

	ಪ್ರತ್ಯಾಗಮ cache_flush_all(llc);
}

ಪೂರ್ಣಾಙ್ಕ fdt_cmo_private_flc_enable(bool enable)
{
	ಸಂರಚನಾ cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ಯದಿ (!flc || !flc->cpu_private)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಪ್ರತ್ಯಾಗಮ cache_enable(flc, enable);
}

ಪೂರ್ಣಾಙ್ಕ fdt_cmo_llc_enable(bool enable)
{
	ಸಂರಚನಾ cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ಯದಿ (!llc)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಯಾವತ್ (llc->next)
		llc = llc->next;

	ಪ್ರತ್ಯಾಗಮ cache_enable(llc, enable);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_cmo_cold_init(ಧ್ರುವ ಶೂನ್ಯ *fdt)
{
	ಸಂರಚನಾ sbi_scratch *scratch;
	ಸಂರಚನಾ cache_device *dev;
	ಪೂರ್ಣಾಙ್ಕ cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ಯದಿ (cpus_offset < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ಯದಿ (rc)
			ಅನುವರ್ತ;

		scratch = sbi_hartid_to_scratch(hartid);
		ಯದಿ (!scratch)
			ಅನುವರ್ತ;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		ಯದಿ (rc && rc != SBI_ENOENT)
			ಪ್ರತ್ಯಾಗಮ rc;
		ಯದಿ (rc == SBI_ENOENT)
			dev = ರಿಕ್ತ;

		set_hart_flc(scratch, dev);
	}

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_cmo_warm_init(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯಾವತ್ (cur) {
		ಯದಿ (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			ಯದಿ (rc)
				ಪ್ರತ್ಯಾಗಮ rc;
		}

		cur = cur->next;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಪೂರ್ಣಾಙ್ಕ fdt_cmo_init(bool cold_boot)
{
	ಧ್ರುವ ಶೂನ್ಯ *fdt = fdt_get_address();
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(ಸಂರಚನಾ cache_device *);
		ಯದಿ (!flc_offset)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	rc = fdt_cmo_warm_init();
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}
