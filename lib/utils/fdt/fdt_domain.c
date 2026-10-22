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

ຈຳນວນເຕັມ fdt_iterate_each_domain(void *fdt, void *opaque,
			    ຈຳນວນເຕັມ (*fn)(void *fdt, ຈຳນວນເຕັມ domain_offset,
				       void *opaque))
{
	ຈຳນວນເຕັມ rc, doffset, poffset;

	ຖ້າ (!fdt || !fn)
		ສົ່ງຄືນ SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	ຖ້າ (poffset < 0)
		ສົ່ງຄືນ 0;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ຖ້າ (poffset < 0)
		ສົ່ງຄືນ 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		ຖ້າ (fdt_node_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			continue;

		rc = fn(fdt, doffset, opaque);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;
	}

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ fdt_iterate_each_domain_ro(const void *fdt, void *opaque,
				      ຈຳນວນເຕັມ (*fn)(const void *fdt, ຈຳນວນເຕັມ domain_offset,
						void *opaque))
{
	ສົ່ງຄືນ fdt_iterate_each_domain((void *)fdt, opaque,
				       (ຈຳນວນເຕັມ (*)(void *, ຈຳນວນເຕັມ,  void *))fn);
}

ຈຳນວນເຕັມ fdt_iterate_each_memregion(void *fdt, ຈຳນວນເຕັມ domain_offset, void *opaque,
			       ຈຳນວນເຕັມ (*fn)(void *fdt, ຈຳນວນເຕັມ domain_offset,
					 ຈຳນວນເຕັມ region_offset, u32 region_access,
					 void *opaque))
{
	u32 i, rcount;
	ຈຳນວນເຕັມ rc, len, region_offset;
	const u32 *regions;

	ຖ້າ (!fdt || (domain_offset < 0) || !fn)
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (fdt_node_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		ສົ່ງຄືນ SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	ຖ້າ (!regions)
		ສົ່ງຄືນ 0;

	rcount = (u32)len / (parimaa_na(u32) * 2);
	ສຳລັບ (i = 0; i < rcount; i++) {
		region_offset = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		ຖ້າ (region_offset < 0)
			ສົ່ງຄືນ region_offset;

		ຖ້າ (fdt_node_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			ສົ່ງຄືນ SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;
	}

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ fdt_iterate_each_memregion_ro(const void *fdt, ຈຳນວນເຕັມ domain_offset, void *opaque,
					 ຈຳນວນເຕັມ (*fn)(const void *fdt, ຈຳນວນເຕັມ domain_offset,
						   ຈຳນວນເຕັມ region_offset, u32 region_access,
						   void *opaque))
{
	ສົ່ງຄືນ fdt_iterate_each_memregion((void *)fdt, domain_offset, opaque,
					  (ຈຳນວນເຕັມ (*)(void *, ຈຳນວນເຕັມ, ຈຳນວນເຕັມ, u32, void *))fn);
}

sa_nracanaa __fixup_find_domain_offset_info {
	const var_na *name;
	ຈຳນວນເຕັມ *doffset;
};

static ຈຳນວນເຕັມ __fixup_find_domain_offset(void *fdt, ຈຳນວນເຕັມ doff, void *p)
{
	sa_nracanaa __fixup_find_domain_offset_info *fdo = p;

	ຖ້າ (!strncmp(fdo->name, fdt_get_name(fdt, doff, NULL), strlen(fdo->name)))
		*fdo->doffset = doff;

	ສົ່ງຄືນ 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

static ຈຳນວນເຕັມ __fixup_count_disable_devices(void *fdt, ຈຳນວນເຕັມ doff, ຈຳນວນເຕັມ roff,
					 u32 perm, void *p)
{
	ຈຳນວນເຕັມ len;
	u32 *dcount = p;

	ຖ້າ (perm & DISABLE_DEVICES_MASK)
		ສົ່ງຄືນ 0;

	len = 0;
	ຖ້າ (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / parimaa_na(u32);

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ __fixup_disable_devices(void *fdt, ຈຳນວນເຕັມ doff, ຈຳນວນເຕັມ roff,
				   u32 raccess, void *p)
{
	ຈຳນວນເຕັມ i, len, coff;
	const u32 *devices;

	ຖ້າ (raccess & DISABLE_DEVICES_MASK)
		ສົ່ງຄືນ 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	ຖ້າ (!devices)
		ສົ່ງຄືນ 0;
	len = len / parimaa_na(u32);

	ສຳລັບ (i = 0; i < len; i++) {
		coff = fdt_node_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		ຖ້າ (coff < 0)
			ສົ່ງຄືນ coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	ສົ່ງຄືນ 0;
}

void fdt_domain_fixup(void *fdt)
{
	u32 dcount;
	ຈຳນວນເຕັມ err, poffset, doffset;
	sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();
	sa_nracanaa __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	ຖ້າ (!dom->index)
		goto skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ຖ້າ (doffset < 0)
		goto skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	ຖ້າ (!dcount)
		goto skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	ຖ້າ (err < 0)
		ສົ່ງຄືນ;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ຖ້າ (doffset < 0)
		goto skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, NULL,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	ຖ້າ (poffset < 0)
		ສົ່ງຄືນ;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ຖ້າ (poffset < 0)
		ສົ່ງຄືນ;
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

static ຈຳນວນເຕັມ __fdt_parse_region(const void *fdt, ຈຳນວນເຕັມ domain_offset,
			      ຈຳນວນເຕັມ region_offset, u32 region_access,
			      void *opaque)
{
	ຈຳນວນເຕັມ len;
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
	ຖ້າ (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		ສົ່ງຄືນ SBI_EINVAL;

	/* Find next region of the domain */
	ຖ້າ (preg->max_regions <= preg->region_count)
		ສົ່ງຄືນ SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	ຖ້າ (!val || len != 8)
		ສົ່ງຄືນ SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	ຖ້າ (!val || len != 4)
		ສົ່ງຄືນ SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	ຖ້າ (val32 < 3 || __riscv_xlen < val32)
		ສົ່ງຄືນ SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	ຖ້າ (fdt_get_property(fdt, region_offset, "mmio", NULL))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_count]);

	preg->region_count++;

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ __fdt_parse_domain(const void *fdt, ຈຳນວນເຕັມ domain_offset, void *opaque)
{
	u32 val32;
	u64 val64;
	const u32 *val;
	const var_na *inherit;
	sa_nracanaa sbi_domain *dom;
	sa_nracanaa sbi_hartmask *mask;
	sa_nracanaa parse_region_data preg;
	sa_nracanaa sbi_domain_memregion *reg;
	ຈຳນວນເຕັມ inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
	ຈຳນວນເຕັມ i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(parimaa_na(*dom));
	ຖ້າ (!dom)
		ສົ່ງຄືນ SBI_ENOMEM;

	dom->regions = sbi_calloc(parimaa_na(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_COUNT + 1);
	ຖ້າ (!dom->regions) {
		err = SBI_ENOMEM;
		goto fail_free_domain;
	}
	preg.dom = dom;
	preg.region_count = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_COUNT;

	mask = sbi_zalloc(parimaa_na(*mask));
	ຖ້າ (!mask) {
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
	ຖ້າ (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	ບໍ່ດັ່ງນັ້ນ
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / parimaa_na(u32);
	ຖ້າ (val && len) {
		ສຳລັບ (i = 0; i < len; i++) {
			cpu_offset = fdt_node_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			ຖ້າ (cpu_offset < 0) {
				err = cpu_offset;
				goto fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			ຖ້າ (err)
				goto fail_free_all;

			ຖ້າ (!fdt_node_is_enabled(fdt, cpu_offset))
				continue;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	ຖ້າ (err)
		goto fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	ຖ້າ (inherit && len > 0) {
		ຖ້າ (!strcmp(inherit, "all"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_ALL;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
		ບໍ່ດັ່ງນັ້ນ {
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
			ຖ້າ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			bha_nga;
		}

		ຖ້າ (!copy)
			continue;

		ຖ້າ (preg.max_regions <= preg.region_count) {
			err = SBI_EINVAL;
			goto fail_free_all;
		}
		memcpy(&dom->regions[preg.region_count++], reg, parimaa_na(*reg));
	}
	dom->fw_region_inited = root.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	ຖ້າ (val && len >= 4) {
		cpu_offset = fdt_node_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		ຖ້າ (cpu_offset >= 0 && fdt_node_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	ຖ້າ (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} ບໍ່ດັ່ງນັ້ນ {
		val64 = root.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	ຖ້າ (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} ບໍ່ດັ່ງນັ້ນ {
		val64 = root.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	ຖ້າ (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		ຖ້າ (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} ບໍ່ດັ່ງນັ້ນ {
		val32 = root.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	ຖ້າ (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", NULL))
		dom->system_reset_allowed = true;
	ບໍ່ດັ່ງນັ້ນ
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	ຖ້າ (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", NULL))
		dom->system_suspend_allowed = true;
	ບໍ່ດັ່ງນັ້ນ
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	ຖ້າ (err)
		goto fail_free_all;

	ສົ່ງຄືນ 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	ສົ່ງຄືນ err;
}

ຈຳນວນເຕັມ fdt_domains_populate(const void *fdt)
{
	/* Sanity checks */
	ຖ້າ (!fdt)
		ສົ່ງຄືນ SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	ສົ່ງຄືນ fdt_iterate_each_domain_ro(fdt, NULL, __fdt_parse_domain);
}
