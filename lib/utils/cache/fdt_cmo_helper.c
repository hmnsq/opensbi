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

ساكن unsigned long flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, بنية cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, بنية cache_device *, flc_offset, _p)

صحيح fdt_cmo_private_flc_flush_all(فراغ)
{
	بنية cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	𐡄𐡍 (!flc || !flc->cpu_private)
		أرجع SBI_ENODEV;

	أرجع cache_flush_all(flc);
}

صحيح fdt_cmo_llc_flush_all(فراغ)
{
	بنية cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	𐡄𐡍 (!llc)
		أرجع SBI_ENODEV;

	طالما (llc->next)
		llc = llc->next;

	أرجع cache_flush_all(llc);
}

صحيح fdt_cmo_private_flc_enable(bool enable)
{
	بنية cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	𐡄𐡍 (!flc || !flc->cpu_private)
		أرجع SBI_ENODEV;

	أرجع cache_enable(flc, enable);
}

صحيح fdt_cmo_llc_enable(bool enable)
{
	بنية cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	𐡄𐡍 (!llc)
		أرجع SBI_ENODEV;

	طالما (llc->next)
		llc = llc->next;

	أرجع cache_enable(llc, enable);
}

ساكن صحيح fdt_cmo_cold_init(ثابت فراغ *fdt)
{
	بنية sbi_scratch *scratch;
	بنية cache_device *dev;
	صحيح cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	𐡄𐡍 (cpus_offset < 0)
		أرجع SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		𐡄𐡍 (rc)
			استمر;

		scratch = sbi_hartid_to_scratch(hartid);
		𐡄𐡍 (!scratch)
			استمر;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		𐡄𐡍 (rc && rc != SBI_ENOENT)
			أرجع rc;
		𐡄𐡍 (rc == SBI_ENOENT)
			dev = عدم;

		set_hart_flc(scratch, dev);
	}

	أرجع SBI_OK;
}

ساكن صحيح fdt_cmo_warm_init(فراغ)
{
	بنية cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	صحيح rc;

	طالما (cur) {
		𐡄𐡍 (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			𐡄𐡍 (rc)
				أرجع rc;
		}

		cur = cur->next;
	}

	أرجع SBI_OK;
}

صحيح fdt_cmo_init(bool cold_boot)
{
	ثابت فراغ *fdt = fdt_get_address();
	صحيح rc;

	𐡄𐡍 (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(بنية cache_device *);
		𐡄𐡍 (!flc_offset)
			أرجع SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		𐡄𐡍 (rc)
			أرجع rc;
	}

	rc = fdt_cmo_warm_init();
	𐡄𐡍 (rc)
		أرجع rc;

	أرجع SBI_OK;
}
