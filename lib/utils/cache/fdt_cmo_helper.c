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

ကိန်းပြည့် fdt_cmo_private_flc_flush_all(void)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	အကယ်၍ (!flc || !flc->cpu_private)
		ပြန်ပေး SBI_ENODEV;

	ပြန်ပေး cache_flush_all(flc);
}

ကိန်းပြည့် fdt_cmo_llc_flush_all(void)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	အကယ်၍ (!llc)
		ပြန်ပေး SBI_ENODEV;

	နေစဉ် (llc->next)
		llc = llc->next;

	ပြန်ပေး cache_flush_all(llc);
}

ကိန်းပြည့် fdt_cmo_private_flc_enable(bool enable)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	အကယ်၍ (!flc || !flc->cpu_private)
		ပြန်ပေး SBI_ENODEV;

	ပြန်ပေး cache_enable(flc, enable);
}

ကိန်းပြည့် fdt_cmo_llc_enable(bool enable)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	အကယ်၍ (!llc)
		ပြန်ပေး SBI_ENODEV;

	နေစဉ် (llc->next)
		llc = llc->next;

	ပြန်ပေး cache_enable(llc, enable);
}

static ကိန်းပြည့် fdt_cmo_cold_init(const void *fdt)
{
	sa_nracanaa sbi_scratch *scratch;
	sa_nracanaa cache_device *dev;
	ကိန်းပြည့် cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	အကယ်၍ (cpus_offset < 0)
		ပြန်ပေး SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		အကယ်၍ (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		အကယ်၍ (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		အကယ်၍ (rc && rc != SBI_ENOENT)
			ပြန်ပေး rc;
		အကယ်၍ (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	ပြန်ပေး SBI_OK;
}

static ကိန်းပြည့် fdt_cmo_warm_init(void)
{
	sa_nracanaa cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	ကိန်းပြည့် rc;

	နေစဉ် (cur) {
		အကယ်၍ (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			အကယ်၍ (rc)
				ပြန်ပေး rc;
		}

		cur = cur->next;
	}

	ပြန်ပေး SBI_OK;
}

ကိန်းပြည့် fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	ကိန်းပြည့် rc;

	အကယ်၍ (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(sa_nracanaa cache_device *);
		အကယ်၍ (!flc_offset)
			ပြန်ပေး SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		အကယ်၍ (rc)
			ပြန်ပေး rc;
	}

	rc = fdt_cmo_warm_init();
	အကယ်၍ (rc)
		ပြန်ပေး rc;

	ပြန်ပေး SBI_OK;
}
