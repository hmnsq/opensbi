// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <libfdt_env.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hartmask.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_domain.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>

പൂർണ്ണസംഖ്യ fdt_iterate_each_domain(ശൂന്യം *fdt, ശൂന്യം *opaque,
			    പൂർണ്ണസംഖ്യ (*fn)(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ domain_offset,
				       ശൂന്യം *opaque))
{
	പൂർണ്ണസംഖ്യ rc, doffset, poffset;

	എങ്കിൽ (!fdt || !fn)
		തിരികെ SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	എങ്കിൽ (poffset < 0)
		തിരികെ 0;
	poffset = fdt_കണ്ണി_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	എങ്കിൽ (poffset < 0)
		തിരികെ 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		എങ്കിൽ (fdt_കണ്ണി_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			തുടരുക;

		rc = fn(fdt, doffset, opaque);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_iterate_each_domain_ro(സ്ഥിരം ശൂന്യം *fdt, ശൂന്യം *opaque,
				      പൂർണ്ണസംഖ്യ (*fn)(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ domain_offset,
						ശൂന്യം *opaque))
{
	തിരികെ fdt_iterate_each_domain((ശൂന്യം *)fdt, opaque,
				       (പൂർണ്ണസംഖ്യ (*)(ശൂന്യം *, പൂർണ്ണസംഖ്യ,  ശൂന്യം *))fn);
}

പൂർണ്ണസംഖ്യ fdt_iterate_each_memregion(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ domain_offset, ശൂന്യം *opaque,
			       പൂർണ്ണസംഖ്യ (*fn)(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ domain_offset,
					 പൂർണ്ണസംഖ്യ region_offset, u32 region_access,
					 ശൂന്യം *opaque))
{
	u32 i, rcount;
	പൂർണ്ണസംഖ്യ rc, len, region_offset;
	സ്ഥിരം u32 *regions;

	എങ്കിൽ (!fdt || (domain_offset < 0) || !fn)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (fdt_കണ്ണി_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		തിരികെ SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	എങ്കിൽ (!regions)
		തിരികെ 0;

	rcount = (u32)len / (വലുപ്പം(u32) * 2);
	വേണ്ടി (i = 0; i < rcount; i++) {
		region_offset = fdt_കണ്ണി_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		എങ്കിൽ (region_offset < 0)
			തിരികെ region_offset;

		എങ്കിൽ (fdt_കണ്ണി_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			തിരികെ SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_iterate_each_memregion_ro(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ domain_offset, ശൂന്യം *opaque,
					 പൂർണ്ണസംഖ്യ (*fn)(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ domain_offset,
						   പൂർണ്ണസംഖ്യ region_offset, u32 region_access,
						   ശൂന്യം *opaque))
{
	തിരികെ fdt_iterate_each_memregion((ശൂന്യം *)fdt, domain_offset, opaque,
					  (പൂർണ്ണസംഖ്യ (*)(ശൂന്യം *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, u32, ശൂന്യം *))fn);
}

ഘടന __fixup_find_domain_offset_info {
	സ്ഥിരം അക്ഷരം *name;
	പൂർണ്ണസംഖ്യ *doffset;
};

സ്ഥിതം പൂർണ്ണസംഖ്യ __fixup_find_domain_offset(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ doff, ശൂന്യം *p)
{
	ഘടന __fixup_find_domain_offset_info *fdo = p;

	എങ്കിൽ (!strncmp(fdo->name, fdt_get_name(fdt, doff, ശൂന്യ), strlen(fdo->name)))
		*fdo->doffset = doff;

	തിരികെ 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

സ്ഥിതം പൂർണ്ണസംഖ്യ __fixup_count_disable_devices(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ doff, പൂർണ്ണസംഖ്യ roff,
					 u32 perm, ശൂന്യം *p)
{
	പൂർണ്ണസംഖ്യ len;
	u32 *dcount = p;

	എങ്കിൽ (perm & DISABLE_DEVICES_MASK)
		തിരികെ 0;

	len = 0;
	എങ്കിൽ (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / വലുപ്പം(u32);

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ __fixup_disable_devices(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ doff, പൂർണ്ണസംഖ്യ roff,
				   u32 raccess, ശൂന്യം *p)
{
	പൂർണ്ണസംഖ്യ i, len, coff;
	സ്ഥിരം u32 *devices;

	എങ്കിൽ (raccess & DISABLE_DEVICES_MASK)
		തിരികെ 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	എങ്കിൽ (!devices)
		തിരികെ 0;
	len = len / വലുപ്പം(u32);

	വേണ്ടി (i = 0; i < len; i++) {
		coff = fdt_കണ്ണി_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		എങ്കിൽ (coff < 0)
			തിരികെ coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	തിരികെ 0;
}

ശൂന്യം fdt_domain_fixup(ശൂന്യം *fdt)
{
	u32 dcount;
	പൂർണ്ണസംഖ്യ err, poffset, doffset;
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();
	ഘടന __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	എങ്കിൽ (!dom->index)
		പോകുക skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	എങ്കിൽ (doffset < 0)
		പോകുക skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	എങ്കിൽ (!dcount)
		പോകുക skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	എങ്കിൽ (err < 0)
		തിരികെ;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	എങ്കിൽ (doffset < 0)
		പോകുക skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, ശൂന്യ,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	എങ്കിൽ (poffset < 0)
		തിരികെ;
	poffset = fdt_കണ്ണി_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	എങ്കിൽ (poffset < 0)
		തിരികെ;
	fdt_nop_കണ്ണി(fdt, poffset);
}

#define FDT_DOMAIN_REGION_MAX_എണ്ണം		16
#define FDT_വേര്_REGION_INHERIT_M_ONLY		0
#define FDT_വേര്_REGION_INHERIT_ALL		1

ഘടന parse_region_data {
	ഘടന sbi_domain *dom;
	u32 region_എണ്ണം;
	u32 max_regions;
};

സ്ഥിതം പൂർണ്ണസംഖ്യ __fdt_parse_region(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ domain_offset,
			      പൂർണ്ണസംഖ്യ region_offset, u32 region_access,
			      ശൂന്യം *opaque)
{
	പൂർണ്ണസംഖ്യ len;
	u32 val32;
	u64 val64;
	സ്ഥിരം u32 *val;
	unsigned long base, order, flags;
	ഘടന parse_region_data *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	എങ്കിൽ (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		തിരികെ SBI_EINVAL;

	/* Find next region of the domain */
	എങ്കിൽ (preg->max_regions <= preg->region_എണ്ണം)
		തിരികെ SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	എങ്കിൽ (!val || len != 8)
		തിരികെ SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	എങ്കിൽ (!val || len != 4)
		തിരികെ SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	എങ്കിൽ (val32 < 3 || __riscv_xlen < val32)
		തിരികെ SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	എങ്കിൽ (fdt_get_property(fdt, region_offset, "mmio", ശൂന്യ))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_എണ്ണം]);

	preg->region_എണ്ണം++;

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ __fdt_parse_domain(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ domain_offset, ശൂന്യം *opaque)
{
	u32 val32;
	u64 val64;
	സ്ഥിരം u32 *val;
	സ്ഥിരം അക്ഷരം *inherit;
	ഘടന sbi_domain *dom;
	ഘടന sbi_hartmask *mask;
	ഘടന parse_region_data preg;
	ഘടന sbi_domain_memregion *reg;
	പൂർണ്ണസംഖ്യ inheritance_mode = FDT_വേര്_REGION_INHERIT_M_ONLY;
	പൂർണ്ണസംഖ്യ i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(വലുപ്പം(*dom));
	എങ്കിൽ (!dom)
		തിരികെ SBI_ENOMEM;

	dom->regions = sbi_calloc(വലുപ്പം(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_എണ്ണം + 1);
	എങ്കിൽ (!dom->regions) {
		err = SBI_ENOMEM;
		പോകുക fail_free_domain;
	}
	preg.dom = dom;
	preg.region_എണ്ണം = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_എണ്ണം;

	mask = sbi_zalloc(വലുപ്പം(*mask));
	എങ്കിൽ (!mask) {
		err = SBI_ENOMEM;
		പോകുക fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_offset, ശൂന്യ),
		    വലുപ്പം(dom->name));
	dom->name[വലുപ്പം(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / വലുപ്പം(u32);
	എങ്കിൽ (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	അല്ലെങ്കിൽ
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / വലുപ്പം(u32);
	എങ്കിൽ (val && len) {
		വേണ്ടി (i = 0; i < len; i++) {
			cpu_offset = fdt_കണ്ണി_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			എങ്കിൽ (cpu_offset < 0) {
				err = cpu_offset;
				പോകുക fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			എങ്കിൽ (err)
				പോകുക fail_free_all;

			എങ്കിൽ (!fdt_കണ്ണി_is_enabled(fdt, cpu_offset))
				തുടരുക;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	എങ്കിൽ (err)
		പോകുക fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	എങ്കിൽ (inherit && len > 0) {
		എങ്കിൽ (!strcmp(inherit, "all"))
			inheritance_mode = FDT_വേര്_REGION_INHERIT_ALL;
		അല്ലെങ്കിൽ എങ്കിൽ (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_വേര്_REGION_INHERIT_M_ONLY;
		അല്ലെങ്കിൽ {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			പോകുക fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&വേര്, reg) {
		bool copy = false;

		തിരഞ്ഞെടുപ്പ് (inheritance_mode) {
		അവസ്ഥ FDT_വേര്_REGION_INHERIT_ALL:
			copy = true;
			നിർത്തുക;
		അവസ്ഥ FDT_വേര്_REGION_INHERIT_M_ONLY:
			എങ്കിൽ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			നിർത്തുക;
		}

		എങ്കിൽ (!copy)
			തുടരുക;

		എങ്കിൽ (preg.max_regions <= preg.region_എണ്ണം) {
			err = SBI_EINVAL;
			പോകുക fail_free_all;
		}
		memcpy(&dom->regions[preg.region_എണ്ണം++], reg, വലുപ്പം(*reg));
	}
	dom->fw_region_inited = വേര്.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	എങ്കിൽ (val && len >= 4) {
		cpu_offset = fdt_കണ്ണി_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		എങ്കിൽ (cpu_offset >= 0 && fdt_കണ്ണി_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	എങ്കിൽ (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} അല്ലെങ്കിൽ {
		val64 = വേര്.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	എങ്കിൽ (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} അല്ലെങ്കിൽ {
		val64 = വേര്.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	എങ്കിൽ (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		എങ്കിൽ (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} അല്ലെങ്കിൽ {
		val32 = വേര്.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	എങ്കിൽ (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", ശൂന്യ))
		dom->system_reset_allowed = true;
	അല്ലെങ്കിൽ
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	എങ്കിൽ (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", ശൂന്യ))
		dom->system_suspend_allowed = true;
	അല്ലെങ്കിൽ
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	എങ്കിൽ (err)
		പോകുക fail_free_all;

	തിരികെ 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	തിരികെ err;
}

പൂർണ്ണസംഖ്യ fdt_domains_populate(സ്ഥിരം ശൂന്യം *fdt)
{
	/* Sanity checks */
	എങ്കിൽ (!fdt)
		തിരികെ SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	തിരികെ fdt_iterate_each_domain_ro(fdt, ശൂന്യ, __fdt_parse_domain);
}
