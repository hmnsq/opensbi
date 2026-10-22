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

पूर्णाङ्क fdt_iterate_each_domain(शून्य *fdt, शून्य *opaque,
			    पूर्णाङ्क (*fn)(शून्य *fdt, पूर्णाङ्क domain_offset,
				       शून्य *opaque))
{
	पूर्णाङ्क rc, doffset, poffset;

	जइ (!fdt || !fn)
		प्रत्यागम SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	जइ (poffset < 0)
		प्रत्यागम 0;
	poffset = fdt_ग्रन्थिः_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	जइ (poffset < 0)
		प्रत्यागम 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		जइ (fdt_ग्रन्थिः_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			अनुवर्त;

		rc = fn(fdt, doffset, opaque);
		जइ (rc)
			प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क fdt_iterate_each_domain_ro(ध्रुव शून्य *fdt, शून्य *opaque,
				      पूर्णाङ्क (*fn)(ध्रुव शून्य *fdt, पूर्णाङ्क domain_offset,
						शून्य *opaque))
{
	प्रत्यागम fdt_iterate_each_domain((शून्य *)fdt, opaque,
				       (पूर्णाङ्क (*)(शून्य *, पूर्णाङ्क,  शून्य *))fn);
}

पूर्णाङ्क fdt_iterate_each_memregion(शून्य *fdt, पूर्णाङ्क domain_offset, शून्य *opaque,
			       पूर्णाङ्क (*fn)(शून्य *fdt, पूर्णाङ्क domain_offset,
					 पूर्णाङ्क region_offset, u32 region_access,
					 शून्य *opaque))
{
	u32 i, rcount;
	पूर्णाङ्क rc, len, region_offset;
	ध्रुव u32 *regions;

	जइ (!fdt || (domain_offset < 0) || !fn)
		प्रत्यागम SBI_EINVAL;

	जइ (fdt_ग्रन्थिः_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		प्रत्यागम SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	जइ (!regions)
		प्रत्यागम 0;

	rcount = (u32)len / (परिमाण(u32) * 2);
	प्रति (i = 0; i < rcount; i++) {
		region_offset = fdt_ग्रन्थिः_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		जइ (region_offset < 0)
			प्रत्यागम region_offset;

		जइ (fdt_ग्रन्थिः_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			प्रत्यागम SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		जइ (rc)
			प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क fdt_iterate_each_memregion_ro(ध्रुव शून्य *fdt, पूर्णाङ्क domain_offset, शून्य *opaque,
					 पूर्णाङ्क (*fn)(ध्रुव शून्य *fdt, पूर्णाङ्क domain_offset,
						   पूर्णाङ्क region_offset, u32 region_access,
						   शून्य *opaque))
{
	प्रत्यागम fdt_iterate_each_memregion((शून्य *)fdt, domain_offset, opaque,
					  (पूर्णाङ्क (*)(शून्य *, पूर्णाङ्क, पूर्णाङ्क, u32, शून्य *))fn);
}

संरचना __fixup_find_domain_offset_info {
	ध्रुव वर्ण *name;
	पूर्णाङ्क *doffset;
};

स्थिर पूर्णाङ्क __fixup_find_domain_offset(शून्य *fdt, पूर्णाङ्क doff, शून्य *p)
{
	संरचना __fixup_find_domain_offset_info *fdo = p;

	जइ (!strncmp(fdo->name, fdt_get_name(fdt, doff, रिक्त), strlen(fdo->name)))
		*fdo->doffset = doff;

	प्रत्यागम 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

स्थिर पूर्णाङ्क __fixup_count_disable_devices(शून्य *fdt, पूर्णाङ्क doff, पूर्णाङ्क roff,
					 u32 perm, शून्य *p)
{
	पूर्णाङ्क len;
	u32 *dcount = p;

	जइ (perm & DISABLE_DEVICES_MASK)
		प्रत्यागम 0;

	len = 0;
	जइ (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / परिमाण(u32);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क __fixup_disable_devices(शून्य *fdt, पूर्णाङ्क doff, पूर्णाङ्क roff,
				   u32 raccess, शून्य *p)
{
	पूर्णाङ्क i, len, coff;
	ध्रुव u32 *devices;

	जइ (raccess & DISABLE_DEVICES_MASK)
		प्रत्यागम 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	जइ (!devices)
		प्रत्यागम 0;
	len = len / परिमाण(u32);

	प्रति (i = 0; i < len; i++) {
		coff = fdt_ग्रन्थिः_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		जइ (coff < 0)
			प्रत्यागम coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	प्रत्यागम 0;
}

शून्य fdt_domain_fixup(शून्य *fdt)
{
	u32 dcount;
	पूर्णाङ्क err, poffset, doffset;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	संरचना __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	जइ (!dom->index)
		गच्छ skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	जइ (doffset < 0)
		गच्छ skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	जइ (!dcount)
		गच्छ skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	जइ (err < 0)
		प्रत्यागम;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	जइ (doffset < 0)
		गच्छ skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, रिक्त,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	जइ (poffset < 0)
		प्रत्यागम;
	poffset = fdt_ग्रन्थिः_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	जइ (poffset < 0)
		प्रत्यागम;
	fdt_nop_ग्रन्थिः(fdt, poffset);
}

#define FDT_DOMAIN_REGION_MAX_गणनम्		16
#define FDT_मूलम्_REGION_INHERIT_M_ONLY		0
#define FDT_मूलम्_REGION_INHERIT_ALL		1

संरचना parse_region_data {
	संरचना sbi_domain *dom;
	u32 region_गणनम्;
	u32 max_regions;
};

स्थिर पूर्णाङ्क __fdt_parse_region(ध्रुव शून्य *fdt, पूर्णाङ्क domain_offset,
			      पूर्णाङ्क region_offset, u32 region_access,
			      शून्य *opaque)
{
	पूर्णाङ्क len;
	u32 val32;
	u64 val64;
	ध्रुव u32 *val;
	unsigned long base, order, flags;
	संरचना parse_region_data *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	जइ (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		प्रत्यागम SBI_EINVAL;

	/* Find next region of the domain */
	जइ (preg->max_regions <= preg->region_गणनम्)
		प्रत्यागम SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	जइ (!val || len != 8)
		प्रत्यागम SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	जइ (!val || len != 4)
		प्रत्यागम SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	जइ (val32 < 3 || __riscv_xlen < val32)
		प्रत्यागम SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	जइ (fdt_get_property(fdt, region_offset, "mmio", रिक्त))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_गणनम्]);

	preg->region_गणनम्++;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क __fdt_parse_domain(ध्रुव शून्य *fdt, पूर्णाङ्क domain_offset, शून्य *opaque)
{
	u32 val32;
	u64 val64;
	ध्रुव u32 *val;
	ध्रुव वर्ण *inherit;
	संरचना sbi_domain *dom;
	संरचना sbi_hartmask *mask;
	संरचना parse_region_data preg;
	संरचना sbi_domain_memregion *reg;
	पूर्णाङ्क inheritance_mode = FDT_मूलम्_REGION_INHERIT_M_ONLY;
	पूर्णाङ्क i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(परिमाण(*dom));
	जइ (!dom)
		प्रत्यागम SBI_ENOMEM;

	dom->regions = sbi_calloc(परिमाण(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_गणनम् + 1);
	जइ (!dom->regions) {
		err = SBI_ENOMEM;
		गच्छ fail_free_domain;
	}
	preg.dom = dom;
	preg.region_गणनम् = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_गणनम्;

	mask = sbi_zalloc(परिमाण(*mask));
	जइ (!mask) {
		err = SBI_ENOMEM;
		गच्छ fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_offset, रिक्त),
		    परिमाण(dom->name));
	dom->name[परिमाण(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / परिमाण(u32);
	जइ (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	अण्णहा
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / परिमाण(u32);
	जइ (val && len) {
		प्रति (i = 0; i < len; i++) {
			cpu_offset = fdt_ग्रन्थिः_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			जइ (cpu_offset < 0) {
				err = cpu_offset;
				गच्छ fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			जइ (err)
				गच्छ fail_free_all;

			जइ (!fdt_ग्रन्थिः_is_enabled(fdt, cpu_offset))
				अनुवर्त;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	जइ (err)
		गच्छ fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	जइ (inherit && len > 0) {
		जइ (!strcmp(inherit, "all"))
			inheritance_mode = FDT_मूलम्_REGION_INHERIT_ALL;
		अण्णहा जइ (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_मूलम्_REGION_INHERIT_M_ONLY;
		अण्णहा {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			गच्छ fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&मूलम्, reg) {
		bool copy = false;

		विकल्प (inheritance_mode) {
		पक्ष FDT_मूलम्_REGION_INHERIT_ALL:
			copy = true;
			भङ्ग;
		पक्ष FDT_मूलम्_REGION_INHERIT_M_ONLY:
			जइ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			भङ्ग;
		}

		जइ (!copy)
			अनुवर्त;

		जइ (preg.max_regions <= preg.region_गणनम्) {
			err = SBI_EINVAL;
			गच्छ fail_free_all;
		}
		memcpy(&dom->regions[preg.region_गणनम्++], reg, परिमाण(*reg));
	}
	dom->fw_region_inited = मूलम्.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	जइ (val && len >= 4) {
		cpu_offset = fdt_ग्रन्थिः_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		जइ (cpu_offset >= 0 && fdt_ग्रन्थिः_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	जइ (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} अण्णहा {
		val64 = मूलम्.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	जइ (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} अण्णहा {
		val64 = मूलम्.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	जइ (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		जइ (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} अण्णहा {
		val32 = मूलम्.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	जइ (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", रिक्त))
		dom->system_reset_allowed = true;
	अण्णहा
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	जइ (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", रिक्त))
		dom->system_suspend_allowed = true;
	अण्णहा
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	जइ (err)
		गच्छ fail_free_all;

	प्रत्यागम 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	प्रत्यागम err;
}

पूर्णाङ्क fdt_domains_populate(ध्रुव शून्य *fdt)
{
	/* Sanity checks */
	जइ (!fdt)
		प्रत्यागम SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	प्रत्यागम fdt_iterate_each_domain_ro(fdt, रिक्त, __fdt_parse_domain);
}
