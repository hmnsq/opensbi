// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <libfdt.h>
#समावेश <libfdt_env.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hartmask.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi_utils/fdt/fdt_domain.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>

पूर्णांक fdt_iterate_each_domain(व्योम *fdt, व्योम *opaque,
			    पूर्णांक (*fn)(व्योम *fdt, पूर्णांक domain_offset,
				       व्योम *opaque))
{
	पूर्णांक rc, doffset, poffset;

	अगर (!fdt || !fn)
		वापस SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	अगर (poffset < 0)
		वापस 0;
	poffset = fdt_गाँठ_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	अगर (poffset < 0)
		वापस 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		अगर (fdt_गाँठ_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			जारी;

		rc = fn(fdt, doffset, opaque);
		अगर (rc)
			वापस rc;
	}

	वापस 0;
}

अटल पूर्णांक fdt_iterate_each_domain_ro(स्थिर व्योम *fdt, व्योम *opaque,
				      पूर्णांक (*fn)(स्थिर व्योम *fdt, पूर्णांक domain_offset,
						व्योम *opaque))
{
	वापस fdt_iterate_each_domain((व्योम *)fdt, opaque,
				       (पूर्णांक (*)(व्योम *, पूर्णांक,  व्योम *))fn);
}

पूर्णांक fdt_iterate_each_memregion(व्योम *fdt, पूर्णांक domain_offset, व्योम *opaque,
			       पूर्णांक (*fn)(व्योम *fdt, पूर्णांक domain_offset,
					 पूर्णांक region_offset, u32 region_access,
					 व्योम *opaque))
{
	u32 i, rcount;
	पूर्णांक rc, len, region_offset;
	स्थिर u32 *regions;

	अगर (!fdt || (domain_offset < 0) || !fn)
		वापस SBI_EINVAL;

	अगर (fdt_गाँठ_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		वापस SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	अगर (!regions)
		वापस 0;

	rcount = (u32)len / (माप(u32) * 2);
	क्रम (i = 0; i < rcount; i++) {
		region_offset = fdt_गाँठ_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		अगर (region_offset < 0)
			वापस region_offset;

		अगर (fdt_गाँठ_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			वापस SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		अगर (rc)
			वापस rc;
	}

	वापस 0;
}

अटल पूर्णांक fdt_iterate_each_memregion_ro(स्थिर व्योम *fdt, पूर्णांक domain_offset, व्योम *opaque,
					 पूर्णांक (*fn)(स्थिर व्योम *fdt, पूर्णांक domain_offset,
						   पूर्णांक region_offset, u32 region_access,
						   व्योम *opaque))
{
	वापस fdt_iterate_each_memregion((व्योम *)fdt, domain_offset, opaque,
					  (पूर्णांक (*)(व्योम *, पूर्णांक, पूर्णांक, u32, व्योम *))fn);
}

काष्ठा __fixup_find_domain_offset_info {
	स्थिर अक्षर *name;
	पूर्णांक *doffset;
};

अटल पूर्णांक __fixup_find_domain_offset(व्योम *fdt, पूर्णांक doff, व्योम *p)
{
	काष्ठा __fixup_find_domain_offset_info *fdo = p;

	अगर (!म_भेदन(fdo->name, fdt_get_name(fdt, doff, शून्य), म_माप(fdo->name)))
		*fdo->doffset = doff;

	वापस 0;
}

#घोषणा DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

अटल पूर्णांक __fixup_count_disable_devices(व्योम *fdt, पूर्णांक doff, पूर्णांक roff,
					 u32 perm, व्योम *p)
{
	पूर्णांक len;
	u32 *dcount = p;

	अगर (perm & DISABLE_DEVICES_MASK)
		वापस 0;

	len = 0;
	अगर (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / माप(u32);

	वापस 0;
}

अटल पूर्णांक __fixup_disable_devices(व्योम *fdt, पूर्णांक doff, पूर्णांक roff,
				   u32 raccess, व्योम *p)
{
	पूर्णांक i, len, coff;
	स्थिर u32 *devices;

	अगर (raccess & DISABLE_DEVICES_MASK)
		वापस 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	अगर (!devices)
		वापस 0;
	len = len / माप(u32);

	क्रम (i = 0; i < len; i++) {
		coff = fdt_गाँठ_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		अगर (coff < 0)
			वापस coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	वापस 0;
}

व्योम fdt_domain_fixup(व्योम *fdt)
{
	u32 dcount;
	पूर्णांक err, poffset, doffset;
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();
	काष्ठा __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	अगर (!dom->सूचकांक_2)
		जाओ skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	अगर (doffset < 0)
		जाओ skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	अगर (!dcount)
		जाओ skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	अगर (err < 0)
		वापस;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	अगर (doffset < 0)
		जाओ skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, शून्य,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	अगर (poffset < 0)
		वापस;
	poffset = fdt_गाँठ_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	अगर (poffset < 0)
		वापस;
	fdt_nop_गाँठ(fdt, poffset);
}

#घोषणा FDT_DOMAIN_REGION_MAX_गिनती		16
#घोषणा FDT_मूल_REGION_INHERIT_M_ONLY		0
#घोषणा FDT_मूल_REGION_INHERIT_ALL		1

काष्ठा parse_region_तथ्य {
	काष्ठा sbi_domain *dom;
	u32 region_गिनती;
	u32 max_regions;
};

अटल पूर्णांक __fdt_parse_region(स्थिर व्योम *fdt, पूर्णांक domain_offset,
			      पूर्णांक region_offset, u32 region_access,
			      व्योम *opaque)
{
	पूर्णांक len;
	u32 val32;
	u64 val64;
	स्थिर u32 *val;
	अचिन्हित दीर्घ base, order, flags;
	काष्ठा parse_region_तथ्य *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	अगर (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		वापस SBI_EINVAL;

	/* Find next region of the domain */
	अगर (preg->max_regions <= preg->region_गिनती)
		वापस SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	अगर (!val || len != 8)
		वापस SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	अगर (!val || len != 4)
		वापस SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	अगर (val32 < 3 || __riscv_xlen < val32)
		वापस SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	अगर (fdt_get_property(fdt, region_offset, "mmio", शून्य))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_गिनती]);

	preg->region_गिनती++;

	वापस 0;
}

अटल पूर्णांक __fdt_parse_domain(स्थिर व्योम *fdt, पूर्णांक domain_offset, व्योम *opaque)
{
	u32 val32;
	u64 val64;
	स्थिर u32 *val;
	स्थिर अक्षर *inherit;
	काष्ठा sbi_domain *dom;
	काष्ठा sbi_hartmask *mask;
	काष्ठा parse_region_तथ्य preg;
	काष्ठा sbi_domain_memregion *reg;
	पूर्णांक inheritance_mode = FDT_मूल_REGION_INHERIT_M_ONLY;
	पूर्णांक i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(माप(*dom));
	अगर (!dom)
		वापस SBI_ENOMEM;

	dom->regions = sbi_calloc(माप(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_गिनती + 1);
	अगर (!dom->regions) {
		err = SBI_ENOMEM;
		जाओ fail_free_domain;
	}
	preg.dom = dom;
	preg.region_गिनती = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_गिनती;

	mask = sbi_zalloc(माप(*mask));
	अगर (!mask) {
		err = SBI_ENOMEM;
		जाओ fail_free_regions;
	}

	/* Read DT node name */
	म_नकलन(dom->name, fdt_get_name(fdt, domain_offset, शून्य),
		    माप(dom->name));
	dom->name[माप(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / माप(u32);
	अगर (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	अन्यथा
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / माप(u32);
	अगर (val && len) {
		क्रम (i = 0; i < len; i++) {
			cpu_offset = fdt_गाँठ_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			अगर (cpu_offset < 0) {
				err = cpu_offset;
				जाओ fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			अगर (err)
				जाओ fail_free_all;

			अगर (!fdt_गाँठ_is_enabled(fdt, cpu_offset))
				जारी;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	अगर (err)
		जाओ fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	अगर (inherit && len > 0) {
		अगर (!म_भेद(inherit, "all"))
			inheritance_mode = FDT_मूल_REGION_INHERIT_ALL;
		अन्यथा अगर (!म_भेद(inherit, "m-only"))
			inheritance_mode = FDT_मूल_REGION_INHERIT_M_ONLY;
		अन्यथा {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			जाओ fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&मूल, reg) {
		bool copy = false;

		चयन (inheritance_mode) {
		हाल FDT_मूल_REGION_INHERIT_ALL:
			copy = true;
			अवरोध;
		हाल FDT_मूल_REGION_INHERIT_M_ONLY:
			अगर (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			अवरोध;
		}

		अगर (!copy)
			जारी;

		अगर (preg.max_regions <= preg.region_गिनती) {
			err = SBI_EINVAL;
			जाओ fail_free_all;
		}
		स_नकल(&dom->regions[preg.region_गिनती++], reg, माप(*reg));
	}
	dom->fw_region_inited = मूल.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	अगर (val && len >= 4) {
		cpu_offset = fdt_गाँठ_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		अगर (cpu_offset >= 0 && fdt_गाँठ_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	अगर (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} अन्यथा {
		val64 = मूल.अग्र_arg1;
	}
	dom->अग्र_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	अगर (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} अन्यथा {
		val64 = मूल.अग्र_addr;
	}
	dom->अग्र_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	अगर (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		अगर (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} अन्यथा {
		val32 = मूल.अग्र_mode;
	}
	dom->अग्र_mode = val32;

	/* Read "system-reset-allowed" DT property */
	अगर (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", शून्य))
		dom->system_reset_allowed = true;
	अन्यथा
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	अगर (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", शून्य))
		dom->system_suspend_allowed = true;
	अन्यथा
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	अगर (err)
		जाओ fail_free_all;

	वापस 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	वापस err;
}

पूर्णांक fdt_domains_populate(स्थिर व्योम *fdt)
{
	/* Sanity checks */
	अगर (!fdt)
		वापस SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	वापस fdt_iterate_each_domain_ro(fdt, शून्य, __fdt_parse_domain);
}
