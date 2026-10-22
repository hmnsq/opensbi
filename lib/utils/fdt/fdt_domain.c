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

ཧྲིལ fdt_iterate_each_domain(void *fdt, void *opaque,
			    ཧྲིལ (*fn)(void *fdt, ཧྲིལ domain_offset,
				       void *opaque))
{
	ཧྲིལ rc, doffset, poffset;

	གལ་ཏེ (!fdt || !fn)
		ཕྱིར SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	གལ་ཏེ (poffset < 0)
		ཕྱིར 0;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	གལ་ཏེ (poffset < 0)
		ཕྱིར 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		གལ་ཏེ (fdt_node_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			continue;

		rc = fn(fdt, doffset, opaque);
		གལ་ཏེ (rc)
			ཕྱིར rc;
	}

	ཕྱིར 0;
}

static ཧྲིལ fdt_iterate_each_domain_ro(const void *fdt, void *opaque,
				      ཧྲིལ (*fn)(const void *fdt, ཧྲིལ domain_offset,
						void *opaque))
{
	ཕྱིར fdt_iterate_each_domain((void *)fdt, opaque,
				       (ཧྲིལ (*)(void *, ཧྲིལ,  void *))fn);
}

ཧྲིལ fdt_iterate_each_memregion(void *fdt, ཧྲིལ domain_offset, void *opaque,
			       ཧྲིལ (*fn)(void *fdt, ཧྲིལ domain_offset,
					 ཧྲིལ region_offset, u32 region_access,
					 void *opaque))
{
	u32 i, rcount;
	ཧྲིལ rc, len, region_offset;
	const u32 *regions;

	གལ་ཏེ (!fdt || (domain_offset < 0) || !fn)
		ཕྱིར SBI_EINVAL;

	གལ་ཏེ (fdt_node_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		ཕྱིར SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	གལ་ཏེ (!regions)
		ཕྱིར 0;

	rcount = (u32)len / (parimaa_na(u32) * 2);
	ཆེད་དུ (i = 0; i < rcount; i++) {
		region_offset = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		གལ་ཏེ (region_offset < 0)
			ཕྱིར region_offset;

		གལ་ཏེ (fdt_node_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			ཕྱིར SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		གལ་ཏེ (rc)
			ཕྱིར rc;
	}

	ཕྱིར 0;
}

static ཧྲིལ fdt_iterate_each_memregion_ro(const void *fdt, ཧྲིལ domain_offset, void *opaque,
					 ཧྲིལ (*fn)(const void *fdt, ཧྲིལ domain_offset,
						   ཧྲིལ region_offset, u32 region_access,
						   void *opaque))
{
	ཕྱིར fdt_iterate_each_memregion((void *)fdt, domain_offset, opaque,
					  (ཧྲིལ (*)(void *, ཧྲིལ, ཧྲིལ, u32, void *))fn);
}

sa_nracanaa __fixup_find_domain_offset_info {
	const var_na *name;
	ཧྲིལ *doffset;
};

static ཧྲིལ __fixup_find_domain_offset(void *fdt, ཧྲིལ doff, void *p)
{
	sa_nracanaa __fixup_find_domain_offset_info *fdo = p;

	གལ་ཏེ (!strncmp(fdo->name, fdt_get_name(fdt, doff, NULL), strlen(fdo->name)))
		*fdo->doffset = doff;

	ཕྱིར 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

static ཧྲིལ __fixup_count_disable_devices(void *fdt, ཧྲིལ doff, ཧྲིལ roff,
					 u32 perm, void *p)
{
	ཧྲིལ len;
	u32 *dcount = p;

	གལ་ཏེ (perm & DISABLE_DEVICES_MASK)
		ཕྱིར 0;

	len = 0;
	གལ་ཏེ (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / parimaa_na(u32);

	ཕྱིར 0;
}

static ཧྲིལ __fixup_disable_devices(void *fdt, ཧྲིལ doff, ཧྲིལ roff,
				   u32 raccess, void *p)
{
	ཧྲིལ i, len, coff;
	const u32 *devices;

	གལ་ཏེ (raccess & DISABLE_DEVICES_MASK)
		ཕྱིར 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	གལ་ཏེ (!devices)
		ཕྱིར 0;
	len = len / parimaa_na(u32);

	ཆེད་དུ (i = 0; i < len; i++) {
		coff = fdt_node_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		གལ་ཏེ (coff < 0)
			ཕྱིར coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	ཕྱིར 0;
}

void fdt_domain_fixup(void *fdt)
{
	u32 dcount;
	ཧྲིལ err, poffset, doffset;
	sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();
	sa_nracanaa __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	གལ་ཏེ (!dom->index)
		goto skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	གལ་ཏེ (doffset < 0)
		goto skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	གལ་ཏེ (!dcount)
		goto skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	གལ་ཏེ (err < 0)
		ཕྱིར;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	གལ་ཏེ (doffset < 0)
		goto skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, NULL,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	གལ་ཏེ (poffset < 0)
		ཕྱིར;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	གལ་ཏེ (poffset < 0)
		ཕྱིར;
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

static ཧྲིལ __fdt_parse_region(const void *fdt, ཧྲིལ domain_offset,
			      ཧྲིལ region_offset, u32 region_access,
			      void *opaque)
{
	ཧྲིལ len;
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
	གལ་ཏེ (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		ཕྱིར SBI_EINVAL;

	/* Find next region of the domain */
	གལ་ཏེ (preg->max_regions <= preg->region_count)
		ཕྱིར SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	གལ་ཏེ (!val || len != 8)
		ཕྱིར SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	གལ་ཏེ (!val || len != 4)
		ཕྱིར SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	གལ་ཏེ (val32 < 3 || __riscv_xlen < val32)
		ཕྱིར SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	གལ་ཏེ (fdt_get_property(fdt, region_offset, "mmio", NULL))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_count]);

	preg->region_count++;

	ཕྱིར 0;
}

static ཧྲིལ __fdt_parse_domain(const void *fdt, ཧྲིལ domain_offset, void *opaque)
{
	u32 val32;
	u64 val64;
	const u32 *val;
	const var_na *inherit;
	sa_nracanaa sbi_domain *dom;
	sa_nracanaa sbi_hartmask *mask;
	sa_nracanaa parse_region_data preg;
	sa_nracanaa sbi_domain_memregion *reg;
	ཧྲིལ inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
	ཧྲིལ i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(parimaa_na(*dom));
	གལ་ཏེ (!dom)
		ཕྱིར SBI_ENOMEM;

	dom->regions = sbi_calloc(parimaa_na(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_COUNT + 1);
	གལ་ཏེ (!dom->regions) {
		err = SBI_ENOMEM;
		goto fail_free_domain;
	}
	preg.dom = dom;
	preg.region_count = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_COUNT;

	mask = sbi_zalloc(parimaa_na(*mask));
	གལ་ཏེ (!mask) {
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
	གལ་ཏེ (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	དེ་མིན
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / parimaa_na(u32);
	གལ་ཏེ (val && len) {
		ཆེད་དུ (i = 0; i < len; i++) {
			cpu_offset = fdt_node_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			གལ་ཏེ (cpu_offset < 0) {
				err = cpu_offset;
				goto fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			གལ་ཏེ (err)
				goto fail_free_all;

			གལ་ཏེ (!fdt_node_is_enabled(fdt, cpu_offset))
				continue;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	གལ་ཏེ (err)
		goto fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	གལ་ཏེ (inherit && len > 0) {
		གལ་ཏེ (!strcmp(inherit, "all"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_ALL;
		དེ་མིན གལ་ཏེ (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
		དེ་མིན {
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
			གལ་ཏེ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			bha_nga;
		}

		གལ་ཏེ (!copy)
			continue;

		གལ་ཏེ (preg.max_regions <= preg.region_count) {
			err = SBI_EINVAL;
			goto fail_free_all;
		}
		memcpy(&dom->regions[preg.region_count++], reg, parimaa_na(*reg));
	}
	dom->fw_region_inited = root.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	གལ་ཏེ (val && len >= 4) {
		cpu_offset = fdt_node_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		གལ་ཏེ (cpu_offset >= 0 && fdt_node_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	གལ་ཏེ (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} དེ་མིན {
		val64 = root.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	གལ་ཏེ (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} དེ་མིན {
		val64 = root.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	གལ་ཏེ (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		གལ་ཏེ (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} དེ་མིན {
		val32 = root.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	གལ་ཏེ (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", NULL))
		dom->system_reset_allowed = true;
	དེ་མིན
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	གལ་ཏེ (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", NULL))
		dom->system_suspend_allowed = true;
	དེ་མིན
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	གལ་ཏེ (err)
		goto fail_free_all;

	ཕྱིར 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	ཕྱིར err;
}

ཧྲིལ fdt_domains_populate(const void *fdt)
{
	/* Sanity checks */
	གལ་ཏེ (!fdt)
		ཕྱིར SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	ཕྱིར fdt_iterate_each_domain_ro(fdt, NULL, __fdt_parse_domain);
}
