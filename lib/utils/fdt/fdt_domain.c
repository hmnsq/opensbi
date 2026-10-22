// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <libfdt_env.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hartmask.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_domain.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>

পূর্ণসংখ্যা fdt_iterate_each_domain(শূন্য *fdt, শূন্য *opaque,
			    পূর্ণসংখ্যা (*fn)(শূন্য *fdt, পূর্ণসংখ্যা domain_offset,
				       শূন্য *opaque))
{
	পূর্ণসংখ্যা rc, doffset, poffset;

	যদি (!fdt || !fn)
		ফেরত SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	যদি (poffset < 0)
		ফেরত 0;
	poffset = fdt_গ্রন্থি_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	যদি (poffset < 0)
		ফেরত 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		যদি (fdt_গ্রন্থি_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			চালু;

		rc = fn(fdt, doffset, opaque);
		যদি (rc)
			ফেরত rc;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা fdt_iterate_each_domain_ro(ধ্রুবক শূন্য *fdt, শূন্য *opaque,
				      পূর্ণসংখ্যা (*fn)(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা domain_offset,
						শূন্য *opaque))
{
	ফেরত fdt_iterate_each_domain((শূন্য *)fdt, opaque,
				       (পূর্ণসংখ্যা (*)(শূন্য *, পূর্ণসংখ্যা,  শূন্য *))fn);
}

পূর্ণসংখ্যা fdt_iterate_each_memregion(শূন্য *fdt, পূর্ণসংখ্যা domain_offset, শূন্য *opaque,
			       পূর্ণসংখ্যা (*fn)(শূন্য *fdt, পূর্ণসংখ্যা domain_offset,
					 পূর্ণসংখ্যা region_offset, u32 region_access,
					 শূন্য *opaque))
{
	u32 i, rcount;
	পূর্ণসংখ্যা rc, len, region_offset;
	ধ্রুবক u32 *regions;

	যদি (!fdt || (domain_offset < 0) || !fn)
		ফেরত SBI_EINVAL;

	যদি (fdt_গ্রন্থি_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		ফেরত SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	যদি (!regions)
		ফেরত 0;

	rcount = (u32)len / (আকার(u32) * 2);
	জন্য (i = 0; i < rcount; i++) {
		region_offset = fdt_গ্রন্থি_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		যদি (region_offset < 0)
			ফেরত region_offset;

		যদি (fdt_গ্রন্থি_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			ফেরত SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		যদি (rc)
			ফেরত rc;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা fdt_iterate_each_memregion_ro(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা domain_offset, শূন্য *opaque,
					 পূর্ণসংখ্যা (*fn)(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা domain_offset,
						   পূর্ণসংখ্যা region_offset, u32 region_access,
						   শূন্য *opaque))
{
	ফেরত fdt_iterate_each_memregion((শূন্য *)fdt, domain_offset, opaque,
					  (পূর্ণসংখ্যা (*)(শূন্য *, পূর্ণসংখ্যা, পূর্ণসংখ্যা, u32, শূন্য *))fn);
}

কাঠামো __fixup_find_domain_offset_info {
	ধ্রুবক অক্ষর *name;
	পূর্ণসংখ্যা *doffset;
};

স্থির পূর্ণসংখ্যা __fixup_find_domain_offset(শূন্য *fdt, পূর্ণসংখ্যা doff, শূন্য *p)
{
	কাঠামো __fixup_find_domain_offset_info *fdo = p;

	যদি (!strncmp(fdo->name, fdt_get_name(fdt, doff, ফাঁকা), strlen(fdo->name)))
		*fdo->doffset = doff;

	ফেরত 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

স্থির পূর্ণসংখ্যা __fixup_count_disable_devices(শূন্য *fdt, পূর্ণসংখ্যা doff, পূর্ণসংখ্যা roff,
					 u32 perm, শূন্য *p)
{
	পূর্ণসংখ্যা len;
	u32 *dcount = p;

	যদি (perm & DISABLE_DEVICES_MASK)
		ফেরত 0;

	len = 0;
	যদি (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / আকার(u32);

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা __fixup_disable_devices(শূন্য *fdt, পূর্ণসংখ্যা doff, পূর্ণসংখ্যা roff,
				   u32 raccess, শূন্য *p)
{
	পূর্ণসংখ্যা i, len, coff;
	ধ্রুবক u32 *devices;

	যদি (raccess & DISABLE_DEVICES_MASK)
		ফেরত 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	যদি (!devices)
		ফেরত 0;
	len = len / আকার(u32);

	জন্য (i = 0; i < len; i++) {
		coff = fdt_গ্রন্থি_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		যদি (coff < 0)
			ফেরত coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	ফেরত 0;
}

শূন্য fdt_domain_fixup(শূন্য *fdt)
{
	u32 dcount;
	পূর্ণসংখ্যা err, poffset, doffset;
	কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();
	কাঠামো __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	যদি (!dom->সূচক_2)
		যাও skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	যদি (doffset < 0)
		যাও skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	যদি (!dcount)
		যাও skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	যদি (err < 0)
		ফেরত;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	যদি (doffset < 0)
		যাও skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, ফাঁকা,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	যদি (poffset < 0)
		ফেরত;
	poffset = fdt_গ্রন্থি_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	যদি (poffset < 0)
		ফেরত;
	fdt_nop_গ্রন্থি(fdt, poffset);
}

#define FDT_DOMAIN_REGION_MAX_গুনতি		16
#define FDT_মূল_REGION_INHERIT_M_ONLY		0
#define FDT_মূল_REGION_INHERIT_ALL		1

কাঠামো parse_region_তথ্য {
	কাঠামো sbi_domain *dom;
	u32 region_গুনতি;
	u32 max_regions;
};

স্থির পূর্ণসংখ্যা __fdt_parse_region(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা domain_offset,
			      পূর্ণসংখ্যা region_offset, u32 region_access,
			      শূন্য *opaque)
{
	পূর্ণসংখ্যা len;
	u32 val32;
	u64 val64;
	ধ্রুবক u32 *val;
	unsigned long base, order, flags;
	কাঠামো parse_region_তথ্য *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	যদি (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		ফেরত SBI_EINVAL;

	/* Find next region of the domain */
	যদি (preg->max_regions <= preg->region_গুনতি)
		ফেরত SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	যদি (!val || len != 8)
		ফেরত SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	যদি (!val || len != 4)
		ফেরত SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	যদি (val32 < 3 || __riscv_xlen < val32)
		ফেরত SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	যদি (fdt_get_property(fdt, region_offset, "mmio", ফাঁকা))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_গুনতি]);

	preg->region_গুনতি++;

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা __fdt_parse_domain(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা domain_offset, শূন্য *opaque)
{
	u32 val32;
	u64 val64;
	ধ্রুবক u32 *val;
	ধ্রুবক অক্ষর *inherit;
	কাঠামো sbi_domain *dom;
	কাঠামো sbi_hartmask *mask;
	কাঠামো parse_region_তথ্য preg;
	কাঠামো sbi_domain_memregion *reg;
	পূর্ণসংখ্যা inheritance_mode = FDT_মূল_REGION_INHERIT_M_ONLY;
	পূর্ণসংখ্যা i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(আকার(*dom));
	যদি (!dom)
		ফেরত SBI_ENOMEM;

	dom->regions = sbi_calloc(আকার(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_গুনতি + 1);
	যদি (!dom->regions) {
		err = SBI_ENOMEM;
		যাও fail_free_domain;
	}
	preg.dom = dom;
	preg.region_গুনতি = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_গুনতি;

	mask = sbi_zalloc(আকার(*mask));
	যদি (!mask) {
		err = SBI_ENOMEM;
		যাও fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_offset, ফাঁকা),
		    আকার(dom->name));
	dom->name[আকার(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / আকার(u32);
	যদি (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	অন্যথা
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / আকার(u32);
	যদি (val && len) {
		জন্য (i = 0; i < len; i++) {
			cpu_offset = fdt_গ্রন্থি_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			যদি (cpu_offset < 0) {
				err = cpu_offset;
				যাও fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			যদি (err)
				যাও fail_free_all;

			যদি (!fdt_গ্রন্থি_is_enabled(fdt, cpu_offset))
				চালু;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	যদি (err)
		যাও fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	যদি (inherit && len > 0) {
		যদি (!strcmp(inherit, "all"))
			inheritance_mode = FDT_মূল_REGION_INHERIT_ALL;
		অন্যথা যদি (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_মূল_REGION_INHERIT_M_ONLY;
		অন্যথা {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			যাও fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&মূল, reg) {
		bool copy = false;

		নির্বাচন (inheritance_mode) {
		ক্ষেত্র FDT_মূল_REGION_INHERIT_ALL:
			copy = true;
			বিরতি;
		ক্ষেত্র FDT_মূল_REGION_INHERIT_M_ONLY:
			যদি (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			বিরতি;
		}

		যদি (!copy)
			চালু;

		যদি (preg.max_regions <= preg.region_গুনতি) {
			err = SBI_EINVAL;
			যাও fail_free_all;
		}
		memcpy(&dom->regions[preg.region_গুনতি++], reg, আকার(*reg));
	}
	dom->fw_region_inited = মূল.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	যদি (val && len >= 4) {
		cpu_offset = fdt_গ্রন্থি_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		যদি (cpu_offset >= 0 && fdt_গ্রন্থি_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	যদি (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} অন্যথা {
		val64 = মূল.পরবর্তী_arg1;
	}
	dom->পরবর্তী_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	যদি (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} অন্যথা {
		val64 = মূল.পরবর্তী_addr;
	}
	dom->পরবর্তী_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	যদি (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		যদি (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} অন্যথা {
		val32 = মূল.পরবর্তী_mode;
	}
	dom->পরবর্তী_mode = val32;

	/* Read "system-reset-allowed" DT property */
	যদি (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", ফাঁকা))
		dom->system_reset_allowed = true;
	অন্যথা
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	যদি (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", ফাঁকা))
		dom->system_suspend_allowed = true;
	অন্যথা
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	যদি (err)
		যাও fail_free_all;

	ফেরত 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	ফেরত err;
}

পূর্ণসংখ্যা fdt_domains_populate(ধ্রুবক শূন্য *fdt)
{
	/* Sanity checks */
	যদি (!fdt)
		ফেরত SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	ফেরত fdt_iterate_each_domain_ro(fdt, ফাঁকা, __fdt_parse_domain);
}
