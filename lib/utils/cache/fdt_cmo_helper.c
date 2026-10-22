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

ساكن بیعلامت دراز flc_جابه_جایی;

#تعریف_کن get_hart_flc(_s) \
	sbi_scratch_read_type(_s, بنية cache_device *, flc_جابه_جایی)
#تعریف_کن مجموعه_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, بنية cache_device *, flc_جابه_جایی, _p)

صحيح fdt_cmo_خصوصی_flc_flush_all(فراغ)
{
	بنية cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	إذا (!flc || !flc->cpu_خصوصی)
		أرجع SBI_ENODEV;

	أرجع cache_flush_all(flc);
}

صحيح fdt_cmo_llc_flush_all(فراغ)
{
	بنية cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	إذا (!llc)
		أرجع SBI_ENODEV;

	طالما (llc->بعدی)
		llc = llc->بعدی;

	أرجع cache_flush_all(llc);
}

صحيح fdt_cmo_خصوصی_flc_enable(bool enable)
{
	بنية cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	إذا (!flc || !flc->cpu_خصوصی)
		أرجع SBI_ENODEV;

	أرجع cache_enable(flc, enable);
}

صحيح fdt_cmo_llc_enable(bool enable)
{
	بنية cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	إذا (!llc)
		أرجع SBI_ENODEV;

	طالما (llc->بعدی)
		llc = llc->بعدی;

	أرجع cache_enable(llc, enable);
}

ساكن صحيح fdt_cmo_cold_init(ثابت فراغ *fdt)
{
	بنية sbi_scratch *scratch;
	بنية cache_device *dev;
	صحيح cpu_جابه_جایی, cpus_جابه_جایی, rc;
	u32 hartid;

	cpus_جابه_جایی = fdt_مسیر_جابه_جایی(fdt, "/cpus");
	إذا (cpus_جابه_جایی < 0)
		أرجع SBI_EINVAL;

	fdt_for_each_subnode(cpu_جابه_جایی, fdt, cpus_جابه_جایی) {
		rc = fdt_parse_hart_id(fdt, cpu_جابه_جایی, &hartid);
		إذا (rc)
			استمر;

		scratch = sbi_hartid_مقصد_scratch(hartid);
		إذا (!scratch)
			استمر;

		rc = fdt_بعدی_cache_get(fdt, cpu_جابه_جایی, &dev);
		إذا (rc && rc != SBI_ENOENT)
			أرجع rc;
		إذا (rc == SBI_ENOENT)
			dev = عدم;

		مجموعه_hart_flc(scratch, dev);
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

		cur = cur->بعدی;
	}

	أرجع SBI_OK;
}

صحيح fdt_cmo_init(bool cold_boot)
{
	ثابت فراغ *fdt = fdt_get_آدرس();
	صحيح rc;

	إذا (cold_boot) {
		flc_جابه_جایی = sbi_scratch_alloc_type_جابه_جایی(بنية cache_device *);
		إذا (!flc_جابه_جایی)
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
