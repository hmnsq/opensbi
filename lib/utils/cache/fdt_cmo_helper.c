/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi_utils/cache/fdt_cache.h>
#شامل <sbi_utils/cache/fdt_cmo_helper.h>
#شامل <sbi_utils/fdt/fdt_helper.h>

ایستا unsigned long flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, ساختار cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, ساختار cache_device *, flc_offset, _p)

صحیح fdt_cmo_private_flc_flush_all(تهی)
{
	ساختار cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	اگر (!flc || !flc->cpu_private)
		بازگشت SBI_ENODEV;

	بازگشت cache_flush_all(flc);
}

صحیح fdt_cmo_llc_flush_all(تهی)
{
	ساختار cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	اگر (!llc)
		بازگشت SBI_ENODEV;

	وقتی (llc->next)
		llc = llc->next;

	بازگشت cache_flush_all(llc);
}

صحیح fdt_cmo_private_flc_enable(bool enable)
{
	ساختار cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	اگر (!flc || !flc->cpu_private)
		بازگشت SBI_ENODEV;

	بازگشت cache_enable(flc, enable);
}

صحیح fdt_cmo_llc_enable(bool enable)
{
	ساختار cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	اگر (!llc)
		بازگشت SBI_ENODEV;

	وقتی (llc->next)
		llc = llc->next;

	بازگشت cache_enable(llc, enable);
}

ایستا صحیح fdt_cmo_cold_init(ثابت تهی *fdt)
{
	ساختار sbi_scratch *scratch;
	ساختار cache_device *dev;
	صحیح cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	اگر (cpus_offset < 0)
		بازگشت SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		اگر (rc)
			ادامه;

		scratch = sbi_hartid_to_scratch(hartid);
		اگر (!scratch)
			ادامه;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		اگر (rc && rc != SBI_ENOENT)
			بازگشت rc;
		اگر (rc == SBI_ENOENT)
			dev = پوچ;

		set_hart_flc(scratch, dev);
	}

	بازگشت SBI_OK;
}

ایستا صحیح fdt_cmo_warm_init(تهی)
{
	ساختار cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	صحیح rc;

	وقتی (cur) {
		اگر (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			اگر (rc)
				بازگشت rc;
		}

		cur = cur->next;
	}

	بازگشت SBI_OK;
}

صحیح fdt_cmo_init(bool cold_boot)
{
	ثابت تهی *fdt = fdt_get_address();
	صحیح rc;

	اگر (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(ساختار cache_device *);
		اگر (!flc_offset)
			بازگشت SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		اگر (rc)
			بازگشت rc;
	}

	rc = fdt_cmo_warm_init();
	اگر (rc)
		بازگشت rc;

	بازگشت SBI_OK;
}
