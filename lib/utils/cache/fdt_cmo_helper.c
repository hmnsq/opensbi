/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi_utils/cache/fdt_cache.h>
#অন্তর্ভাব <sbi_utils/cache/fdt_cmo_helper.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>

স্থির unsigned long flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, সংরচনা cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, সংরচনা cache_device *, flc_offset, _p)

পূর্ণাঙ্ক fdt_cmo_private_flc_flush_all(শূন্য)
{
	সংরচনা cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	যদি (!flc || !flc->cpu_private)
		প্রত্যাগম SBI_ENODEV;

	প্রত্যাগম cache_flush_all(flc);
}

পূর্ণাঙ্ক fdt_cmo_llc_flush_all(শূন্য)
{
	সংরচনা cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	যদি (!llc)
		প্রত্যাগম SBI_ENODEV;

	যাবত্ (llc->next)
		llc = llc->next;

	প্রত্যাগম cache_flush_all(llc);
}

পূর্ণাঙ্ক fdt_cmo_private_flc_enable(bool enable)
{
	সংরচনা cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	যদি (!flc || !flc->cpu_private)
		প্রত্যাগম SBI_ENODEV;

	প্রত্যাগম cache_enable(flc, enable);
}

পূর্ণাঙ্ক fdt_cmo_llc_enable(bool enable)
{
	সংরচনা cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	যদি (!llc)
		প্রত্যাগম SBI_ENODEV;

	যাবত্ (llc->next)
		llc = llc->next;

	প্রত্যাগম cache_enable(llc, enable);
}

স্থির পূর্ণাঙ্ক fdt_cmo_cold_init(ধ্রুব শূন্য *fdt)
{
	সংরচনা sbi_scratch *scratch;
	সংরচনা cache_device *dev;
	পূর্ণাঙ্ক cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_offset < 0)
		প্রত্যাগম SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (rc)
			অনুবর্ত;

		scratch = sbi_hartid_to_scratch(hartid);
		যদি (!scratch)
			অনুবর্ত;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		যদি (rc && rc != SBI_ENOENT)
			প্রত্যাগম rc;
		যদি (rc == SBI_ENOENT)
			dev = রিক্ত;

		set_hart_flc(scratch, dev);
	}

	প্রত্যাগম SBI_OK;
}

স্থির পূর্ণাঙ্ক fdt_cmo_warm_init(শূন্য)
{
	সংরচনা cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	পূর্ণাঙ্ক rc;

	যাবত্ (cur) {
		যদি (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			যদি (rc)
				প্রত্যাগম rc;
		}

		cur = cur->next;
	}

	প্রত্যাগম SBI_OK;
}

পূর্ণাঙ্ক fdt_cmo_init(bool cold_boot)
{
	ধ্রুব শূন্য *fdt = fdt_get_address();
	পূর্ণাঙ্ক rc;

	যদি (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(সংরচনা cache_device *);
		যদি (!flc_offset)
			প্রত্যাগম SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		যদি (rc)
			প্রত্যাগম rc;
	}

	rc = fdt_cmo_warm_init();
	যদি (rc)
		প্রত্যাগম rc;

	প্রত্যাগম SBI_OK;
}
