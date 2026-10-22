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

整数 fdt_cmo_private_flc_flush_all(void)
{
	struct cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	如果 (!flc || !flc->cpu_private)
		返回 SBI_ENODEV;

	返回 cache_flush_all(flc);
}

整数 fdt_cmo_llc_flush_all(void)
{
	struct cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	如果 (!llc)
		返回 SBI_ENODEV;

	当 (llc->next)
		llc = llc->next;

	返回 cache_flush_all(llc);
}

整数 fdt_cmo_private_flc_enable(bool enable)
{
	struct cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	如果 (!flc || !flc->cpu_private)
		返回 SBI_ENODEV;

	返回 cache_enable(flc, enable);
}

整数 fdt_cmo_llc_enable(bool enable)
{
	struct cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	如果 (!llc)
		返回 SBI_ENODEV;

	当 (llc->next)
		llc = llc->next;

	返回 cache_enable(llc, enable);
}

static 整数 fdt_cmo_cold_init(const void *fdt)
{
	struct sbi_scratch *scratch;
	struct cache_device *dev;
	整数 cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	如果 (cpus_offset < 0)
		返回 SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		如果 (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		如果 (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		如果 (rc && rc != SBI_ENOENT)
			返回 rc;
		如果 (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	返回 SBI_OK;
}

static 整数 fdt_cmo_warm_init(void)
{
	struct cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	整数 rc;

	当 (cur) {
		如果 (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			如果 (rc)
				返回 rc;
		}

		cur = cur->next;
	}

	返回 SBI_OK;
}

整数 fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	整数 rc;

	如果 (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(struct cache_device *);
		如果 (!flc_offset)
			返回 SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		如果 (rc)
			返回 rc;
	}

	rc = fdt_cmo_warm_init();
	如果 (rc)
		返回 rc;

	返回 SBI_OK;
}
