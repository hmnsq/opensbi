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

स्थिर अचिह्नित दीर्घ flc_विस्थापनम्;

#परिभाषय get_hart_flc(_s) \
	sbi_scratch_read_type(_s, संरचना cache_device *, flc_विस्थापनम्)
#परिभाषय समुच्चयः_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, संरचना cache_device *, flc_विस्थापनम्, _p)

पूर्णाङ्क fdt_cmo_वैयक्तिकम्_flc_flush_all(शून्य)
{
	संरचना cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	यदि (!flc || !flc->cpu_वैयक्तिकम्)
		प्रत्यागम SBI_ENODEV;

	प्रत्यागम cache_flush_all(flc);
}

पूर्णाङ्क fdt_cmo_llc_flush_all(शून्य)
{
	संरचना cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	यदि (!llc)
		प्रत्यागम SBI_ENODEV;

	यावत् (llc->अग्र)
		llc = llc->अग्र;

	प्रत्यागम cache_flush_all(llc);
}

पूर्णाङ्क fdt_cmo_वैयक्तिकम्_flc_enable(bool enable)
{
	संरचना cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	यदि (!flc || !flc->cpu_वैयक्तिकम्)
		प्रत्यागम SBI_ENODEV;

	प्रत्यागम cache_enable(flc, enable);
}

पूर्णाङ्क fdt_cmo_llc_enable(bool enable)
{
	संरचना cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	यदि (!llc)
		प्रत्यागम SBI_ENODEV;

	यावत् (llc->अग्र)
		llc = llc->अग्र;

	प्रत्यागम cache_enable(llc, enable);
}

स्थिर पूर्णाङ्क fdt_cmo_cold_init(ध्रुव शून्य *fdt)
{
	संरचना sbi_scratch *scratch;
	संरचना cache_device *dev;
	पूर्णाङ्क cpu_विस्थापनम्, cpus_विस्थापनम्, rc;
	u32 hartid;

	cpus_विस्थापनम् = fdt_मार्गः_विस्थापनम्(fdt, "/cpus");
	यदि (cpus_विस्थापनम् < 0)
		प्रत्यागम SBI_EINVAL;

	fdt_for_each_subnode(cpu_विस्थापनम्, fdt, cpus_विस्थापनम्) {
		rc = fdt_parse_hart_id(fdt, cpu_विस्थापनम्, &hartid);
		यदि (rc)
			अनुवर्त;

		scratch = sbi_hartid_गन्तव्यम्_scratch(hartid);
		यदि (!scratch)
			अनुवर्त;

		rc = fdt_अग्र_cache_get(fdt, cpu_विस्थापनम्, &dev);
		यदि (rc && rc != SBI_ENOENT)
			प्रत्यागम rc;
		यदि (rc == SBI_ENOENT)
			dev = रिक्त;

		समुच्चयः_hart_flc(scratch, dev);
	}

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fdt_cmo_warm_init(शून्य)
{
	संरचना cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	पूर्णाङ्क rc;

	यावत् (cur) {
		यदि (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			यदि (rc)
				प्रत्यागम rc;
		}

		cur = cur->अग्र;
	}

	प्रत्यागम SBI_OK;
}

पूर्णाङ्क fdt_cmo_init(bool cold_boot)
{
	ध्रुव शून्य *fdt = fdt_get_स्थानाङ्कः();
	पूर्णाङ्क rc;

	यदि (cold_boot) {
		flc_विस्थापनम् = sbi_scratch_alloc_type_विस्थापनम्(संरचना cache_device *);
		यदि (!flc_विस्थापनम्)
			प्रत्यागम SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		यदि (rc)
			प्रत्यागम rc;
	}

	rc = fdt_cmo_warm_init();
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम SBI_OK;
}
