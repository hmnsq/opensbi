/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi_utils/cache/fdt_cache.h>
#समावेश <sbi_utils/cache/fdt_cmo_helper.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>

अटल अचिन्हित दीर्घ flc_offset;

#घोषणा get_hart_flc(_s) \
	sbi_scratch_read_type(_s, काष्ठा cache_device *, flc_offset)
#घोषणा set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, काष्ठा cache_device *, flc_offset, _p)

पूर्णांक fdt_cmo_private_flc_flush_all(व्योम)
{
	काष्ठा cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	अगर (!flc || !flc->cpu_private)
		वापस SBI_ENODEV;

	वापस cache_flush_all(flc);
}

पूर्णांक fdt_cmo_llc_flush_all(व्योम)
{
	काष्ठा cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	अगर (!llc)
		वापस SBI_ENODEV;

	जबतक (llc->अग्र)
		llc = llc->अग्र;

	वापस cache_flush_all(llc);
}

पूर्णांक fdt_cmo_private_flc_enable(bool enable)
{
	काष्ठा cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	अगर (!flc || !flc->cpu_private)
		वापस SBI_ENODEV;

	वापस cache_enable(flc, enable);
}

पूर्णांक fdt_cmo_llc_enable(bool enable)
{
	काष्ठा cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	अगर (!llc)
		वापस SBI_ENODEV;

	जबतक (llc->अग्र)
		llc = llc->अग्र;

	वापस cache_enable(llc, enable);
}

अटल पूर्णांक fdt_cmo_cold_init(स्थिर व्योम *fdt)
{
	काष्ठा sbi_scratch *scratch;
	काष्ठा cache_device *dev;
	पूर्णांक cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	अगर (cpus_offset < 0)
		वापस SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		अगर (rc)
			जारी;

		scratch = sbi_hartid_to_scratch(hartid);
		अगर (!scratch)
			जारी;

		rc = fdt_अग्र_cache_get(fdt, cpu_offset, &dev);
		अगर (rc && rc != SBI_ENOENT)
			वापस rc;
		अगर (rc == SBI_ENOENT)
			dev = शून्य;

		set_hart_flc(scratch, dev);
	}

	वापस SBI_OK;
}

अटल पूर्णांक fdt_cmo_warm_init(व्योम)
{
	काष्ठा cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	पूर्णांक rc;

	जबतक (cur) {
		अगर (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			अगर (rc)
				वापस rc;
		}

		cur = cur->अग्र;
	}

	वापस SBI_OK;
}

पूर्णांक fdt_cmo_init(bool cold_boot)
{
	स्थिर व्योम *fdt = fdt_get_address();
	पूर्णांक rc;

	अगर (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(काष्ठा cache_device *);
		अगर (!flc_offset)
			वापस SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		अगर (rc)
			वापस rc;
	}

	rc = fdt_cmo_warm_init();
	अगर (rc)
		वापस rc;

	वापस SBI_OK;
}
