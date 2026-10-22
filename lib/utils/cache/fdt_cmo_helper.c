/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi_utils/cache/fdt_cache.h>
#અન્તર્ભાવ <sbi_utils/cache/fdt_cmo_helper.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>

સ્થિર unsigned long flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, સંરચના cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, સંરચના cache_device *, flc_offset, _p)

પૂર્ણાંક fdt_cmo_private_flc_flush_all(શૂન્ય)
{
	સંરચના cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	જો (!flc || !flc->cpu_private)
		પાછા SBI_ENODEV;

	પાછા cache_flush_all(flc);
}

પૂર્ણાંક fdt_cmo_llc_flush_all(શૂન્ય)
{
	સંરચના cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	જો (!llc)
		પાછા SBI_ENODEV;

	જ્યારે (llc->next)
		llc = llc->next;

	પાછા cache_flush_all(llc);
}

પૂર્ણાંક fdt_cmo_private_flc_enable(bool enable)
{
	સંરચના cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	જો (!flc || !flc->cpu_private)
		પાછા SBI_ENODEV;

	પાછા cache_enable(flc, enable);
}

પૂર્ણાંક fdt_cmo_llc_enable(bool enable)
{
	સંરચના cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	જો (!llc)
		પાછા SBI_ENODEV;

	જ્યારે (llc->next)
		llc = llc->next;

	પાછા cache_enable(llc, enable);
}

સ્થિર પૂર્ણાંક fdt_cmo_cold_init(અચલ શૂન્ય *fdt)
{
	સંરચના sbi_scratch *scratch;
	સંરચના cache_device *dev;
	પૂર્ણાંક cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	જો (cpus_offset < 0)
		પાછા SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		જો (rc)
			ચાલુ;

		scratch = sbi_hartid_to_scratch(hartid);
		જો (!scratch)
			ચાલુ;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		જો (rc && rc != SBI_ENOENT)
			પાછા rc;
		જો (rc == SBI_ENOENT)
			dev = ખાલી;

		set_hart_flc(scratch, dev);
	}

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક fdt_cmo_warm_init(શૂન્ય)
{
	સંરચના cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	પૂર્ણાંક rc;

	જ્યારે (cur) {
		જો (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			જો (rc)
				પાછા rc;
		}

		cur = cur->next;
	}

	પાછા SBI_OK;
}

પૂર્ણાંક fdt_cmo_init(bool cold_boot)
{
	અચલ શૂન્ય *fdt = fdt_get_address();
	પૂર્ણાંક rc;

	જો (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(સંરચના cache_device *);
		જો (!flc_offset)
			પાછા SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		જો (rc)
			પાછા rc;
	}

	rc = fdt_cmo_warm_init();
	જો (rc)
		પાછા rc;

	પાછા SBI_OK;
}
