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

	ᰀᰧ (!flc || !flc->cpu_private)
		ᰀᰧᰌ SBI_ENODEV;

	ᰀᰧᰌ cache_flush_all(flc);
}

puur_naa_nka fdt_cmo_llc_flush_all(void)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ᰀᰧ (!llc)
		ᰀᰧᰌ SBI_ENODEV;

	ᰀᰬ (llc->next)
		llc = llc->next;

	ᰀᰧᰌ cache_flush_all(llc);
}

puur_naa_nka fdt_cmo_private_flc_enable(bool enable)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ᰀᰧ (!flc || !flc->cpu_private)
		ᰀᰧᰌ SBI_ENODEV;

	ᰀᰧᰌ cache_enable(flc, enable);
}

puur_naa_nka fdt_cmo_llc_enable(bool enable)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ᰀᰧ (!llc)
		ᰀᰧᰌ SBI_ENODEV;

	ᰀᰬ (llc->next)
		llc = llc->next;

	ᰀᰧᰌ cache_enable(llc, enable);
}

static puur_naa_nka fdt_cmo_cold_init(const void *fdt)
{
	sa_nracanaa sbi_scratch *scratch;
	sa_nracanaa cache_device *dev;
	puur_naa_nka cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ᰀᰧ (cpus_offset < 0)
		ᰀᰧᰌ SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ᰀᰧ (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ᰀᰧ (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		ᰀᰧ (rc && rc != SBI_ENOENT)
			ᰀᰧᰌ rc;
		ᰀᰧ (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	ᰀᰧᰌ SBI_OK;
}

static puur_naa_nka fdt_cmo_warm_init(void)
{
	sa_nracanaa cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	puur_naa_nka rc;

	ᰀᰬ (cur) {
		ᰀᰧ (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			ᰀᰧ (rc)
				ᰀᰧᰌ rc;
		}

		cur = cur->next;
	}

	ᰀᰧᰌ SBI_OK;
}

puur_naa_nka fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	puur_naa_nka rc;

	ᰀᰧ (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(sa_nracanaa cache_device *);
		ᰀᰧ (!flc_offset)
			ᰀᰧᰌ SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		ᰀᰧ (rc)
			ᰀᰧᰌ rc;
	}

	rc = fdt_cmo_warm_init();
	ᰀᰧ (rc)
		ᰀᰧᰌ rc;

	ᰀᰧᰌ SBI_OK;
}
