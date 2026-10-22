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

ساکن غیرعلامتی طویل flc_انحراف;

#تعریف_کرو get_hart_flc(_s) \
	sbi_scratch_read_type(_s, ساخت cache_device *, flc_انحراف)
#تعریف_کرو سیٹ_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, ساخت cache_device *, flc_انحراف, _p)

صحیح fdt_cmo_نجی_flc_flush_all(خالی)
{
	ساخت cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	اگر (!flc || !flc->cpu_نجی)
		واپس SBI_ENODEV;

	واپس cache_flush_all(flc);
}

صحیح fdt_cmo_llc_flush_all(خالی)
{
	ساخت cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	اگر (!llc)
		واپس SBI_ENODEV;

	جب (llc->اگلا)
		llc = llc->اگلا;

	واپس cache_flush_all(llc);
}

صحیح fdt_cmo_نجی_flc_enable(bool enable)
{
	ساخت cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	اگر (!flc || !flc->cpu_نجی)
		واپس SBI_ENODEV;

	واپس cache_enable(flc, enable);
}

صحیح fdt_cmo_llc_enable(bool enable)
{
	ساخت cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	اگر (!llc)
		واپس SBI_ENODEV;

	جب (llc->اگلا)
		llc = llc->اگلا;

	واپس cache_enable(llc, enable);
}

ساکن صحیح fdt_cmo_cold_init(ثابت خالی *fdt)
{
	ساخت sbi_scratch *scratch;
	ساخت cache_device *dev;
	صحیح cpu_انحراف, cpus_انحراف, rc;
	u32 hartid;

	cpus_انحراف = fdt_راستہ_انحراف(fdt, "/cpus");
	اگر (cpus_انحراف < 0)
		واپس SBI_EINVAL;

	fdt_for_each_subnode(cpu_انحراف, fdt, cpus_انحراف) {
		rc = fdt_parse_hart_id(fdt, cpu_انحراف, &hartid);
		اگر (rc)
			جاری;

		scratch = sbi_hartid_مقصد_scratch(hartid);
		اگر (!scratch)
			جاری;

		rc = fdt_اگلا_cache_get(fdt, cpu_انحراف, &dev);
		اگر (rc && rc != SBI_ENOENT)
			واپس rc;
		اگر (rc == SBI_ENOENT)
			dev = تہی;

		سیٹ_hart_flc(scratch, dev);
	}

	واپس SBI_OK;
}

ساکن صحیح fdt_cmo_warm_init(خالی)
{
	ساخت cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	صحیح rc;

	جب (cur) {
		اگر (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			اگر (rc)
				واپس rc;
		}

		cur = cur->اگلا;
	}

	واپس SBI_OK;
}

صحیح fdt_cmo_init(bool cold_boot)
{
	ثابت خالی *fdt = fdt_get_پتہ();
	صحیح rc;

	اگر (cold_boot) {
		flc_انحراف = sbi_scratch_alloc_type_انحراف(ساخت cache_device *);
		اگر (!flc_انحراف)
			واپس SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		اگر (rc)
			واپس rc;
	}

	rc = fdt_cmo_warm_init();
	اگر (rc)
		واپس rc;

	واپس SBI_OK;
}
