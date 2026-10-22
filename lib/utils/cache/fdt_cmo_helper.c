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

	जइ (!flc || !flc->cpu_private)
		प्रत्यागम SBI_ENODEV;

	प्रत्यागम cache_flush_all(flc);
}

पूर्णाङ्क fdt_cmo_llc_flush_all(शून्य)
{
	संरचना cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	जइ (!llc)
		प्रत्यागम SBI_ENODEV;

	जाव (llc->next)
		llc = llc->next;

	प्रत्यागम cache_flush_all(llc);
}

पूर्णाङ्क fdt_cmo_private_flc_enable(bool enable)
{
	संरचना cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	जइ (!flc || !flc->cpu_private)
		प्रत्यागम SBI_ENODEV;

	प्रत्यागम cache_enable(flc, enable);
}

पूर्णाङ्क fdt_cmo_llc_enable(bool enable)
{
	संरचना cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	जइ (!llc)
		प्रत्यागम SBI_ENODEV;

	जाव (llc->next)
		llc = llc->next;

	प्रत्यागम cache_enable(llc, enable);
}

स्थिर पूर्णाङ्क fdt_cmo_cold_init(ध्रुव शून्य *fdt)
{
	संरचना sbi_scratch *scratch;
	संरचना cache_device *dev;
	पूर्णाङ्क cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	जइ (cpus_offset < 0)
		प्रत्यागम SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		जइ (rc)
			अनुवर्त;

		scratch = sbi_hartid_to_scratch(hartid);
		जइ (!scratch)
			अनुवर्त;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		जइ (rc && rc != SBI_ENOENT)
			प्रत्यागम rc;
		जइ (rc == SBI_ENOENT)
			dev = रिक्त;

		set_hart_flc(scratch, dev);
	}

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fdt_cmo_warm_init(शून्य)
{
	संरचना cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	पूर्णाङ्क rc;

	जाव (cur) {
		जइ (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			जइ (rc)
				प्रत्यागम rc;
		}

		cur = cur->next;
	}

	प्रत्यागम SBI_OK;
}

पूर्णाङ्क fdt_cmo_init(bool cold_boot)
{
	ध्रुव शून्य *fdt = fdt_get_address();
	पूर्णाङ्क rc;

	जइ (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(संरचना cache_device *);
		जइ (!flc_offset)
			प्रत्यागम SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		जइ (rc)
			प्रत्यागम rc;
	}

	rc = fdt_cmo_warm_init();
	जइ (rc)
		प्रत्यागम rc;

	प्रत्यागम SBI_OK;
}
