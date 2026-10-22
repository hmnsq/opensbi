/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi_utils/cache/fdt_cache.h>
#അന്തര്ഭാവ <sbi_utils/cache/fdt_cmo_helper.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>

സ്ഥിതം unsigned long flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, ഘടന cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, ഘടന cache_device *, flc_offset, _p)

പൂർണ്ണസംഖ്യ fdt_cmo_private_flc_flush_all(ശൂന്യം)
{
	ഘടന cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	എങ്കിൽ (!flc || !flc->cpu_private)
		തിരികെ SBI_ENODEV;

	തിരികെ cache_flush_all(flc);
}

പൂർണ്ണസംഖ്യ fdt_cmo_llc_flush_all(ശൂന്യം)
{
	ഘടന cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	എങ്കിൽ (!llc)
		തിരികെ SBI_ENODEV;

	ആകുമ്പോൾ (llc->next)
		llc = llc->next;

	തിരികെ cache_flush_all(llc);
}

പൂർണ്ണസംഖ്യ fdt_cmo_private_flc_enable(bool enable)
{
	ഘടന cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	എങ്കിൽ (!flc || !flc->cpu_private)
		തിരികെ SBI_ENODEV;

	തിരികെ cache_enable(flc, enable);
}

പൂർണ്ണസംഖ്യ fdt_cmo_llc_enable(bool enable)
{
	ഘടന cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	എങ്കിൽ (!llc)
		തിരികെ SBI_ENODEV;

	ആകുമ്പോൾ (llc->next)
		llc = llc->next;

	തിരികെ cache_enable(llc, enable);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_cmo_cold_init(സ്ഥിരം ശൂന്യം *fdt)
{
	ഘടന sbi_scratch *scratch;
	ഘടന cache_device *dev;
	പൂർണ്ണസംഖ്യ cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	എങ്കിൽ (cpus_offset < 0)
		തിരികെ SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		എങ്കിൽ (rc)
			തുടരുക;

		scratch = sbi_hartid_to_scratch(hartid);
		എങ്കിൽ (!scratch)
			തുടരുക;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		എങ്കിൽ (rc && rc != SBI_ENOENT)
			തിരികെ rc;
		എങ്കിൽ (rc == SBI_ENOENT)
			dev = ശൂന്യ;

		set_hart_flc(scratch, dev);
	}

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_cmo_warm_init(ശൂന്യം)
{
	ഘടന cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	പൂർണ്ണസംഖ്യ rc;

	ആകുമ്പോൾ (cur) {
		എങ്കിൽ (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			എങ്കിൽ (rc)
				തിരികെ rc;
		}

		cur = cur->next;
	}

	തിരികെ SBI_OK;
}

പൂർണ്ണസംഖ്യ fdt_cmo_init(bool cold_boot)
{
	സ്ഥിരം ശൂന്യം *fdt = fdt_get_address();
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(ഘടന cache_device *);
		എങ്കിൽ (!flc_offset)
			തിരികെ SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	rc = fdt_cmo_warm_init();
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ SBI_OK;
}
