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

จำนวนเต็ม fdt_cmo_private_flc_flush_all(void)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ถ้า (!flc || !flc->cpu_private)
		คืน SBI_ENODEV;

	คืน cache_flush_all(flc);
}

จำนวนเต็ม fdt_cmo_llc_flush_all(void)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ถ้า (!llc)
		คืน SBI_ENODEV;

	ขณะ (llc->next)
		llc = llc->next;

	คืน cache_flush_all(llc);
}

จำนวนเต็ม fdt_cmo_private_flc_enable(bool enable)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ถ้า (!flc || !flc->cpu_private)
		คืน SBI_ENODEV;

	คืน cache_enable(flc, enable);
}

จำนวนเต็ม fdt_cmo_llc_enable(bool enable)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ถ้า (!llc)
		คืน SBI_ENODEV;

	ขณะ (llc->next)
		llc = llc->next;

	คืน cache_enable(llc, enable);
}

static จำนวนเต็ม fdt_cmo_cold_init(const void *fdt)
{
	sa_nracanaa sbi_scratch *scratch;
	sa_nracanaa cache_device *dev;
	จำนวนเต็ม cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ถ้า (cpus_offset < 0)
		คืน SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ถ้า (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ถ้า (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		ถ้า (rc && rc != SBI_ENOENT)
			คืน rc;
		ถ้า (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	คืน SBI_OK;
}

static จำนวนเต็ม fdt_cmo_warm_init(void)
{
	sa_nracanaa cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	จำนวนเต็ม rc;

	ขณะ (cur) {
		ถ้า (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			ถ้า (rc)
				คืน rc;
		}

		cur = cur->next;
	}

	คืน SBI_OK;
}

จำนวนเต็ม fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	จำนวนเต็ม rc;

	ถ้า (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(sa_nracanaa cache_device *);
		ถ้า (!flc_offset)
			คืน SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		ถ้า (rc)
			คืน rc;
	}

	rc = fdt_cmo_warm_init();
	ถ้า (rc)
		คืน rc;

	คืน SBI_OK;
}
