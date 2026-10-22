/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi_utils/cache/fdt_cache.h>
#அந்தர்பாவ <sbi_utils/cache/fdt_cmo_helper.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>

நிலையான unsigned long flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, கட்டமைப்பு cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, கட்டமைப்பு cache_device *, flc_offset, _p)

முழு fdt_cmo_private_flc_flush_all(வெறுமை)
{
	கட்டமைப்பு cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	எனில் (!flc || !flc->cpu_private)
		திரும்பு SBI_ENODEV;

	திரும்பு cache_flush_all(flc);
}

முழு fdt_cmo_llc_flush_all(வெறுமை)
{
	கட்டமைப்பு cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	எனில் (!llc)
		திரும்பு SBI_ENODEV;

	போது (llc->next)
		llc = llc->next;

	திரும்பு cache_flush_all(llc);
}

முழு fdt_cmo_private_flc_enable(bool enable)
{
	கட்டமைப்பு cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	எனில் (!flc || !flc->cpu_private)
		திரும்பு SBI_ENODEV;

	திரும்பு cache_enable(flc, enable);
}

முழு fdt_cmo_llc_enable(bool enable)
{
	கட்டமைப்பு cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	எனில் (!llc)
		திரும்பு SBI_ENODEV;

	போது (llc->next)
		llc = llc->next;

	திரும்பு cache_enable(llc, enable);
}

நிலையான முழு fdt_cmo_cold_init(மாறிலி வெறுமை *fdt)
{
	கட்டமைப்பு sbi_scratch *scratch;
	கட்டமைப்பு cache_device *dev;
	முழு cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	எனில் (cpus_offset < 0)
		திரும்பு SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		எனில் (rc)
			தொடர்;

		scratch = sbi_hartid_to_scratch(hartid);
		எனில் (!scratch)
			தொடர்;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		எனில் (rc && rc != SBI_ENOENT)
			திரும்பு rc;
		எனில் (rc == SBI_ENOENT)
			dev = வெற்று;

		set_hart_flc(scratch, dev);
	}

	திரும்பு SBI_OK;
}

நிலையான முழு fdt_cmo_warm_init(வெறுமை)
{
	கட்டமைப்பு cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	முழு rc;

	போது (cur) {
		எனில் (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			எனில் (rc)
				திரும்பு rc;
		}

		cur = cur->next;
	}

	திரும்பு SBI_OK;
}

முழு fdt_cmo_init(bool cold_boot)
{
	மாறிலி வெறுமை *fdt = fdt_get_address();
	முழு rc;

	எனில் (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(கட்டமைப்பு cache_device *);
		எனில் (!flc_offset)
			திரும்பு SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		எனில் (rc)
			திரும்பு rc;
	}

	rc = fdt_cmo_warm_init();
	எனில் (rc)
		திரும்பு rc;

	திரும்பு SBI_OK;
}
