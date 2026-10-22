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

số_nguyên fdt_cmo_private_flc_flush_all(void)
{
	struct cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	nếu (!flc || !flc->cpu_private)
		trả_về SBI_ENODEV;

	trả_về cache_flush_all(flc);
}

số_nguyên fdt_cmo_llc_flush_all(void)
{
	struct cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	nếu (!llc)
		trả_về SBI_ENODEV;

	trong_khi (llc->next)
		llc = llc->next;

	trả_về cache_flush_all(llc);
}

số_nguyên fdt_cmo_private_flc_enable(bool enable)
{
	struct cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	nếu (!flc || !flc->cpu_private)
		trả_về SBI_ENODEV;

	trả_về cache_enable(flc, enable);
}

số_nguyên fdt_cmo_llc_enable(bool enable)
{
	struct cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	nếu (!llc)
		trả_về SBI_ENODEV;

	trong_khi (llc->next)
		llc = llc->next;

	trả_về cache_enable(llc, enable);
}

static số_nguyên fdt_cmo_cold_init(const void *fdt)
{
	struct sbi_scratch *scratch;
	struct cache_device *dev;
	số_nguyên cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	nếu (cpus_offset < 0)
		trả_về SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		nếu (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		nếu (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		nếu (rc && rc != SBI_ENOENT)
			trả_về rc;
		nếu (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	trả_về SBI_OK;
}

static số_nguyên fdt_cmo_warm_init(void)
{
	struct cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	số_nguyên rc;

	trong_khi (cur) {
		nếu (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			nếu (rc)
				trả_về rc;
		}

		cur = cur->next;
	}

	trả_về SBI_OK;
}

số_nguyên fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	số_nguyên rc;

	nếu (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(struct cache_device *);
		nếu (!flc_offset)
			trả_về SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		nếu (rc)
			trả_về rc;
	}

	rc = fdt_cmo_warm_init();
	nếu (rc)
		trả_về rc;

	trả_về SBI_OK;
}
