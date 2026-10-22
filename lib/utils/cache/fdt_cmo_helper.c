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

පූර්ණ_සංඛ්‍යා fdt_cmo_private_flc_flush_all(void)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	නම් (!flc || !flc->cpu_private)
		ආපසු SBI_ENODEV;

	ආපසු cache_flush_all(flc);
}

පූර්ණ_සංඛ්‍යා fdt_cmo_llc_flush_all(void)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	නම් (!llc)
		ආපසු SBI_ENODEV;

	අතරතුර (llc->next)
		llc = llc->next;

	ආපසු cache_flush_all(llc);
}

පූර්ණ_සංඛ්‍යා fdt_cmo_private_flc_enable(bool enable)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	නම් (!flc || !flc->cpu_private)
		ආපසු SBI_ENODEV;

	ආපසු cache_enable(flc, enable);
}

පූර්ණ_සංඛ්‍යා fdt_cmo_llc_enable(bool enable)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	නම් (!llc)
		ආපසු SBI_ENODEV;

	අතරතුර (llc->next)
		llc = llc->next;

	ආපසු cache_enable(llc, enable);
}

static පූර්ණ_සංඛ්‍යා fdt_cmo_cold_init(const void *fdt)
{
	sa_nracanaa sbi_scratch *scratch;
	sa_nracanaa cache_device *dev;
	පූර්ණ_සංඛ්‍යා cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	නම් (cpus_offset < 0)
		ආපසු SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		නම් (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		නම් (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		නම් (rc && rc != SBI_ENOENT)
			ආපසු rc;
		නම් (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	ආපසු SBI_OK;
}

static පූර්ණ_සංඛ්‍යා fdt_cmo_warm_init(void)
{
	sa_nracanaa cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	පූර්ණ_සංඛ්‍යා rc;

	අතරතුර (cur) {
		නම් (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			නම් (rc)
				ආපසු rc;
		}

		cur = cur->next;
	}

	ආපසු SBI_OK;
}

පූර්ණ_සංඛ්‍යා fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	පූර්ණ_සංඛ්‍යා rc;

	නම් (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(sa_nracanaa cache_device *);
		නම් (!flc_offset)
			ආපසු SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		නම් (rc)
			ආපසු rc;
	}

	rc = fdt_cmo_warm_init();
	නම් (rc)
		ආපසු rc;

	ආපසු SBI_OK;
}
