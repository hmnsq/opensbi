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
	sbi_scratch_read_type(_s, struct cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, struct cache_device *, flc_offset, _p)

цео fdt_cmo_private_flc_flush_all(void)
{
	struct cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ако (!flc || !flc->cpu_private)
		врати SBI_ENODEV;

	врати cache_flush_all(flc);
}

цео fdt_cmo_llc_flush_all(void)
{
	struct cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ако (!llc)
		врати SBI_ENODEV;

	док (llc->next)
		llc = llc->next;

	врати cache_flush_all(llc);
}

цео fdt_cmo_private_flc_enable(bool enable)
{
	struct cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ако (!flc || !flc->cpu_private)
		врати SBI_ENODEV;

	врати cache_enable(flc, enable);
}

цео fdt_cmo_llc_enable(bool enable)
{
	struct cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ако (!llc)
		врати SBI_ENODEV;

	док (llc->next)
		llc = llc->next;

	врати cache_enable(llc, enable);
}

static цео fdt_cmo_cold_init(const void *fdt)
{
	struct sbi_scratch *scratch;
	struct cache_device *dev;
	цео cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ако (cpus_offset < 0)
		врати SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ако (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ако (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		ако (rc && rc != SBI_ENOENT)
			врати rc;
		ако (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	врати SBI_OK;
}

static цео fdt_cmo_warm_init(void)
{
	struct cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	цео rc;

	док (cur) {
		ако (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			ако (rc)
				врати rc;
		}

		cur = cur->next;
	}

	врати SBI_OK;
}

цео fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	цео rc;

	ако (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(struct cache_device *);
		ако (!flc_offset)
			врати SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		ако (rc)
			врати rc;
	}

	rc = fdt_cmo_warm_init();
	ако (rc)
		врати rc;

	врати SBI_OK;
}
