// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <libfdt_env.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hartmask.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_domain.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>

પૂર્ણાંક fdt_iterate_each_domain(શૂન્ય *fdt, શૂન્ય *opaque,
			    પૂર્ણાંક (*fn)(શૂન્ય *fdt, પૂર્ણાંક domain_offset,
				       શૂન્ય *opaque))
{
	પૂર્ણાંક rc, doffset, poffset;

	જો (!fdt || !fn)
		પાછા SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	જો (poffset < 0)
		પાછા 0;
	poffset = fdt_ગાંઠ_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	જો (poffset < 0)
		પાછા 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		જો (fdt_ગાંઠ_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			ચાલુ;

		rc = fn(fdt, doffset, opaque);
		જો (rc)
			પાછા rc;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક fdt_iterate_each_domain_ro(અચલ શૂન્ય *fdt, શૂન્ય *opaque,
				      પૂર્ણાંક (*fn)(અચલ શૂન્ય *fdt, પૂર્ણાંક domain_offset,
						શૂન્ય *opaque))
{
	પાછા fdt_iterate_each_domain((શૂન્ય *)fdt, opaque,
				       (પૂર્ણાંક (*)(શૂન્ય *, પૂર્ણાંક,  શૂન્ય *))fn);
}

પૂર્ણાંક fdt_iterate_each_memregion(શૂન્ય *fdt, પૂર્ણાંક domain_offset, શૂન્ય *opaque,
			       પૂર્ણાંક (*fn)(શૂન્ય *fdt, પૂર્ણાંક domain_offset,
					 પૂર્ણાંક region_offset, u32 region_access,
					 શૂન્ય *opaque))
{
	u32 i, rcount;
	પૂર્ણાંક rc, len, region_offset;
	અચલ u32 *regions;

	જો (!fdt || (domain_offset < 0) || !fn)
		પાછા SBI_EINVAL;

	જો (fdt_ગાંઠ_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		પાછા SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	જો (!regions)
		પાછા 0;

	rcount = (u32)len / (કદ(u32) * 2);
	માટે (i = 0; i < rcount; i++) {
		region_offset = fdt_ગાંઠ_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		જો (region_offset < 0)
			પાછા region_offset;

		જો (fdt_ગાંઠ_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			પાછા SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		જો (rc)
			પાછા rc;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક fdt_iterate_each_memregion_ro(અચલ શૂન્ય *fdt, પૂર્ણાંક domain_offset, શૂન્ય *opaque,
					 પૂર્ણાંક (*fn)(અચલ શૂન્ય *fdt, પૂર્ણાંક domain_offset,
						   પૂર્ણાંક region_offset, u32 region_access,
						   શૂન્ય *opaque))
{
	પાછા fdt_iterate_each_memregion((શૂન્ય *)fdt, domain_offset, opaque,
					  (પૂર્ણાંક (*)(શૂન્ય *, પૂર્ણાંક, પૂર્ણાંક, u32, શૂન્ય *))fn);
}

સંરચના __fixup_find_domain_offset_info {
	અચલ અક્ષર *name;
	પૂર્ણાંક *doffset;
};

સ્થિર પૂર્ણાંક __fixup_find_domain_offset(શૂન્ય *fdt, પૂર્ણાંક doff, શૂન્ય *p)
{
	સંરચના __fixup_find_domain_offset_info *fdo = p;

	જો (!strncmp(fdo->name, fdt_get_name(fdt, doff, ખાલી), strlen(fdo->name)))
		*fdo->doffset = doff;

	પાછા 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

સ્થિર પૂર્ણાંક __fixup_count_disable_devices(શૂન્ય *fdt, પૂર્ણાંક doff, પૂર્ણાંક roff,
					 u32 perm, શૂન્ય *p)
{
	પૂર્ણાંક len;
	u32 *dcount = p;

	જો (perm & DISABLE_DEVICES_MASK)
		પાછા 0;

	len = 0;
	જો (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / કદ(u32);

	પાછા 0;
}

સ્થિર પૂર્ણાંક __fixup_disable_devices(શૂન્ય *fdt, પૂર્ણાંક doff, પૂર્ણાંક roff,
				   u32 raccess, શૂન્ય *p)
{
	પૂર્ણાંક i, len, coff;
	અચલ u32 *devices;

	જો (raccess & DISABLE_DEVICES_MASK)
		પાછા 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	જો (!devices)
		પાછા 0;
	len = len / કદ(u32);

	માટે (i = 0; i < len; i++) {
		coff = fdt_ગાંઠ_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		જો (coff < 0)
			પાછા coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	પાછા 0;
}

શૂન્ય fdt_domain_fixup(શૂન્ય *fdt)
{
	u32 dcount;
	પૂર્ણાંક err, poffset, doffset;
	સંરચના sbi_domain *dom = sbi_domain_thishart_ptr();
	સંરચના __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	જો (!dom->index)
		જાઓ skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	જો (doffset < 0)
		જાઓ skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	જો (!dcount)
		જાઓ skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	જો (err < 0)
		પાછા;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	જો (doffset < 0)
		જાઓ skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, ખાલી,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	જો (poffset < 0)
		પાછા;
	poffset = fdt_ગાંઠ_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	જો (poffset < 0)
		પાછા;
	fdt_nop_ગાંઠ(fdt, poffset);
}

#define FDT_DOMAIN_REGION_MAX_ગણતરી		16
#define FDT_મૂળ_REGION_INHERIT_M_ONLY		0
#define FDT_મૂળ_REGION_INHERIT_ALL		1

સંરચના parse_region_data {
	સંરચના sbi_domain *dom;
	u32 region_ગણતરી;
	u32 max_regions;
};

સ્થિર પૂર્ણાંક __fdt_parse_region(અચલ શૂન્ય *fdt, પૂર્ણાંક domain_offset,
			      પૂર્ણાંક region_offset, u32 region_access,
			      શૂન્ય *opaque)
{
	પૂર્ણાંક len;
	u32 val32;
	u64 val64;
	અચલ u32 *val;
	unsigned long base, order, flags;
	સંરચના parse_region_data *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	જો (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		પાછા SBI_EINVAL;

	/* Find next region of the domain */
	જો (preg->max_regions <= preg->region_ગણતરી)
		પાછા SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	જો (!val || len != 8)
		પાછા SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	જો (!val || len != 4)
		પાછા SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	જો (val32 < 3 || __riscv_xlen < val32)
		પાછા SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	જો (fdt_get_property(fdt, region_offset, "mmio", ખાલી))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_ગણતરી]);

	preg->region_ગણતરી++;

	પાછા 0;
}

સ્થિર પૂર્ણાંક __fdt_parse_domain(અચલ શૂન્ય *fdt, પૂર્ણાંક domain_offset, શૂન્ય *opaque)
{
	u32 val32;
	u64 val64;
	અચલ u32 *val;
	અચલ અક્ષર *inherit;
	સંરચના sbi_domain *dom;
	સંરચના sbi_hartmask *mask;
	સંરચના parse_region_data preg;
	સંરચના sbi_domain_memregion *reg;
	પૂર્ણાંક inheritance_mode = FDT_મૂળ_REGION_INHERIT_M_ONLY;
	પૂર્ણાંક i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(કદ(*dom));
	જો (!dom)
		પાછા SBI_ENOMEM;

	dom->regions = sbi_calloc(કદ(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_ગણતરી + 1);
	જો (!dom->regions) {
		err = SBI_ENOMEM;
		જાઓ fail_free_domain;
	}
	preg.dom = dom;
	preg.region_ગણતરી = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_ગણતરી;

	mask = sbi_zalloc(કદ(*mask));
	જો (!mask) {
		err = SBI_ENOMEM;
		જાઓ fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_offset, ખાલી),
		    કદ(dom->name));
	dom->name[કદ(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / કદ(u32);
	જો (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	નહીંતર
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / કદ(u32);
	જો (val && len) {
		માટે (i = 0; i < len; i++) {
			cpu_offset = fdt_ગાંઠ_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			જો (cpu_offset < 0) {
				err = cpu_offset;
				જાઓ fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			જો (err)
				જાઓ fail_free_all;

			જો (!fdt_ગાંઠ_is_enabled(fdt, cpu_offset))
				ચાલુ;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	જો (err)
		જાઓ fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	જો (inherit && len > 0) {
		જો (!strcmp(inherit, "all"))
			inheritance_mode = FDT_મૂળ_REGION_INHERIT_ALL;
		નહીંતર જો (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_મૂળ_REGION_INHERIT_M_ONLY;
		નહીંતર {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			જાઓ fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&મૂળ, reg) {
		bool copy = false;

		પસંદગી (inheritance_mode) {
		સ્થિતિ FDT_મૂળ_REGION_INHERIT_ALL:
			copy = true;
			તોડો;
		સ્થિતિ FDT_મૂળ_REGION_INHERIT_M_ONLY:
			જો (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			તોડો;
		}

		જો (!copy)
			ચાલુ;

		જો (preg.max_regions <= preg.region_ગણતરી) {
			err = SBI_EINVAL;
			જાઓ fail_free_all;
		}
		memcpy(&dom->regions[preg.region_ગણતરી++], reg, કદ(*reg));
	}
	dom->fw_region_inited = મૂળ.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	જો (val && len >= 4) {
		cpu_offset = fdt_ગાંઠ_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		જો (cpu_offset >= 0 && fdt_ગાંઠ_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	જો (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} નહીંતર {
		val64 = મૂળ.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	જો (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} નહીંતર {
		val64 = મૂળ.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	જો (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		જો (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} નહીંતર {
		val32 = મૂળ.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	જો (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", ખાલી))
		dom->system_reset_allowed = true;
	નહીંતર
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	જો (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", ખાલી))
		dom->system_suspend_allowed = true;
	નહીંતર
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	જો (err)
		જાઓ fail_free_all;

	પાછા 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	પાછા err;
}

પૂર્ણાંક fdt_domains_populate(અચલ શૂન્ય *fdt)
{
	/* Sanity checks */
	જો (!fdt)
		પાછા SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	પાછા fdt_iterate_each_domain_ro(fdt, ખાલી, __fdt_parse_domain);
}
