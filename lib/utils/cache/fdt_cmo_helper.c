/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi_utils/cache/fdt_cache.h>
#অন্তর্ভাৱ <sbi_utils/cache/fdt_cmo_helper.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>

স্থির unsigned long flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, সংরচনা cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, সংরচনা cache_device *, flc_offset, _p)

পূর্ণাঙ্ক fdt_cmo_private_flc_flush_all(শূন্য)
{
	সংরচনা cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	যদি (!flc || !flc->cpu_private)
		ঘূৰাই SBI_ENODEV;

	ঘূৰাই cache_flush_all(flc);
}

পূর্ণাঙ্ক fdt_cmo_llc_flush_all(শূন্য)
{
	সংরচনা cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	যদি (!llc)
		ঘূৰাই SBI_ENODEV;

	যেতিয়ালৈ (llc->next)
		llc = llc->next;

	ঘূৰাই cache_flush_all(llc);
}

পূর্ণাঙ্ক fdt_cmo_private_flc_enable(bool enable)
{
	সংরচনা cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	যদি (!flc || !flc->cpu_private)
		ঘূৰাই SBI_ENODEV;

	ঘূৰাই cache_enable(flc, enable);
}

পূর্ণাঙ্ক fdt_cmo_llc_enable(bool enable)
{
	সংরচনা cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	যদি (!llc)
		ঘূৰাই SBI_ENODEV;

	যেতিয়ালৈ (llc->next)
		llc = llc->next;

	ঘূৰাই cache_enable(llc, enable);
}

স্থির পূর্ণাঙ্ক fdt_cmo_cold_init(ধ্রুৱ শূন্য *fdt)
{
	সংরচনা sbi_scratch *scratch;
	সংরচনা cache_device *dev;
	পূর্ণাঙ্ক cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_offset < 0)
		ঘূৰাই SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (rc)
			অনুৱর্ত;

		scratch = sbi_hartid_to_scratch(hartid);
		যদি (!scratch)
			অনুৱর্ত;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		যদি (rc && rc != SBI_ENOENT)
			ঘূৰাই rc;
		যদি (rc == SBI_ENOENT)
			dev = রিক্ত;

		set_hart_flc(scratch, dev);
	}

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক fdt_cmo_warm_init(শূন্য)
{
	সংরচনা cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	পূর্ণাঙ্ক rc;

	যেতিয়ালৈ (cur) {
		যদি (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			যদি (rc)
				ঘূৰাই rc;
		}

		cur = cur->next;
	}

	ঘূৰাই SBI_OK;
}

পূর্ণাঙ্ক fdt_cmo_init(bool cold_boot)
{
	ধ্রুৱ শূন্য *fdt = fdt_get_address();
	পূর্ণাঙ্ক rc;

	যদি (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(সংরচনা cache_device *);
		যদি (!flc_offset)
			ঘূৰাই SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		যদি (rc)
			ঘূৰাই rc;
	}

	rc = fdt_cmo_warm_init();
	যদি (rc)
		ঘূৰাই rc;

	ঘূৰাই SBI_OK;
}
