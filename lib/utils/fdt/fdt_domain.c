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

цяло fdt_iterate_each_domain(void *fdt, void *opaque,
			    цяло (*fn)(void *fdt, цяло domain_offset,
				       void *opaque))
{
	цяло rc, doffset, poffset;

	ако (!fdt || !fn)
		върни SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	ако (poffset < 0)
		върни 0;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ако (poffset < 0)
		върни 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		ако (fdt_node_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			continue;

		rc = fn(fdt, doffset, opaque);
		ако (rc)
			върни rc;
	}

	върни 0;
}

static цяло fdt_iterate_each_domain_ro(const void *fdt, void *opaque,
				      цяло (*fn)(const void *fdt, цяло domain_offset,
						void *opaque))
{
	върни fdt_iterate_each_domain((void *)fdt, opaque,
				       (цяло (*)(void *, цяло,  void *))fn);
}

цяло fdt_iterate_each_memregion(void *fdt, цяло domain_offset, void *opaque,
			       цяло (*fn)(void *fdt, цяло domain_offset,
					 цяло region_offset, u32 region_access,
					 void *opaque))
{
	u32 i, rcount;
	цяло rc, len, region_offset;
	const u32 *regions;

	ако (!fdt || (domain_offset < 0) || !fn)
		върни SBI_EINVAL;

	ако (fdt_node_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		върни SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	ако (!regions)
		върни 0;

	rcount = (u32)len / (sizeof(u32) * 2);
	за (i = 0; i < rcount; i++) {
		region_offset = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		ако (region_offset < 0)
			върни region_offset;

		ако (fdt_node_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			върни SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		ако (rc)
			върни rc;
	}

	върни 0;
}

static цяло fdt_iterate_each_memregion_ro(const void *fdt, цяло domain_offset, void *opaque,
					 цяло (*fn)(const void *fdt, цяло domain_offset,
						   цяло region_offset, u32 region_access,
						   void *opaque))
{
	върни fdt_iterate_each_memregion((void *)fdt, domain_offset, opaque,
					  (цяло (*)(void *, цяло, цяло, u32, void *))fn);
}

struct __fixup_find_domain_offset_info {
	const char *name;
	цяло *doffset;
};

static цяло __fixup_find_domain_offset(void *fdt, цяло doff, void *p)
{
	struct __fixup_find_domain_offset_info *fdo = p;

	ако (!strncmp(fdo->name, fdt_get_name(fdt, doff, NULL), strlen(fdo->name)))
		*fdo->doffset = doff;

	върни 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

static цяло __fixup_count_disable_devices(void *fdt, цяло doff, цяло roff,
					 u32 perm, void *p)
{
	цяло len;
	u32 *dcount = p;

	ако (perm & DISABLE_DEVICES_MASK)
		върни 0;

	len = 0;
	ако (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / sizeof(u32);

	върни 0;
}

static цяло __fixup_disable_devices(void *fdt, цяло doff, цяло roff,
				   u32 raccess, void *p)
{
	цяло i, len, coff;
	const u32 *devices;

	ако (raccess & DISABLE_DEVICES_MASK)
		върни 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	ако (!devices)
		върни 0;
	len = len / sizeof(u32);

	за (i = 0; i < len; i++) {
		coff = fdt_node_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		ако (coff < 0)
			върни coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	върни 0;
}

void fdt_domain_fixup(void *fdt)
{
	u32 dcount;
	цяло err, poffset, doffset;
	struct sbi_domain *dom = sbi_domain_thishart_ptr();
	struct __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	ако (!dom->index)
		goto skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ако (doffset < 0)
		goto skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	ако (!dcount)
		goto skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	ако (err < 0)
		върни;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ако (doffset < 0)
		goto skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, NULL,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	ако (poffset < 0)
		върни;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ако (poffset < 0)
		върни;
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

static цяло __fdt_parse_region(const void *fdt, цяло domain_offset,
			      цяло region_offset, u32 region_access,
			      void *opaque)
{
	цяло len;
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
	ако (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		върни SBI_EINVAL;

	/* Find next region of the domain */
	ако (preg->max_regions <= preg->region_count)
		върни SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	ако (!val || len != 8)
		върни SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	ако (!val || len != 4)
		върни SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	ако (val32 < 3 || __riscv_xlen < val32)
		върни SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	ако (fdt_get_property(fdt, region_offset, "mmio", NULL))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_count]);

	preg->region_count++;

	върни 0;
}

static цяло __fdt_parse_domain(const void *fdt, цяло domain_offset, void *opaque)
{
	u32 val32;
	u64 val64;
	const u32 *val;
	const char *inherit;
	struct sbi_domain *dom;
	struct sbi_hartmask *mask;
	struct parse_region_data preg;
	struct sbi_domain_memregion *reg;
	цяло inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
	цяло i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(sizeof(*dom));
	ако (!dom)
		върни SBI_ENOMEM;

	dom->regions = sbi_calloc(sizeof(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_COUNT + 1);
	ако (!dom->regions) {
		err = SBI_ENOMEM;
		goto fail_free_domain;
	}
	preg.dom = dom;
	preg.region_count = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_COUNT;

	mask = sbi_zalloc(sizeof(*mask));
	ако (!mask) {
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
	ако (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	иначе
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / sizeof(u32);
	ако (val && len) {
		за (i = 0; i < len; i++) {
			cpu_offset = fdt_node_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			ако (cpu_offset < 0) {
				err = cpu_offset;
				goto fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			ако (err)
				goto fail_free_all;

			ако (!fdt_node_is_enabled(fdt, cpu_offset))
				continue;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	ако (err)
		goto fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	ако (inherit && len > 0) {
		ако (!strcmp(inherit, "all"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_ALL;
		иначе ако (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
		иначе {
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
			ако (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			break;
		}

		ако (!copy)
			continue;

		ако (preg.max_regions <= preg.region_count) {
			err = SBI_EINVAL;
			goto fail_free_all;
		}
		memcpy(&dom->regions[preg.region_count++], reg, sizeof(*reg));
	}
	dom->fw_region_inited = root.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	ако (val && len >= 4) {
		cpu_offset = fdt_node_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		ако (cpu_offset >= 0 && fdt_node_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	ако (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} иначе {
		val64 = root.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	ако (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} иначе {
		val64 = root.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	ако (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		ако (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} иначе {
		val32 = root.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	ако (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", NULL))
		dom->system_reset_allowed = true;
	иначе
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	ако (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", NULL))
		dom->system_suspend_allowed = true;
	иначе
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	ако (err)
		goto fail_free_all;

	върни 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	върни err;
}

цяло fdt_domains_populate(const void *fdt)
{
	/* Sanity checks */
	ако (!fdt)
		върни SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	върни fdt_iterate_each_domain_ro(fdt, NULL, __fdt_parse_domain);
}
