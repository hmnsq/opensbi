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

целое fdt_cmo_private_flc_flush_all(void)
{
	struct cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	если (!flc || !flc->cpu_private)
		вернуть SBI_ENODEV;

	вернуть cache_flush_all(flc);
}

целое fdt_cmo_llc_flush_all(void)
{
	struct cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	если (!llc)
		вернуть SBI_ENODEV;

	пока (llc->next)
		llc = llc->next;

	вернуть cache_flush_all(llc);
}

целое fdt_cmo_private_flc_enable(bool enable)
{
	struct cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	если (!flc || !flc->cpu_private)
		вернуть SBI_ENODEV;

	вернуть cache_enable(flc, enable);
}

целое fdt_cmo_llc_enable(bool enable)
{
	struct cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	если (!llc)
		вернуть SBI_ENODEV;

	пока (llc->next)
		llc = llc->next;

	вернуть cache_enable(llc, enable);
}

static целое fdt_cmo_cold_init(const void *fdt)
{
	struct sbi_scratch *scratch;
	struct cache_device *dev;
	целое cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	если (cpus_offset < 0)
		вернуть SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		если (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		если (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		если (rc && rc != SBI_ENOENT)
			вернуть rc;
		если (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	вернуть SBI_OK;
}

static целое fdt_cmo_warm_init(void)
{
	struct cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	целое rc;

	пока (cur) {
		если (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			если (rc)
				вернуть rc;
		}

		cur = cur->next;
	}

	вернуть SBI_OK;
}

целое fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	целое rc;

	если (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(struct cache_device *);
		если (!flc_offset)
			вернуть SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		если (rc)
			вернуть rc;
	}

	rc = fdt_cmo_warm_init();
	если (rc)
		вернуть rc;

	вернуть SBI_OK;
}
