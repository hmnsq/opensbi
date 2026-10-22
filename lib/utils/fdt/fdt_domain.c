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

số_nguyên fdt_iterate_each_domain(void *fdt, void *opaque,
			    số_nguyên (*fn)(void *fdt, số_nguyên domain_offset,
				       void *opaque))
{
	số_nguyên rc, doffset, poffset;

	nếu (!fdt || !fn)
		trả_về SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	nếu (poffset < 0)
		trả_về 0;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	nếu (poffset < 0)
		trả_về 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		nếu (fdt_node_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			continue;

		rc = fn(fdt, doffset, opaque);
		nếu (rc)
			trả_về rc;
	}

	trả_về 0;
}

static số_nguyên fdt_iterate_each_domain_ro(const void *fdt, void *opaque,
				      số_nguyên (*fn)(const void *fdt, số_nguyên domain_offset,
						void *opaque))
{
	trả_về fdt_iterate_each_domain((void *)fdt, opaque,
				       (số_nguyên (*)(void *, số_nguyên,  void *))fn);
}

số_nguyên fdt_iterate_each_memregion(void *fdt, số_nguyên domain_offset, void *opaque,
			       số_nguyên (*fn)(void *fdt, số_nguyên domain_offset,
					 số_nguyên region_offset, u32 region_access,
					 void *opaque))
{
	u32 i, rcount;
	số_nguyên rc, len, region_offset;
	const u32 *regions;

	nếu (!fdt || (domain_offset < 0) || !fn)
		trả_về SBI_EINVAL;

	nếu (fdt_node_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		trả_về SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	nếu (!regions)
		trả_về 0;

	rcount = (u32)len / (sizeof(u32) * 2);
	cho (i = 0; i < rcount; i++) {
		region_offset = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		nếu (region_offset < 0)
			trả_về region_offset;

		nếu (fdt_node_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			trả_về SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		nếu (rc)
			trả_về rc;
	}

	trả_về 0;
}

static số_nguyên fdt_iterate_each_memregion_ro(const void *fdt, số_nguyên domain_offset, void *opaque,
					 số_nguyên (*fn)(const void *fdt, số_nguyên domain_offset,
						   số_nguyên region_offset, u32 region_access,
						   void *opaque))
{
	trả_về fdt_iterate_each_memregion((void *)fdt, domain_offset, opaque,
					  (số_nguyên (*)(void *, số_nguyên, số_nguyên, u32, void *))fn);
}

struct __fixup_find_domain_offset_info {
	const char *name;
	số_nguyên *doffset;
};

static số_nguyên __fixup_find_domain_offset(void *fdt, số_nguyên doff, void *p)
{
	struct __fixup_find_domain_offset_info *fdo = p;

	nếu (!strncmp(fdo->name, fdt_get_name(fdt, doff, NULL), strlen(fdo->name)))
		*fdo->doffset = doff;

	trả_về 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

static số_nguyên __fixup_count_disable_devices(void *fdt, số_nguyên doff, số_nguyên roff,
					 u32 perm, void *p)
{
	số_nguyên len;
	u32 *dcount = p;

	nếu (perm & DISABLE_DEVICES_MASK)
		trả_về 0;

	len = 0;
	nếu (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / sizeof(u32);

	trả_về 0;
}

static số_nguyên __fixup_disable_devices(void *fdt, số_nguyên doff, số_nguyên roff,
				   u32 raccess, void *p)
{
	số_nguyên i, len, coff;
	const u32 *devices;

	nếu (raccess & DISABLE_DEVICES_MASK)
		trả_về 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	nếu (!devices)
		trả_về 0;
	len = len / sizeof(u32);

	cho (i = 0; i < len; i++) {
		coff = fdt_node_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		nếu (coff < 0)
			trả_về coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	trả_về 0;
}

void fdt_domain_fixup(void *fdt)
{
	u32 dcount;
	số_nguyên err, poffset, doffset;
	struct sbi_domain *dom = sbi_domain_thishart_ptr();
	struct __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	nếu (!dom->index)
		goto skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	nếu (doffset < 0)
		goto skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	nếu (!dcount)
		goto skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	nếu (err < 0)
		trả_về;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	nếu (doffset < 0)
		goto skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, NULL,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	nếu (poffset < 0)
		trả_về;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	nếu (poffset < 0)
		trả_về;
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

static số_nguyên __fdt_parse_region(const void *fdt, số_nguyên domain_offset,
			      số_nguyên region_offset, u32 region_access,
			      void *opaque)
{
	số_nguyên len;
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
	nếu (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		trả_về SBI_EINVAL;

	/* Find next region of the domain */
	nếu (preg->max_regions <= preg->region_count)
		trả_về SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	nếu (!val || len != 8)
		trả_về SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	nếu (!val || len != 4)
		trả_về SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	nếu (val32 < 3 || __riscv_xlen < val32)
		trả_về SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	nếu (fdt_get_property(fdt, region_offset, "mmio", NULL))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_count]);

	preg->region_count++;

	trả_về 0;
}

static số_nguyên __fdt_parse_domain(const void *fdt, số_nguyên domain_offset, void *opaque)
{
	u32 val32;
	u64 val64;
	const u32 *val;
	const char *inherit;
	struct sbi_domain *dom;
	struct sbi_hartmask *mask;
	struct parse_region_data preg;
	struct sbi_domain_memregion *reg;
	số_nguyên inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
	số_nguyên i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(sizeof(*dom));
	nếu (!dom)
		trả_về SBI_ENOMEM;

	dom->regions = sbi_calloc(sizeof(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_COUNT + 1);
	nếu (!dom->regions) {
		err = SBI_ENOMEM;
		goto fail_free_domain;
	}
	preg.dom = dom;
	preg.region_count = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_COUNT;

	mask = sbi_zalloc(sizeof(*mask));
	nếu (!mask) {
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
	nếu (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	không_thì
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / sizeof(u32);
	nếu (val && len) {
		cho (i = 0; i < len; i++) {
			cpu_offset = fdt_node_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			nếu (cpu_offset < 0) {
				err = cpu_offset;
				goto fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			nếu (err)
				goto fail_free_all;

			nếu (!fdt_node_is_enabled(fdt, cpu_offset))
				continue;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	nếu (err)
		goto fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	nếu (inherit && len > 0) {
		nếu (!strcmp(inherit, "all"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_ALL;
		không_thì nếu (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
		không_thì {
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
			nếu (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			break;
		}

		nếu (!copy)
			continue;

		nếu (preg.max_regions <= preg.region_count) {
			err = SBI_EINVAL;
			goto fail_free_all;
		}
		memcpy(&dom->regions[preg.region_count++], reg, sizeof(*reg));
	}
	dom->fw_region_inited = root.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	nếu (val && len >= 4) {
		cpu_offset = fdt_node_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		nếu (cpu_offset >= 0 && fdt_node_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	nếu (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} không_thì {
		val64 = root.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	nếu (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} không_thì {
		val64 = root.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	nếu (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		nếu (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} không_thì {
		val32 = root.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	nếu (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", NULL))
		dom->system_reset_allowed = true;
	không_thì
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	nếu (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", NULL))
		dom->system_suspend_allowed = true;
	không_thì
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	nếu (err)
		goto fail_free_all;

	trả_về 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	trả_về err;
}

số_nguyên fdt_domains_populate(const void *fdt)
{
	/* Sanity checks */
	nếu (!fdt)
		trả_về SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	trả_về fdt_iterate_each_domain_ro(fdt, NULL, __fdt_parse_domain);
}
