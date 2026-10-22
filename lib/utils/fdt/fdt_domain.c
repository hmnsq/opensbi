// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <libfdt.h>
#ضمن <libfdt_env.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hartmask.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi_utils/fdt/fdt_domain.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>

صحيح fdt_iterate_each_domain(فراغ *fdt, فراغ *opaque,
			    صحيح (*fn)(فراغ *fdt, صحيح domain_offset,
				       فراغ *opaque))
{
	صحيح rc, doffset, poffset;

	𐡄𐡍 (!fdt || !fn)
		أرجع SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	𐡄𐡍 (poffset < 0)
		أرجع 0;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	𐡄𐡍 (poffset < 0)
		أرجع 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		𐡄𐡍 (fdt_node_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			استمر;

		rc = fn(fdt, doffset, opaque);
		𐡄𐡍 (rc)
			أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح fdt_iterate_each_domain_ro(ثابت فراغ *fdt, فراغ *opaque,
				      صحيح (*fn)(ثابت فراغ *fdt, صحيح domain_offset,
						فراغ *opaque))
{
	أرجع fdt_iterate_each_domain((فراغ *)fdt, opaque,
				       (صحيح (*)(فراغ *, صحيح,  فراغ *))fn);
}

صحيح fdt_iterate_each_memregion(فراغ *fdt, صحيح domain_offset, فراغ *opaque,
			       صحيح (*fn)(فراغ *fdt, صحيح domain_offset,
					 صحيح region_offset, u32 region_access,
					 فراغ *opaque))
{
	u32 i, rcount;
	صحيح rc, len, region_offset;
	ثابت u32 *regions;

	𐡄𐡍 (!fdt || (domain_offset < 0) || !fn)
		أرجع SBI_EINVAL;

	𐡄𐡍 (fdt_node_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		أرجع SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	𐡄𐡍 (!regions)
		أرجع 0;

	rcount = (u32)len / (حجم(u32) * 2);
	لكل (i = 0; i < rcount; i++) {
		region_offset = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		𐡄𐡍 (region_offset < 0)
			أرجع region_offset;

		𐡄𐡍 (fdt_node_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			أرجع SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		𐡄𐡍 (rc)
			أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح fdt_iterate_each_memregion_ro(ثابت فراغ *fdt, صحيح domain_offset, فراغ *opaque,
					 صحيح (*fn)(ثابت فراغ *fdt, صحيح domain_offset,
						   صحيح region_offset, u32 region_access,
						   فراغ *opaque))
{
	أرجع fdt_iterate_each_memregion((فراغ *)fdt, domain_offset, opaque,
					  (صحيح (*)(فراغ *, صحيح, صحيح, u32, فراغ *))fn);
}

بنية __fixup_find_domain_offset_info {
	ثابت حرف *name;
	صحيح *doffset;
};

ساكن صحيح __fixup_find_domain_offset(فراغ *fdt, صحيح doff, فراغ *p)
{
	بنية __fixup_find_domain_offset_info *fdo = p;

	𐡄𐡍 (!strncmp(fdo->name, fdt_get_name(fdt, doff, عدم), strlen(fdo->name)))
		*fdo->doffset = doff;

	أرجع 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

ساكن صحيح __fixup_count_disable_devices(فراغ *fdt, صحيح doff, صحيح roff,
					 u32 perm, فراغ *p)
{
	صحيح len;
	u32 *dcount = p;

	𐡄𐡍 (perm & DISABLE_DEVICES_MASK)
		أرجع 0;

	len = 0;
	𐡄𐡍 (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / حجم(u32);

	أرجع 0;
}

ساكن صحيح __fixup_disable_devices(فراغ *fdt, صحيح doff, صحيح roff,
				   u32 raccess, فراغ *p)
{
	صحيح i, len, coff;
	ثابت u32 *devices;

	𐡄𐡍 (raccess & DISABLE_DEVICES_MASK)
		أرجع 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	𐡄𐡍 (!devices)
		أرجع 0;
	len = len / حجم(u32);

	لكل (i = 0; i < len; i++) {
		coff = fdt_node_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		𐡄𐡍 (coff < 0)
			أرجع coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	أرجع 0;
}

فراغ fdt_domain_fixup(فراغ *fdt)
{
	u32 dcount;
	صحيح err, poffset, doffset;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	بنية __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	𐡄𐡍 (!dom->index)
		اذهب skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	𐡄𐡍 (doffset < 0)
		اذهب skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	𐡄𐡍 (!dcount)
		اذهب skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	𐡄𐡍 (err < 0)
		أرجع;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	𐡄𐡍 (doffset < 0)
		اذهب skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, عدم,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	𐡄𐡍 (poffset < 0)
		أرجع;
	poffset = fdt_node_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	𐡄𐡍 (poffset < 0)
		أرجع;
	fdt_nop_node(fdt, poffset);
}

#define FDT_DOMAIN_REGION_MAX_COUNT		16
#define FDT_ROOT_REGION_INHERIT_M_ONLY		0
#define FDT_ROOT_REGION_INHERIT_ALL		1

بنية parse_region_data {
	بنية sbi_domain *dom;
	u32 region_count;
	u32 max_regions;
};

ساكن صحيح __fdt_parse_region(ثابت فراغ *fdt, صحيح domain_offset,
			      صحيح region_offset, u32 region_access,
			      فراغ *opaque)
{
	صحيح len;
	u32 val32;
	u64 val64;
	ثابت u32 *val;
	unsigned long base, order, flags;
	بنية parse_region_data *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	𐡄𐡍 (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		أرجع SBI_EINVAL;

	/* Find next region of the domain */
	𐡄𐡍 (preg->max_regions <= preg->region_count)
		أرجع SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	𐡄𐡍 (!val || len != 8)
		أرجع SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	𐡄𐡍 (!val || len != 4)
		أرجع SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	𐡄𐡍 (val32 < 3 || __riscv_xlen < val32)
		أرجع SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	𐡄𐡍 (fdt_get_property(fdt, region_offset, "mmio", عدم))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_count]);

	preg->region_count++;

	أرجع 0;
}

ساكن صحيح __fdt_parse_domain(ثابت فراغ *fdt, صحيح domain_offset, فراغ *opaque)
{
	u32 val32;
	u64 val64;
	ثابت u32 *val;
	ثابت حرف *inherit;
	بنية sbi_domain *dom;
	بنية sbi_hartmask *mask;
	بنية parse_region_data preg;
	بنية sbi_domain_memregion *reg;
	صحيح inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
	صحيح i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(حجم(*dom));
	𐡄𐡍 (!dom)
		أرجع SBI_ENOMEM;

	dom->regions = sbi_calloc(حجم(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_COUNT + 1);
	𐡄𐡍 (!dom->regions) {
		err = SBI_ENOMEM;
		اذهب fail_free_domain;
	}
	preg.dom = dom;
	preg.region_count = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_COUNT;

	mask = sbi_zalloc(حجم(*mask));
	𐡄𐡍 (!mask) {
		err = SBI_ENOMEM;
		اذهب fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_offset, عدم),
		    حجم(dom->name));
	dom->name[حجم(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / حجم(u32);
	𐡄𐡍 (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	وإلا
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / حجم(u32);
	𐡄𐡍 (val && len) {
		لكل (i = 0; i < len; i++) {
			cpu_offset = fdt_node_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			𐡄𐡍 (cpu_offset < 0) {
				err = cpu_offset;
				اذهب fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			𐡄𐡍 (err)
				اذهب fail_free_all;

			𐡄𐡍 (!fdt_node_is_enabled(fdt, cpu_offset))
				استمر;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	𐡄𐡍 (err)
		اذهب fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	𐡄𐡍 (inherit && len > 0) {
		𐡄𐡍 (!strcmp(inherit, "all"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_ALL;
		وإلا 𐡄𐡍 (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_ROOT_REGION_INHERIT_M_ONLY;
		وإلا {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			اذهب fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&root, reg) {
		bool copy = false;

		بدل (inheritance_mode) {
		حالة FDT_ROOT_REGION_INHERIT_ALL:
			copy = true;
			اكسر;
		حالة FDT_ROOT_REGION_INHERIT_M_ONLY:
			𐡄𐡍 (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			اكسر;
		}

		𐡄𐡍 (!copy)
			استمر;

		𐡄𐡍 (preg.max_regions <= preg.region_count) {
			err = SBI_EINVAL;
			اذهب fail_free_all;
		}
		memcpy(&dom->regions[preg.region_count++], reg, حجم(*reg));
	}
	dom->fw_region_inited = root.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	𐡄𐡍 (val && len >= 4) {
		cpu_offset = fdt_node_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		𐡄𐡍 (cpu_offset >= 0 && fdt_node_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	𐡄𐡍 (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} وإلا {
		val64 = root.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	𐡄𐡍 (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} وإلا {
		val64 = root.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	𐡄𐡍 (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		𐡄𐡍 (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} وإلا {
		val32 = root.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	𐡄𐡍 (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", عدم))
		dom->system_reset_allowed = true;
	وإلا
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	𐡄𐡍 (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", عدم))
		dom->system_suspend_allowed = true;
	وإلا
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	𐡄𐡍 (err)
		اذهب fail_free_all;

	أرجع 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	أرجع err;
}

صحيح fdt_domains_populate(ثابت فراغ *fdt)
{
	/* Sanity checks */
	𐡄𐡍 (!fdt)
		أرجع SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	أرجع fdt_iterate_each_domain_ro(fdt, عدم, __fdt_parse_domain);
}
