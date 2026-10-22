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

स्थिर अचिन्हित दीर्घ flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, संरचना cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, संरचना cache_device *, flc_offset, _p)

पूर्णांक fdt_cmo_private_flc_flush_all(शून्य)
{
	संरचना cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	जर (!flc || !flc->cpu_private)
		परत SBI_ENODEV;

	परत cache_flush_all(flc);
}

पूर्णांक fdt_cmo_llc_flush_all(शून्य)
{
	संरचना cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	जर (!llc)
		परत SBI_ENODEV;

	पर्यंत (llc->next)
		llc = llc->next;

	परत cache_flush_all(llc);
}

पूर्णांक fdt_cmo_private_flc_enable(bool enable)
{
	संरचना cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	जर (!flc || !flc->cpu_private)
		परत SBI_ENODEV;

	परत cache_enable(flc, enable);
}

पूर्णांक fdt_cmo_llc_enable(bool enable)
{
	संरचना cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	जर (!llc)
		परत SBI_ENODEV;

	पर्यंत (llc->next)
		llc = llc->next;

	परत cache_enable(llc, enable);
}

स्थिर पूर्णांक fdt_cmo_cold_init(स्थिरांक शून्य *fdt)
{
	संरचना sbi_scratch *scratch;
	संरचना cache_device *dev;
	पूर्णांक cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	जर (cpus_offset < 0)
		परत SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		जर (rc)
			सुरूठेवा;

		scratch = sbi_hartid_to_scratch(hartid);
		जर (!scratch)
			सुरूठेवा;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		जर (rc && rc != SBI_ENOENT)
			परत rc;
		जर (rc == SBI_ENOENT)
			dev = रिक्त;

		set_hart_flc(scratch, dev);
	}

	परत SBI_OK;
}

स्थिर पूर्णांक fdt_cmo_warm_init(शून्य)
{
	संरचना cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	पूर्णांक rc;

	पर्यंत (cur) {
		जर (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			जर (rc)
				परत rc;
		}

		cur = cur->next;
	}

	परत SBI_OK;
}

पूर्णांक fdt_cmo_init(bool cold_boot)
{
	स्थिरांक शून्य *fdt = fdt_get_address();
	पूर्णांक rc;

	जर (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(संरचना cache_device *);
		जर (!flc_offset)
			परत SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		जर (rc)
			परत rc;
	}

	rc = fdt_cmo_warm_init();
	जर (rc)
		परत rc;

	परत SBI_OK;
}
