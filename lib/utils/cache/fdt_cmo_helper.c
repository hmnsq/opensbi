/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/cache/fdt_cache.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/cache/fdt_cmo_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>

ସ୍ଥିର unsigned long flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, ସଂରଚନା cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, ସଂରଚନା cache_device *, flc_offset, _p)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_cmo_private_flc_flush_all(ଶୂନ୍ୟ)
{
	ସଂରଚନା cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ଯଦି (!flc || !flc->cpu_private)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଫେରନ୍ତୁ cache_flush_all(flc);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_cmo_llc_flush_all(ଶୂନ୍ୟ)
{
	ସଂରଚନା cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ଯଦି (!llc)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଯେତେବେଳେ (llc->next)
		llc = llc->next;

	ଫେରନ୍ତୁ cache_flush_all(llc);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_cmo_private_flc_enable(bool enable)
{
	ସଂରଚନା cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ଯଦି (!flc || !flc->cpu_private)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଫେରନ୍ତୁ cache_enable(flc, enable);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_cmo_llc_enable(bool enable)
{
	ସଂରଚନା cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ଯଦି (!llc)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଯେତେବେଳେ (llc->next)
		llc = llc->next;

	ଫେରନ୍ତୁ cache_enable(llc, enable);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_cmo_cold_init(ଅଚଳ ଶୂନ୍ୟ *fdt)
{
	ସଂରଚନା sbi_scratch *scratch;
	ସଂରଚନା cache_device *dev;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ଯଦି (cpus_offset < 0)
		ଫେରନ୍ତୁ SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ଯଦି (rc)
			ଜାରି;

		scratch = sbi_hartid_to_scratch(hartid);
		ଯଦି (!scratch)
			ଜାରି;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		ଯଦି (rc && rc != SBI_ENOENT)
			ଫେରନ୍ତୁ rc;
		ଯଦି (rc == SBI_ENOENT)
			dev = ଖାଲି;

		set_hart_flc(scratch, dev);
	}

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_cmo_warm_init(ଶୂନ୍ୟ)
{
	ସଂରଚନା cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯେତେବେଳେ (cur) {
		ଯଦି (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			ଯଦି (rc)
				ଫେରନ୍ତୁ rc;
		}

		cur = cur->next;
	}

	ଫେରନ୍ତୁ SBI_OK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_cmo_init(bool cold_boot)
{
	ଅଚଳ ଶୂନ୍ୟ *fdt = fdt_get_address();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(ସଂରଚନା cache_device *);
		ଯଦି (!flc_offset)
			ଫେରନ୍ତୁ SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	rc = fdt_cmo_warm_init();
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ SBI_OK;
}
