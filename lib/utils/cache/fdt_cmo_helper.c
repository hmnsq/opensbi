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

цяло fdt_cmo_private_flc_flush_all(void)
{
	struct cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ако (!flc || !flc->cpu_private)
		върни SBI_ENODEV;

	върни cache_flush_all(flc);
}

цяло fdt_cmo_llc_flush_all(void)
{
	struct cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ако (!llc)
		върни SBI_ENODEV;

	докато (llc->next)
		llc = llc->next;

	върни cache_flush_all(llc);
}

цяло fdt_cmo_private_flc_enable(bool enable)
{
	struct cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ако (!flc || !flc->cpu_private)
		върни SBI_ENODEV;

	върни cache_enable(flc, enable);
}

цяло fdt_cmo_llc_enable(bool enable)
{
	struct cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ако (!llc)
		върни SBI_ENODEV;

	докато (llc->next)
		llc = llc->next;

	върни cache_enable(llc, enable);
}

static цяло fdt_cmo_cold_init(const void *fdt)
{
	struct sbi_scratch *scratch;
	struct cache_device *dev;
	цяло cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ако (cpus_offset < 0)
		върни SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ако (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ако (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		ако (rc && rc != SBI_ENOENT)
			върни rc;
		ако (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	върни SBI_OK;
}

static цяло fdt_cmo_warm_init(void)
{
	struct cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	цяло rc;

	докато (cur) {
		ако (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			ако (rc)
				върни rc;
		}

		cur = cur->next;
	}

	върни SBI_OK;
}

цяло fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	цяло rc;

	ако (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(struct cache_device *);
		ако (!flc_offset)
			върни SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		ако (rc)
			върни rc;
	}

	rc = fdt_cmo_warm_init();
	ако (rc)
		върни rc;

	върни SBI_OK;
}
