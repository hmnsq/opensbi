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

ཧྲིལ fdt_cmo_private_flc_flush_all(void)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	གལ་ཏེ (!flc || !flc->cpu_private)
		ཕྱིར SBI_ENODEV;

	ཕྱིར cache_flush_all(flc);
}

ཧྲིལ fdt_cmo_llc_flush_all(void)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	གལ་ཏེ (!llc)
		ཕྱིར SBI_ENODEV;

	སྐབས (llc->next)
		llc = llc->next;

	ཕྱིར cache_flush_all(llc);
}

ཧྲིལ fdt_cmo_private_flc_enable(bool enable)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	གལ་ཏེ (!flc || !flc->cpu_private)
		ཕྱིར SBI_ENODEV;

	ཕྱིར cache_enable(flc, enable);
}

ཧྲིལ fdt_cmo_llc_enable(bool enable)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	གལ་ཏེ (!llc)
		ཕྱིར SBI_ENODEV;

	སྐབས (llc->next)
		llc = llc->next;

	ཕྱིར cache_enable(llc, enable);
}

static ཧྲིལ fdt_cmo_cold_init(const void *fdt)
{
	sa_nracanaa sbi_scratch *scratch;
	sa_nracanaa cache_device *dev;
	ཧྲིལ cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	གལ་ཏེ (cpus_offset < 0)
		ཕྱིར SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		གལ་ཏེ (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		གལ་ཏེ (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		གལ་ཏེ (rc && rc != SBI_ENOENT)
			ཕྱིར rc;
		གལ་ཏེ (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	ཕྱིར SBI_OK;
}

static ཧྲིལ fdt_cmo_warm_init(void)
{
	sa_nracanaa cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	ཧྲིལ rc;

	སྐབས (cur) {
		གལ་ཏེ (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			གལ་ཏེ (rc)
				ཕྱིར rc;
		}

		cur = cur->next;
	}

	ཕྱིར SBI_OK;
}

ཧྲིལ fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	ཧྲིལ rc;

	གལ་ཏེ (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(sa_nracanaa cache_device *);
		གལ་ཏེ (!flc_offset)
			ཕྱིར SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		གལ་ཏེ (rc)
			ཕྱིར rc;
	}

	rc = fdt_cmo_warm_init();
	གལ་ཏེ (rc)
		ཕྱིར rc;

	ཕྱིར SBI_OK;
}
