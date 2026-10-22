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

整数 fdt_iterate_each_domain(void *fdt, void *opaque,
			    整数 (*fn)(void *fdt, 整数 domain_offset,
				       void *opaque))
{
	整数 rc, doffset, poffset;

	如果 (!fdt || !fn)
		返回 SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	如果 (poffset < 0)
		返回 0;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	如果 (poffset < 0)
		返回 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		如果 (fdt_node_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			continue;

		rc = fn(fdt, doffset, opaque);
		如果 (rc)
			返回 rc;
	}

	返回 0;
}

static 整数 fdt_iterate_each_domain_ro(const void *fdt, void *opaque,
				      整数 (*fn)(const void *fdt, 整数 domain_offset,
						void *opaque))
{
	返回 fdt_iterate_each_domain((void *)fdt, opaque,
				       (整数 (*)(void *, 整数,  void *))fn);
}

整数 fdt_iterate_each_memregion(void *fdt, 整数 domain_offset, void *opaque,
			       整数 (*fn)(void *fdt, 整数 domain_offset,
					 整数 region_offset, u32 region_access,
					 void *opaque))
{
	u32 i, rcount;
	整数 rc, len, region_offset;
	const u32 *regions;

	如果 (!fdt || (domain_offset < 0) || !fn)
		返回 SBI_EINVAL;

	如果 (fdt_node_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		返回 SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	如果 (!regions)
		返回 0;

	rcount = (u32)len / (sizeof(u32) * 2);
	为 (i = 0; i < rcount; i++) {
		region_offset = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		如果 (region_offset < 0)
			返回 region_offset;

		如果 (fdt_node_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			返回 SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		如果 (rc)
			返回 rc;
	}

	返回 0;
}

static 整数 fdt_iterate_each_memregion_ro(const void *fdt, 整数 domain_offset, void *opaque,
					 整数 (*fn)(const void *fdt, 整数 domain_offset,
						   整数 region_offset, u32 region_access,
						   void *opaque))
{
	返回 fdt_iterate_each_memregion((void *)fdt, domain_offset, opaque,
					  (整数 (*)(void *, 整数, 整数, u32, void *))fn);
}

struct __fixup_find_domain_offset_info {
	const char *name;
	整数 *doffset;
};

static 整数 __fixup_find_domain_offset(void *fdt, 整数 doff, void *p)
{
	struct __fixup_find_domain_offset_info *fdo = p;

	如果 (!strncmp(fdo->name, fdt_get_name(fdt, doff, NULL), strlen(fdo->name)))
		*fdo->doffset = doff;

	返回 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

static 整数 __fixup_count_disable_devices(void *fdt, 整数 doff, 整数 roff,
					 u32 perm, void *p)
{
	整数 len;
	u32 *dcount = p;

	如果 (perm & DISABLE_DEVICES_MASK)
		返回 0;

	len = 0;
	如果 (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / sizeof(u32);

	返回 0;
}

static 整数 __fixup_disable_devices(void *fdt, 整数 doff, 整数 roff,
				   u32 raccess, void *p)
{
	整数 i, len, coff;
	const u32 *devices;

	如果 (raccess & DISABLE_DEVICES_MASK)
		返回 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	如果 (!devices)
		返回 0;
	len = len / sizeof(u32);

	为 (i = 0; i < len; i++) {
		coff = fdt_node_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		如果 (coff < 0)
			返回 coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	返回 0;
}

void fdt_domain_fixup(void *fdt)
{
	u32 dcount;
	整数 err, poffset, doffset;
	struct sbi_domain *dom = sbi_domain_thishart_ptr();
	struct __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	如果 (!dom->index)
		goto skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	如果 (doffset < 0)
		goto skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	如果 (!dcount)
		goto skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	如果 (err < 0)
		返回;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	如果 (doffset < 0)
		goto skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, NULL,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	如果 (poffset < 0)
		返回;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	如果 (poffset < 0)
		返回;
	fdt_nop_node(fdt, poffset);
}

#define FDT_DOMAIN_REGION_MAX_COUNT		16
#define FDT_ROOT_REGION_INHERIT_M_ONLY		0
#define FDT_ROOT_REGION_INHERIT_ALL		1

struct parse_region_data {
	struct sbi_domain *dom;
	u32 region_count;
	u32 max_regions;
};

static 整数 __fdt_parse_region(const void *fdt, 整数 domain_offset,
			      整数 region_offset, u32 region_access,
			      void *opaque)
{
	整数 len;
	u32 val32;
	u64 val64;
	const u32 *val;
	unsigned long base, order, flags;
	struct parse_region_data *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	如果 (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		返回 SBI_EINVAL;

	/* Find next region of the domain */
	如果 (preg->max_regions <= preg->region_count)
		返回 SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	如果 (!val || len != 8)
		返回 SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	如果 (!val || len != 4)
		返回 SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	如果 (val32 < 3 || __riscv_xlen < val32)
		返回 SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	如果 (fdt_get_property(fdt, region_offset, "mmio", NULL))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_count]);

	preg->region_count++;

	返回 0;
}

static 整数 __fdt_parse_domain(const void *fdt, 整数 domain_offset, void *opaque)
{
	u32 val32;
	u64 val64;
	const u32 *val;
	const char *inherit;
	struct sbi_domain *dom;
	struct sbi_hartmask *mask;
	struct parse_region_data preg;
	struct sbi_domain_memregion *reg;
	整数 inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
	整数 i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(sizeof(*dom));
	如果 (!dom)
		返回 SBI_ENOMEM;

	dom->regions = sbi_calloc(sizeof(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_COUNT + 1);
	如果 (!dom->regions) {
		err = SBI_ENOMEM;
		goto fail_free_domain;
	}
	preg.dom = dom;
	preg.region_count = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_COUNT;

	mask = sbi_zalloc(sizeof(*mask));
	如果 (!mask) {
		err = SBI_ENOMEM;
		goto fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_offset, NULL),
		    sizeof(dom->name));
	dom->name[sizeof(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / sizeof(u32);
	如果 (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	否则
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / sizeof(u32);
	如果 (val && len) {
		为 (i = 0; i < len; i++) {
			cpu_offset = fdt_node_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			如果 (cpu_offset < 0) {
				err = cpu_offset;
				goto fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			如果 (err)
				goto fail_free_all;

			如果 (!fdt_node_is_enabled(fdt, cpu_offset))
				continue;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	如果 (err)
		goto fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	如果 (inherit && len > 0) {
		如果 (!strcmp(inherit, "all"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_ALL;
		否则 如果 (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
		否则 {
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
		case FDT_ROOT_REGION_INHERIT_ALL:
			copy = true;
			break;
		case FDT_ROOT_REGION_INHERIT_M_ONLY:
			如果 (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			break;
		}

		如果 (!copy)
			continue;

		如果 (preg.max_regions <= preg.region_count) {
			err = SBI_EINVAL;
			goto fail_free_all;
		}
		memcpy(&dom->regions[preg.region_count++], reg, sizeof(*reg));
	}
	dom->fw_region_inited = root.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	如果 (val && len >= 4) {
		cpu_offset = fdt_node_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		如果 (cpu_offset >= 0 && fdt_node_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	如果 (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} 否则 {
		val64 = root.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	如果 (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} 否则 {
		val64 = root.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	如果 (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		如果 (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} 否则 {
		val32 = root.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	如果 (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", NULL))
		dom->system_reset_allowed = true;
	否则
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	如果 (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", NULL))
		dom->system_suspend_allowed = true;
	否则
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	如果 (err)
		goto fail_free_all;

	返回 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	返回 err;
}

整数 fdt_domains_populate(const void *fdt)
{
	/* Sanity checks */
	如果 (!fdt)
		返回 SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	返回 fdt_iterate_each_domain_ro(fdt, NULL, __fdt_parse_domain);
}
