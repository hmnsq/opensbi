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
	sbi_scratch_read_type(_s, কাঠামো cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, কাঠামো cache_device *, flc_offset, _p)

পূর্ণসংখ্যা fdt_cmo_private_flc_flush_all(শূন্য)
{
	কাঠামো cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	যদি (!flc || !flc->cpu_private)
		ফেরত SBI_ENODEV;

	ফেরত cache_flush_all(flc);
}

পূর্ণসংখ্যা fdt_cmo_llc_flush_all(শূন্য)
{
	কাঠামো cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	যদি (!llc)
		ফেরত SBI_ENODEV;

	যখন (llc->পরবর্তী)
		llc = llc->পরবর্তী;

	ফেরত cache_flush_all(llc);
}

পূর্ণসংখ্যা fdt_cmo_private_flc_enable(bool enable)
{
	কাঠামো cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	যদি (!flc || !flc->cpu_private)
		ফেরত SBI_ENODEV;

	ফেরত cache_enable(flc, enable);
}

পূর্ণসংখ্যা fdt_cmo_llc_enable(bool enable)
{
	কাঠামো cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	যদি (!llc)
		ফেরত SBI_ENODEV;

	যখন (llc->পরবর্তী)
		llc = llc->পরবর্তী;

	ফেরত cache_enable(llc, enable);
}

স্থির পূর্ণসংখ্যা fdt_cmo_cold_init(ধ্রুবক শূন্য *fdt)
{
	কাঠামো sbi_scratch *scratch;
	কাঠামো cache_device *dev;
	পূর্ণসংখ্যা cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_offset < 0)
		ফেরত SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (rc)
			চালু;

		scratch = sbi_hartid_to_scratch(hartid);
		যদি (!scratch)
			চালু;

		rc = fdt_পরবর্তী_cache_get(fdt, cpu_offset, &dev);
		যদি (rc && rc != SBI_ENOENT)
			ফেরত rc;
		যদি (rc == SBI_ENOENT)
			dev = ফাঁকা;

		set_hart_flc(scratch, dev);
	}

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা fdt_cmo_warm_init(শূন্য)
{
	কাঠামো cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	পূর্ণসংখ্যা rc;

	যখন (cur) {
		যদি (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			যদি (rc)
				ফেরত rc;
		}

		cur = cur->পরবর্তী;
	}

	ফেরত SBI_OK;
}

পূর্ণসংখ্যা fdt_cmo_init(bool cold_boot)
{
	ধ্রুবক শূন্য *fdt = fdt_get_address();
	পূর্ণসংখ্যা rc;

	যদি (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(কাঠামো cache_device *);
		যদি (!flc_offset)
			ফেরত SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		যদি (rc)
			ফেরত rc;
	}

	rc = fdt_cmo_warm_init();
	যদি (rc)
		ফেরত rc;

	ফেরত SBI_OK;
}
