// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <libfdt_env.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hartmask.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_domain.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>

पूर्णांक fdt_iterate_each_domain(शून्य *fdt, शून्य *opaque,
			    पूर्णांक (*fn)(शून्य *fdt, पूर्णांक domain_offset,
				       शून्य *opaque))
{
	पूर्णांक rc, doffset, poffset;

	जर (!fdt || !fn)
		परत SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	जर (poffset < 0)
		परत 0;
	poffset = fdt_गाठ_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	जर (poffset < 0)
		परत 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		जर (fdt_गाठ_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			सुरूठेवा;

		rc = fn(fdt, doffset, opaque);
		जर (rc)
			परत rc;
	}

	परत 0;
}

स्थिर पूर्णांक fdt_iterate_each_domain_ro(स्थिरांक शून्य *fdt, शून्य *opaque,
				      पूर्णांक (*fn)(स्थिरांक शून्य *fdt, पूर्णांक domain_offset,
						शून्य *opaque))
{
	परत fdt_iterate_each_domain((शून्य *)fdt, opaque,
				       (पूर्णांक (*)(शून्य *, पूर्णांक,  शून्य *))fn);
}

पूर्णांक fdt_iterate_each_memregion(शून्य *fdt, पूर्णांक domain_offset, शून्य *opaque,
			       पूर्णांक (*fn)(शून्य *fdt, पूर्णांक domain_offset,
					 पूर्णांक region_offset, u32 region_access,
					 शून्य *opaque))
{
	u32 i, rcount;
	पूर्णांक rc, len, region_offset;
	स्थिरांक u32 *regions;

	जर (!fdt || (domain_offset < 0) || !fn)
		परत SBI_EINVAL;

	जर (fdt_गाठ_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		परत SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	जर (!regions)
		परत 0;

	rcount = (u32)len / (आकार(u32) * 2);
	साठी (i = 0; i < rcount; i++) {
		region_offset = fdt_गाठ_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		जर (region_offset < 0)
			परत region_offset;

		जर (fdt_गाठ_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			परत SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		जर (rc)
			परत rc;
	}

	परत 0;
}

स्थिर पूर्णांक fdt_iterate_each_memregion_ro(स्थिरांक शून्य *fdt, पूर्णांक domain_offset, शून्य *opaque,
					 पूर्णांक (*fn)(स्थिरांक शून्य *fdt, पूर्णांक domain_offset,
						   पूर्णांक region_offset, u32 region_access,
						   शून्य *opaque))
{
	परत fdt_iterate_each_memregion((शून्य *)fdt, domain_offset, opaque,
					  (पूर्णांक (*)(शून्य *, पूर्णांक, पूर्णांक, u32, शून्य *))fn);
}

संरचना __fixup_find_domain_offset_info {
	स्थिरांक अक्षर *name;
	पूर्णांक *doffset;
};

स्थिर पूर्णांक __fixup_find_domain_offset(शून्य *fdt, पूर्णांक doff, शून्य *p)
{
	संरचना __fixup_find_domain_offset_info *fdo = p;

	जर (!म_भेदन(fdo->name, fdt_get_name(fdt, doff, रिक्त), म_माप(fdo->name)))
		*fdo->doffset = doff;

	परत 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

स्थिर पूर्णांक __fixup_count_disable_devices(शून्य *fdt, पूर्णांक doff, पूर्णांक roff,
					 u32 perm, शून्य *p)
{
	पूर्णांक len;
	u32 *dcount = p;

	जर (perm & DISABLE_DEVICES_MASK)
		परत 0;

	len = 0;
	जर (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / आकार(u32);

	परत 0;
}

स्थिर पूर्णांक __fixup_disable_devices(शून्य *fdt, पूर्णांक doff, पूर्णांक roff,
				   u32 raccess, शून्य *p)
{
	पूर्णांक i, len, coff;
	स्थिरांक u32 *devices;

	जर (raccess & DISABLE_DEVICES_MASK)
		परत 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	जर (!devices)
		परत 0;
	len = len / आकार(u32);

	साठी (i = 0; i < len; i++) {
		coff = fdt_गाठ_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		जर (coff < 0)
			परत coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	परत 0;
}

शून्य fdt_domain_fixup(शून्य *fdt)
{
	u32 dcount;
	पूर्णांक err, poffset, doffset;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	संरचना __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	जर (!dom->index)
		जा skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	जर (doffset < 0)
		जा skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	जर (!dcount)
		जा skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	जर (err < 0)
		परत;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	जर (doffset < 0)
		जा skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, रिक्त,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	जर (poffset < 0)
		परत;
	poffset = fdt_गाठ_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	जर (poffset < 0)
		परत;
	fdt_nop_गाठ(fdt, poffset);
}

#define FDT_DOMAIN_REGION_MAX_मोजणी		16
#define FDT_मूळ_REGION_INHERIT_M_ONLY		0
#define FDT_मूळ_REGION_INHERIT_ALL		1

संरचना parse_region_data {
	संरचना sbi_domain *dom;
	u32 region_मोजणी;
	u32 max_regions;
};

स्थिर पूर्णांक __fdt_parse_region(स्थिरांक शून्य *fdt, पूर्णांक domain_offset,
			      पूर्णांक region_offset, u32 region_access,
			      शून्य *opaque)
{
	पूर्णांक len;
	u32 val32;
	u64 val64;
	स्थिरांक u32 *val;
	अचिन्हित दीर्घ base, order, flags;
	संरचना parse_region_data *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	जर (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		परत SBI_EINVAL;

	/* Find next region of the domain */
	जर (preg->max_regions <= preg->region_मोजणी)
		परत SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	जर (!val || len != 8)
		परत SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	जर (!val || len != 4)
		परत SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	जर (val32 < 3 || __riscv_xlen < val32)
		परत SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	जर (fdt_get_property(fdt, region_offset, "mmio", रिक्त))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_मोजणी]);

	preg->region_मोजणी++;

	परत 0;
}

स्थिर पूर्णांक __fdt_parse_domain(स्थिरांक शून्य *fdt, पूर्णांक domain_offset, शून्य *opaque)
{
	u32 val32;
	u64 val64;
	स्थिरांक u32 *val;
	स्थिरांक अक्षर *inherit;
	संरचना sbi_domain *dom;
	संरचना sbi_hartmask *mask;
	संरचना parse_region_data preg;
	संरचना sbi_domain_memregion *reg;
	पूर्णांक inheritance_mode = FDT_मूळ_REGION_INHERIT_M_ONLY;
	पूर्णांक i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(आकार(*dom));
	जर (!dom)
		परत SBI_ENOMEM;

	dom->regions = sbi_calloc(आकार(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_मोजणी + 1);
	जर (!dom->regions) {
		err = SBI_ENOMEM;
		जा fail_free_domain;
	}
	preg.dom = dom;
	preg.region_मोजणी = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_मोजणी;

	mask = sbi_zalloc(आकार(*mask));
	जर (!mask) {
		err = SBI_ENOMEM;
		जा fail_free_regions;
	}

	/* Read DT node name */
	म_नकलन(dom->name, fdt_get_name(fdt, domain_offset, रिक्त),
		    आकार(dom->name));
	dom->name[आकार(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / आकार(u32);
	जर (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	अन्यथा
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / आकार(u32);
	जर (val && len) {
		साठी (i = 0; i < len; i++) {
			cpu_offset = fdt_गाठ_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			जर (cpu_offset < 0) {
				err = cpu_offset;
				जा fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			जर (err)
				जा fail_free_all;

			जर (!fdt_गाठ_is_enabled(fdt, cpu_offset))
				सुरूठेवा;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	जर (err)
		जा fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	जर (inherit && len > 0) {
		जर (!म_भेद(inherit, "all"))
			inheritance_mode = FDT_मूळ_REGION_INHERIT_ALL;
		अन्यथा जर (!म_भेद(inherit, "m-only"))
			inheritance_mode = FDT_मूळ_REGION_INHERIT_M_ONLY;
		अन्यथा {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			जा fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&मूळ, reg) {
		bool copy = false;

		निवड (inheritance_mode) {
		स्थिती FDT_मूळ_REGION_INHERIT_ALL:
			copy = true;
			थांबा;
		स्थिती FDT_मूळ_REGION_INHERIT_M_ONLY:
			जर (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			थांबा;
		}

		जर (!copy)
			सुरूठेवा;

		जर (preg.max_regions <= preg.region_मोजणी) {
			err = SBI_EINVAL;
			जा fail_free_all;
		}
		स_नकल(&dom->regions[preg.region_मोजणी++], reg, आकार(*reg));
	}
	dom->fw_region_inited = मूळ.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	जर (val && len >= 4) {
		cpu_offset = fdt_गाठ_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		जर (cpu_offset >= 0 && fdt_गाठ_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	जर (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} अन्यथा {
		val64 = मूळ.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	जर (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} अन्यथा {
		val64 = मूळ.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	जर (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		जर (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} अन्यथा {
		val32 = मूळ.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	जर (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", रिक्त))
		dom->system_reset_allowed = true;
	अन्यथा
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	जर (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", रिक्त))
		dom->system_suspend_allowed = true;
	अन्यथा
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	जर (err)
		जा fail_free_all;

	परत 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	परत err;
}

पूर्णांक fdt_domains_populate(स्थिरांक शून्य *fdt)
{
	/* Sanity checks */
	जर (!fdt)
		परत SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	परत fdt_iterate_each_domain_ro(fdt, रिक्त, __fdt_parse_domain);
}
