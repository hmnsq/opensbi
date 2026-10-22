// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <libfdt_env.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hartmask.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_domain.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>

முழு fdt_iterate_each_domain(வெறுமை *fdt, வெறுமை *opaque,
			    முழு (*fn)(வெறுமை *fdt, முழு domain_offset,
				       வெறுமை *opaque))
{
	முழு rc, doffset, poffset;

	எனில் (!fdt || !fn)
		திரும்பு SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	எனில் (poffset < 0)
		திரும்பு 0;
	poffset = fdt_முனை_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	எனில் (poffset < 0)
		திரும்பு 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		எனில் (fdt_முனை_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			தொடர்;

		rc = fn(fdt, doffset, opaque);
		எனில் (rc)
			திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான முழு fdt_iterate_each_domain_ro(மாறிலி வெறுமை *fdt, வெறுமை *opaque,
				      முழு (*fn)(மாறிலி வெறுமை *fdt, முழு domain_offset,
						வெறுமை *opaque))
{
	திரும்பு fdt_iterate_each_domain((வெறுமை *)fdt, opaque,
				       (முழு (*)(வெறுமை *, முழு,  வெறுமை *))fn);
}

முழு fdt_iterate_each_memregion(வெறுமை *fdt, முழு domain_offset, வெறுமை *opaque,
			       முழு (*fn)(வெறுமை *fdt, முழு domain_offset,
					 முழு region_offset, u32 region_access,
					 வெறுமை *opaque))
{
	u32 i, rcount;
	முழு rc, len, region_offset;
	மாறிலி u32 *regions;

	எனில் (!fdt || (domain_offset < 0) || !fn)
		திரும்பு SBI_EINVAL;

	எனில் (fdt_முனை_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		திரும்பு SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	எனில் (!regions)
		திரும்பு 0;

	rcount = (u32)len / (அளவு(u32) * 2);
	க்கு (i = 0; i < rcount; i++) {
		region_offset = fdt_முனை_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		எனில் (region_offset < 0)
			திரும்பு region_offset;

		எனில் (fdt_முனை_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			திரும்பு SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		எனில் (rc)
			திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான முழு fdt_iterate_each_memregion_ro(மாறிலி வெறுமை *fdt, முழு domain_offset, வெறுமை *opaque,
					 முழு (*fn)(மாறிலி வெறுமை *fdt, முழு domain_offset,
						   முழு region_offset, u32 region_access,
						   வெறுமை *opaque))
{
	திரும்பு fdt_iterate_each_memregion((வெறுமை *)fdt, domain_offset, opaque,
					  (முழு (*)(வெறுமை *, முழு, முழு, u32, வெறுமை *))fn);
}

கட்டமைப்பு __fixup_find_domain_offset_info {
	மாறிலி எழுத்து *name;
	முழு *doffset;
};

நிலையான முழு __fixup_find_domain_offset(வெறுமை *fdt, முழு doff, வெறுமை *p)
{
	கட்டமைப்பு __fixup_find_domain_offset_info *fdo = p;

	எனில் (!strncmp(fdo->name, fdt_get_name(fdt, doff, வெற்று), strlen(fdo->name)))
		*fdo->doffset = doff;

	திரும்பு 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

நிலையான முழு __fixup_count_disable_devices(வெறுமை *fdt, முழு doff, முழு roff,
					 u32 perm, வெறுமை *p)
{
	முழு len;
	u32 *dcount = p;

	எனில் (perm & DISABLE_DEVICES_MASK)
		திரும்பு 0;

	len = 0;
	எனில் (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / அளவு(u32);

	திரும்பு 0;
}

நிலையான முழு __fixup_disable_devices(வெறுமை *fdt, முழு doff, முழு roff,
				   u32 raccess, வெறுமை *p)
{
	முழு i, len, coff;
	மாறிலி u32 *devices;

	எனில் (raccess & DISABLE_DEVICES_MASK)
		திரும்பு 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	எனில் (!devices)
		திரும்பு 0;
	len = len / அளவு(u32);

	க்கு (i = 0; i < len; i++) {
		coff = fdt_முனை_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		எனில் (coff < 0)
			திரும்பு coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	திரும்பு 0;
}

வெறுமை fdt_domain_fixup(வெறுமை *fdt)
{
	u32 dcount;
	முழு err, poffset, doffset;
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();
	கட்டமைப்பு __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	எனில் (!dom->index)
		செல் skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	எனில் (doffset < 0)
		செல் skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	எனில் (!dcount)
		செல் skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	எனில் (err < 0)
		திரும்பு;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	எனில் (doffset < 0)
		செல் skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, வெற்று,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	எனில் (poffset < 0)
		திரும்பு;
	poffset = fdt_முனை_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	எனில் (poffset < 0)
		திரும்பு;
	fdt_nop_முனை(fdt, poffset);
}

#define FDT_DOMAIN_REGION_MAX_மொத்தம்		16
#define FDT_வேர்_REGION_INHERIT_M_ONLY		0
#define FDT_வேர்_REGION_INHERIT_ALL		1

கட்டமைப்பு parse_region_data {
	கட்டமைப்பு sbi_domain *dom;
	u32 region_மொத்தம்;
	u32 max_regions;
};

நிலையான முழு __fdt_parse_region(மாறிலி வெறுமை *fdt, முழு domain_offset,
			      முழு region_offset, u32 region_access,
			      வெறுமை *opaque)
{
	முழு len;
	u32 val32;
	u64 val64;
	மாறிலி u32 *val;
	unsigned long base, order, flags;
	கட்டமைப்பு parse_region_data *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	எனில் (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		திரும்பு SBI_EINVAL;

	/* Find next region of the domain */
	எனில் (preg->max_regions <= preg->region_மொத்தம்)
		திரும்பு SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	எனில் (!val || len != 8)
		திரும்பு SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	எனில் (!val || len != 4)
		திரும்பு SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	எனில் (val32 < 3 || __riscv_xlen < val32)
		திரும்பு SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	எனில் (fdt_get_property(fdt, region_offset, "mmio", வெற்று))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_மொத்தம்]);

	preg->region_மொத்தம்++;

	திரும்பு 0;
}

நிலையான முழு __fdt_parse_domain(மாறிலி வெறுமை *fdt, முழு domain_offset, வெறுமை *opaque)
{
	u32 val32;
	u64 val64;
	மாறிலி u32 *val;
	மாறிலி எழுத்து *inherit;
	கட்டமைப்பு sbi_domain *dom;
	கட்டமைப்பு sbi_hartmask *mask;
	கட்டமைப்பு parse_region_data preg;
	கட்டமைப்பு sbi_domain_memregion *reg;
	முழு inheritance_mode = FDT_வேர்_REGION_INHERIT_M_ONLY;
	முழு i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(அளவு(*dom));
	எனில் (!dom)
		திரும்பு SBI_ENOMEM;

	dom->regions = sbi_calloc(அளவு(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_மொத்தம் + 1);
	எனில் (!dom->regions) {
		err = SBI_ENOMEM;
		செல் fail_free_domain;
	}
	preg.dom = dom;
	preg.region_மொத்தம் = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_மொத்தம்;

	mask = sbi_zalloc(அளவு(*mask));
	எனில் (!mask) {
		err = SBI_ENOMEM;
		செல் fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_offset, வெற்று),
		    அளவு(dom->name));
	dom->name[அளவு(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / அளவு(u32);
	எனில் (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	இல்லையெனில்
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / அளவு(u32);
	எனில் (val && len) {
		க்கு (i = 0; i < len; i++) {
			cpu_offset = fdt_முனை_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			எனில் (cpu_offset < 0) {
				err = cpu_offset;
				செல் fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			எனில் (err)
				செல் fail_free_all;

			எனில் (!fdt_முனை_is_enabled(fdt, cpu_offset))
				தொடர்;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	எனில் (err)
		செல் fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	எனில் (inherit && len > 0) {
		எனில் (!strcmp(inherit, "all"))
			inheritance_mode = FDT_வேர்_REGION_INHERIT_ALL;
		இல்லையெனில் எனில் (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_வேர்_REGION_INHERIT_M_ONLY;
		இல்லையெனில் {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			செல் fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&வேர், reg) {
		bool copy = false;

		தேர்வு (inheritance_mode) {
		நிலை FDT_வேர்_REGION_INHERIT_ALL:
			copy = true;
			நிறுத்து;
		நிலை FDT_வேர்_REGION_INHERIT_M_ONLY:
			எனில் (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			நிறுத்து;
		}

		எனில் (!copy)
			தொடர்;

		எனில் (preg.max_regions <= preg.region_மொத்தம்) {
			err = SBI_EINVAL;
			செல் fail_free_all;
		}
		memcpy(&dom->regions[preg.region_மொத்தம்++], reg, அளவு(*reg));
	}
	dom->fw_region_inited = வேர்.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	எனில் (val && len >= 4) {
		cpu_offset = fdt_முனை_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		எனில் (cpu_offset >= 0 && fdt_முனை_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	எனில் (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} இல்லையெனில் {
		val64 = வேர்.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	எனில் (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} இல்லையெனில் {
		val64 = வேர்.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	எனில் (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		எனில் (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} இல்லையெனில் {
		val32 = வேர்.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	எனில் (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", வெற்று))
		dom->system_reset_allowed = true;
	இல்லையெனில்
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	எனில் (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", வெற்று))
		dom->system_suspend_allowed = true;
	இல்லையெனில்
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	எனில் (err)
		செல் fail_free_all;

	திரும்பு 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	திரும்பு err;
}

முழு fdt_domains_populate(மாறிலி வெறுமை *fdt)
{
	/* Sanity checks */
	எனில் (!fdt)
		திரும்பு SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	திரும்பு fdt_iterate_each_domain_ro(fdt, வெற்று, __fdt_parse_domain);
}
