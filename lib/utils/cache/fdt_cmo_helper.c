/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#include <libfdt.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_scratch.h>
#include <sbi_utils/cache/fdt_cache.h>
#include <sbi_utils/cache/fdt_cmo_helper.h>
#include <sbi_utils/fdt/fdt_helper.h>

static unsigned long flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, sa_nracanaa cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, sa_nracanaa cache_device *, flc_offset, _p)

puur_naa_nka fdt_cmo_private_flc_flush_all(void)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ᮚᮦᮔ᮪ (!flc || !flc->cpu_private)
		ᮝᮜᮤ SBI_ENODEV;

	ᮝᮜᮤ cache_flush_all(flc);
}

puur_naa_nka fdt_cmo_llc_flush_all(void)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ᮚᮦᮔ᮪ (!llc)
		ᮝᮜᮤ SBI_ENODEV;

	ᮞᮨᮓᮨᮜ (llc->next)
		llc = llc->next;

	ᮝᮜᮤ cache_flush_all(llc);
}

puur_naa_nka fdt_cmo_private_flc_enable(bool enable)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ᮚᮦᮔ᮪ (!flc || !flc->cpu_private)
		ᮝᮜᮤ SBI_ENODEV;

	ᮝᮜᮤ cache_enable(flc, enable);
}

puur_naa_nka fdt_cmo_llc_enable(bool enable)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ᮚᮦᮔ᮪ (!llc)
		ᮝᮜᮤ SBI_ENODEV;

	ᮞᮨᮓᮨᮜ (llc->next)
		llc = llc->next;

	ᮝᮜᮤ cache_enable(llc, enable);
}

static puur_naa_nka fdt_cmo_cold_init(const void *fdt)
{
	sa_nracanaa sbi_scratch *scratch;
	sa_nracanaa cache_device *dev;
	puur_naa_nka cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ᮚᮦᮔ᮪ (cpus_offset < 0)
		ᮝᮜᮤ SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ᮚᮦᮔ᮪ (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ᮚᮦᮔ᮪ (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		ᮚᮦᮔ᮪ (rc && rc != SBI_ENOENT)
			ᮝᮜᮤ rc;
		ᮚᮦᮔ᮪ (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	ᮝᮜᮤ SBI_OK;
}

static puur_naa_nka fdt_cmo_warm_init(void)
{
	sa_nracanaa cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	puur_naa_nka rc;

	ᮞᮨᮓᮨᮜ (cur) {
		ᮚᮦᮔ᮪ (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			ᮚᮦᮔ᮪ (rc)
				ᮝᮜᮤ rc;
		}

		cur = cur->next;
	}

	ᮝᮜᮤ SBI_OK;
}

puur_naa_nka fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	puur_naa_nka rc;

	ᮚᮦᮔ᮪ (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(sa_nracanaa cache_device *);
		ᮚᮦᮔ᮪ (!flc_offset)
			ᮝᮜᮤ SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		ᮚᮦᮔ᮪ (rc)
			ᮝᮜᮤ rc;
	}

	rc = fdt_cmo_warm_init();
	ᮚᮦᮔ᮪ (rc)
		ᮝᮜᮤ rc;

	ᮝᮜᮤ SBI_OK;
}
