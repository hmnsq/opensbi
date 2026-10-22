// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <libfdt.h>
#include <libfdt_env.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hartmask.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_scratch.h>
#include <sbi_utils/fdt/fdt_domain.h>
#include <sbi_utils/fdt/fdt_helper.h>

පූර්ණ_සංඛ්‍යා fdt_iterate_each_domain(void *fdt, void *opaque,
			    පූර්ණ_සංඛ්‍යා (*fn)(void *fdt, පූර්ණ_සංඛ්‍යා domain_offset,
				       void *opaque))
{
	පූර්ණ_සංඛ්‍යා rc, doffset, poffset;

	නම් (!fdt || !fn)
		ආපසු SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	නම් (poffset < 0)
		ආපසු 0;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	නම් (poffset < 0)
		ආපසු 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		නම් (fdt_node_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			continue;

		rc = fn(fdt, doffset, opaque);
		නම් (rc)
			ආපසු rc;
	}

	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා fdt_iterate_each_domain_ro(const void *fdt, void *opaque,
				      පූර්ණ_සංඛ්‍යා (*fn)(const void *fdt, පූර්ණ_සංඛ්‍යා domain_offset,
						void *opaque))
{
	ආපසු fdt_iterate_each_domain((void *)fdt, opaque,
				       (පූර්ණ_සංඛ්‍යා (*)(void *, පූර්ණ_සංඛ්‍යා,  void *))fn);
}

පූර්ණ_සංඛ්‍යා fdt_iterate_each_memregion(void *fdt, පූර්ණ_සංඛ්‍යා domain_offset, void *opaque,
			       පූර්ණ_සංඛ්‍යා (*fn)(void *fdt, පූර්ණ_සංඛ්‍යා domain_offset,
					 පූර්ණ_සංඛ්‍යා region_offset, u32 region_access,
					 void *opaque))
{
	u32 i, rcount;
	පූර්ණ_සංඛ්‍යා rc, len, region_offset;
	const u32 *regions;

	නම් (!fdt || (domain_offset < 0) || !fn)
		ආපසු SBI_EINVAL;

	නම් (fdt_node_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		ආපසු SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	නම් (!regions)
		ආපසු 0;

	rcount = (u32)len / (parimaa_na(u32) * 2);
	සඳහා (i = 0; i < rcount; i++) {
		region_offset = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		නම් (region_offset < 0)
			ආපසු region_offset;

		නම් (fdt_node_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			ආපසු SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		නම් (rc)
			ආපසු rc;
	}

	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා fdt_iterate_each_memregion_ro(const void *fdt, පූර්ණ_සංඛ්‍යා domain_offset, void *opaque,
					 පූර්ණ_සංඛ්‍යා (*fn)(const void *fdt, පූර්ණ_සංඛ්‍යා domain_offset,
						   පූර්ණ_සංඛ්‍යා region_offset, u32 region_access,
						   void *opaque))
{
	ආපසු fdt_iterate_each_memregion((void *)fdt, domain_offset, opaque,
					  (පූර්ණ_සංඛ්‍යා (*)(void *, පූර්ණ_සංඛ්‍යා, පූර්ණ_සංඛ්‍යා, u32, void *))fn);
}

sa_nracanaa __fixup_find_domain_offset_info {
	const var_na *name;
	පූර්ණ_සංඛ්‍යා *doffset;
};

static පූර්ණ_සංඛ්‍යා __fixup_find_domain_offset(void *fdt, පූර්ණ_සංඛ්‍යා doff, void *p)
{
	sa_nracanaa __fixup_find_domain_offset_info *fdo = p;

	නම් (!strncmp(fdo->name, fdt_get_name(fdt, doff, NULL), strlen(fdo->name)))
		*fdo->doffset = doff;

	ආපසු 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

static පූර්ණ_සංඛ්‍යා __fixup_count_disable_devices(void *fdt, පූර්ණ_සංඛ්‍යා doff, පූර්ණ_සංඛ්‍යා roff,
					 u32 perm, void *p)
{
	පූර්ණ_සංඛ්‍යා len;
	u32 *dcount = p;

	නම් (perm & DISABLE_DEVICES_MASK)
		ආපසු 0;

	len = 0;
	නම් (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / parimaa_na(u32);

	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා __fixup_disable_devices(void *fdt, පූර්ණ_සංඛ්‍යා doff, පූර්ණ_සංඛ්‍යා roff,
				   u32 raccess, void *p)
{
	පූර්ණ_සංඛ්‍යා i, len, coff;
	const u32 *devices;

	නම් (raccess & DISABLE_DEVICES_MASK)
		ආපසු 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	නම් (!devices)
		ආපසු 0;
	len = len / parimaa_na(u32);

	සඳහා (i = 0; i < len; i++) {
		coff = fdt_node_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		නම් (coff < 0)
			ආපසු coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	ආපසු 0;
}

void fdt_domain_fixup(void *fdt)
{
	u32 dcount;
	පූර්ණ_සංඛ්‍යා err, poffset, doffset;
	sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();
	sa_nracanaa __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	නම් (!dom->index)
		goto skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	නම් (doffset < 0)
		goto skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	නම් (!dcount)
		goto skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	නම් (err < 0)
		ආපසු;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	නම් (doffset < 0)
		goto skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, NULL,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	නම් (poffset < 0)
		ආපසු;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	නම් (poffset < 0)
		ආපසු;
	fdt_nop_node(fdt, poffset);
}

#define FDT_DOMAIN_REGION_MAX_COUNT		16
#define FDT_ROOT_REGION_INHERIT_M_ONLY		0
#define FDT_ROOT_REGION_INHERIT_ALL		1

sa_nracanaa parse_region_data {
	sa_nracanaa sbi_domain *dom;
	u32 region_count;
	u32 max_regions;
};

static පූර්ණ_සංඛ්‍යා __fdt_parse_region(const void *fdt, පූර්ණ_සංඛ්‍යා domain_offset,
			      පූර්ණ_සංඛ්‍යා region_offset, u32 region_access,
			      void *opaque)
{
	පූර්ණ_සංඛ්‍යා len;
	u32 val32;
	u64 val64;
	const u32 *val;
	unsigned long base, order, flags;
	sa_nracanaa parse_region_data *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	නම් (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		ආපසු SBI_EINVAL;

	/* Find next region of the domain */
	නම් (preg->max_regions <= preg->region_count)
		ආපසු SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	නම් (!val || len != 8)
		ආපසු SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	නම් (!val || len != 4)
		ආපසු SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	නම් (val32 < 3 || __riscv_xlen < val32)
		ආපසු SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	නම් (fdt_get_property(fdt, region_offset, "mmio", NULL))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_count]);

	preg->region_count++;

	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා __fdt_parse_domain(const void *fdt, පූර්ණ_සංඛ්‍යා domain_offset, void *opaque)
{
	u32 val32;
	u64 val64;
	const u32 *val;
	const var_na *inherit;
	sa_nracanaa sbi_domain *dom;
	sa_nracanaa sbi_hartmask *mask;
	sa_nracanaa parse_region_data preg;
	sa_nracanaa sbi_domain_memregion *reg;
	පූර්ණ_සංඛ්‍යා inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
	පූර්ණ_සංඛ්‍යා i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(parimaa_na(*dom));
	නම් (!dom)
		ආපසු SBI_ENOMEM;

	dom->regions = sbi_calloc(parimaa_na(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_COUNT + 1);
	නම් (!dom->regions) {
		err = SBI_ENOMEM;
		goto fail_free_domain;
	}
	preg.dom = dom;
	preg.region_count = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_COUNT;

	mask = sbi_zalloc(parimaa_na(*mask));
	නම් (!mask) {
		err = SBI_ENOMEM;
		goto fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_offset, NULL),
		    parimaa_na(dom->name));
	dom->name[parimaa_na(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / parimaa_na(u32);
	නම් (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	නැතහොත්
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / parimaa_na(u32);
	නම් (val && len) {
		සඳහා (i = 0; i < len; i++) {
			cpu_offset = fdt_node_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			නම් (cpu_offset < 0) {
				err = cpu_offset;
				goto fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			නම් (err)
				goto fail_free_all;

			නම් (!fdt_node_is_enabled(fdt, cpu_offset))
				continue;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	නම් (err)
		goto fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	නම් (inherit && len > 0) {
		නම් (!strcmp(inherit, "all"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_ALL;
		නැතහොත් නම් (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
		නැතහොත් {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			goto fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&root, reg) {
		bool copy = false;

		switch (inheritance_mode) {
		pak_sa FDT_ROOT_REGION_INHERIT_ALL:
			copy = true;
			bha_nga;
		pak_sa FDT_ROOT_REGION_INHERIT_M_ONLY:
			නම් (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			bha_nga;
		}

		නම් (!copy)
			continue;

		නම් (preg.max_regions <= preg.region_count) {
			err = SBI_EINVAL;
			goto fail_free_all;
		}
		memcpy(&dom->regions[preg.region_count++], reg, parimaa_na(*reg));
	}
	dom->fw_region_inited = root.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	නම් (val && len >= 4) {
		cpu_offset = fdt_node_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		නම් (cpu_offset >= 0 && fdt_node_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	නම් (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} නැතහොත් {
		val64 = root.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	නම් (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} නැතහොත් {
		val64 = root.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	නම් (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		නම් (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} නැතහොත් {
		val32 = root.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	නම් (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", NULL))
		dom->system_reset_allowed = true;
	නැතහොත්
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	නම් (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", NULL))
		dom->system_suspend_allowed = true;
	නැතහොත්
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	නම් (err)
		goto fail_free_all;

	ආපසු 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	ආපසු err;
}

පූර්ණ_සංඛ්‍යා fdt_domains_populate(const void *fdt)
{
	/* Sanity checks */
	නම් (!fdt)
		ආපසු SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	ආපසු fdt_iterate_each_domain_ro(fdt, NULL, __fdt_parse_domain);
}
