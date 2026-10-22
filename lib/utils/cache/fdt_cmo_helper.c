/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi_utils/cache/fdt_cache.h>
#ضمن <sbi_utils/cache/fdt_cmo_helper.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>

ساكن بینښه اوږد flc_دننه_واټن;

#تعریف_کړه get_hart_flc(_s) \
	sbi_scratch_read_type(_s, بنية cache_device *, flc_دننه_واټن)
#تعریف_کړه سېټ_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, بنية cache_device *, flc_دننه_واټن, _p)

صحيح fdt_cmo_شخصي_flc_flush_all(فراغ)
{
	بنية cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	إذا (!flc || !flc->cpu_شخصي)
		أرجع SBI_ENODEV;

	أرجع cache_flush_all(flc);
}

صحيح fdt_cmo_llc_flush_all(فراغ)
{
	بنية cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	إذا (!llc)
		أرجع SBI_ENODEV;

	طالما (llc->بل)
		llc = llc->بل;

	أرجع cache_flush_all(llc);
}

صحيح fdt_cmo_شخصي_flc_enable(bool enable)
{
	بنية cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	إذا (!flc || !flc->cpu_شخصي)
		أرجع SBI_ENODEV;

	أرجع cache_enable(flc, enable);
}

صحيح fdt_cmo_llc_enable(bool enable)
{
	بنية cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	إذا (!llc)
		أرجع SBI_ENODEV;

	طالما (llc->بل)
		llc = llc->بل;

	أرجع cache_enable(llc, enable);
}

ساكن صحيح fdt_cmo_cold_init(ثابت فراغ *fdt)
{
	بنية sbi_scratch *scratch;
	بنية cache_device *dev;
	صحيح cpu_دننه_واټن, cpus_دننه_واټن, rc;
	u32 hartid;

	cpus_دننه_واټن = fdt_لار_دننه_واټن(fdt, "/cpus");
	إذا (cpus_دننه_واټن < 0)
		أرجع SBI_EINVAL;

	fdt_for_each_subnode(cpu_دننه_واټن, fdt, cpus_دننه_واټن) {
		rc = fdt_parse_hart_id(fdt, cpu_دننه_واټن, &hartid);
		إذا (rc)
			استمر;

		scratch = sbi_hartid_منزل_scratch(hartid);
		إذا (!scratch)
			استمر;

		rc = fdt_بل_cache_get(fdt, cpu_دننه_واټن, &dev);
		إذا (rc && rc != SBI_ENOENT)
			أرجع rc;
		إذا (rc == SBI_ENOENT)
			dev = عدم;

		سېټ_hart_flc(scratch, dev);
	}

	أرجع SBI_OK;
}

ساكن صحيح fdt_cmo_warm_init(فراغ)
{
	بنية cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	صحيح rc;

	طالما (cur) {
		إذا (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			إذا (rc)
				أرجع rc;
		}

		cur = cur->بل;
	}

	أرجع SBI_OK;
}

صحيح fdt_cmo_init(bool cold_boot)
{
	ثابت فراغ *fdt = fdt_get_پته();
	صحيح rc;

	إذا (cold_boot) {
		flc_دننه_واټن = sbi_scratch_alloc_type_دننه_واټن(بنية cache_device *);
		إذا (!flc_دننه_واټن)
			أرجع SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		إذا (rc)
			أرجع rc;
	}

	rc = fdt_cmo_warm_init();
	إذا (rc)
		أرجع rc;

	أرجع SBI_OK;
}
