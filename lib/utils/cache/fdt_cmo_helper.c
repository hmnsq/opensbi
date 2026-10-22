/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi_utils/cache/fdt_cache.h>
#अन्तर्भाव <sbi_utils/cache/fdt_cmo_helper.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>

स्थिर unsigned long flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, संरचना cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, संरचना cache_device *, flc_offset, _p)

पूर्णाङ्क fdt_cmo_private_flc_flush_all(शून्य)
{
	संरचना cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	सचे (!flc || !flc->cpu_private)
		पटिनिवत्त SBI_ENODEV;

	पटिनिवत्त cache_flush_all(flc);
}

पूर्णाङ्क fdt_cmo_llc_flush_all(शून्य)
{
	संरचना cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	सचे (!llc)
		पटिनिवत्त SBI_ENODEV;

	याव (llc->next)
		llc = llc->next;

	पटिनिवत्त cache_flush_all(llc);
}

पूर्णाङ्क fdt_cmo_private_flc_enable(bool enable)
{
	संरचना cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	सचे (!flc || !flc->cpu_private)
		पटिनिवत्त SBI_ENODEV;

	पटिनिवत्त cache_enable(flc, enable);
}

पूर्णाङ्क fdt_cmo_llc_enable(bool enable)
{
	संरचना cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	सचे (!llc)
		पटिनिवत्त SBI_ENODEV;

	याव (llc->next)
		llc = llc->next;

	पटिनिवत्त cache_enable(llc, enable);
}

स्थिर पूर्णाङ्क fdt_cmo_cold_init(ध्रुव शून्य *fdt)
{
	संरचना sbi_scratch *scratch;
	संरचना cache_device *dev;
	पूर्णाङ्क cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	सचे (cpus_offset < 0)
		पटिनिवत्त SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		सचे (rc)
			अनुवर्त;

		scratch = sbi_hartid_to_scratch(hartid);
		सचे (!scratch)
			अनुवर्त;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		सचे (rc && rc != SBI_ENOENT)
			पटिनिवत्त rc;
		सचे (rc == SBI_ENOENT)
			dev = रिक्त;

		set_hart_flc(scratch, dev);
	}

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क fdt_cmo_warm_init(शून्य)
{
	संरचना cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	पूर्णाङ्क rc;

	याव (cur) {
		सचे (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			सचे (rc)
				पटिनिवत्त rc;
		}

		cur = cur->next;
	}

	पटिनिवत्त SBI_OK;
}

पूर्णाङ्क fdt_cmo_init(bool cold_boot)
{
	ध्रुव शून्य *fdt = fdt_get_address();
	पूर्णाङ्क rc;

	सचे (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(संरचना cache_device *);
		सचे (!flc_offset)
			पटिनिवत्त SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		सचे (rc)
			पटिनिवत्त rc;
	}

	rc = fdt_cmo_warm_init();
	सचे (rc)
		पटिनिवत्त rc;

	पटिनिवत्त SBI_OK;
}
