// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <libfdt.h>
#شامل <libfdt_env.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hartmask.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi_utils/fdt/fdt_domain.h>
#شامل <sbi_utils/fdt/fdt_helper.h>

صحیح fdt_iterate_each_domain(تهی *fdt, تهی *opaque,
			    صحیح (*fn)(تهی *fdt, صحیح domain_offset,
				       تهی *opaque))
{
	صحیح rc, doffset, poffset;

	اگر (!fdt || !fn)
		بازگشت SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	اگر (poffset < 0)
		بازگشت 0;
	poffset = fdt_گره_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	اگر (poffset < 0)
		بازگشت 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		اگر (fdt_گره_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			ادامه;

		rc = fn(fdt, doffset, opaque);
		اگر (rc)
			بازگشت rc;
	}

	بازگشت 0;
}

ایستا صحیح fdt_iterate_each_domain_ro(ثابت تهی *fdt, تهی *opaque,
				      صحیح (*fn)(ثابت تهی *fdt, صحیح domain_offset,
						تهی *opaque))
{
	بازگشت fdt_iterate_each_domain((تهی *)fdt, opaque,
				       (صحیح (*)(تهی *, صحیح,  تهی *))fn);
}

صحیح fdt_iterate_each_memregion(تهی *fdt, صحیح domain_offset, تهی *opaque,
			       صحیح (*fn)(تهی *fdt, صحیح domain_offset,
					 صحیح region_offset, u32 region_access,
					 تهی *opaque))
{
	u32 i, rcount;
	صحیح rc, len, region_offset;
	ثابت u32 *regions;

	اگر (!fdt || (domain_offset < 0) || !fn)
		بازگشت SBI_EINVAL;

	اگر (fdt_گره_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		بازگشت SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	اگر (!regions)
		بازگشت 0;

	rcount = (u32)len / (اندازه(u32) * 2);
	برای (i = 0; i < rcount; i++) {
		region_offset = fdt_گره_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		اگر (region_offset < 0)
			بازگشت region_offset;

		اگر (fdt_گره_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			بازگشت SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		اگر (rc)
			بازگشت rc;
	}

	بازگشت 0;
}

ایستا صحیح fdt_iterate_each_memregion_ro(ثابت تهی *fdt, صحیح domain_offset, تهی *opaque,
					 صحیح (*fn)(ثابت تهی *fdt, صحیح domain_offset,
						   صحیح region_offset, u32 region_access,
						   تهی *opaque))
{
	بازگشت fdt_iterate_each_memregion((تهی *)fdt, domain_offset, opaque,
					  (صحیح (*)(تهی *, صحیح, صحیح, u32, تهی *))fn);
}

ساختار __fixup_find_domain_offset_info {
	ثابت نویسه *name;
	صحیح *doffset;
};

ایستا صحیح __fixup_find_domain_offset(تهی *fdt, صحیح doff, تهی *p)
{
	ساختار __fixup_find_domain_offset_info *fdo = p;

	اگر (!strncmp(fdo->name, fdt_get_name(fdt, doff, پوچ), strlen(fdo->name)))
		*fdo->doffset = doff;

	بازگشت 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

ایستا صحیح __fixup_count_disable_devices(تهی *fdt, صحیح doff, صحیح roff,
					 u32 perm, تهی *p)
{
	صحیح len;
	u32 *dcount = p;

	اگر (perm & DISABLE_DEVICES_MASK)
		بازگشت 0;

	len = 0;
	اگر (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / اندازه(u32);

	بازگشت 0;
}

ایستا صحیح __fixup_disable_devices(تهی *fdt, صحیح doff, صحیح roff,
				   u32 raccess, تهی *p)
{
	صحیح i, len, coff;
	ثابت u32 *devices;

	اگر (raccess & DISABLE_DEVICES_MASK)
		بازگشت 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	اگر (!devices)
		بازگشت 0;
	len = len / اندازه(u32);

	برای (i = 0; i < len; i++) {
		coff = fdt_گره_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		اگر (coff < 0)
			بازگشت coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	بازگشت 0;
}

تهی fdt_domain_fixup(تهی *fdt)
{
	u32 dcount;
	صحیح err, poffset, doffset;
	ساختار sbi_domain *dom = sbi_domain_thishart_ptr();
	ساختار __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	اگر (!dom->index)
		برو skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	اگر (doffset < 0)
		برو skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	اگر (!dcount)
		برو skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	اگر (err < 0)
		بازگشت;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	اگر (doffset < 0)
		برو skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, پوچ,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	اگر (poffset < 0)
		بازگشت;
	poffset = fdt_گره_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	اگر (poffset < 0)
		بازگشت;
	fdt_nop_گره(fdt, poffset);
}

#define FDT_DOMAIN_REGION_MAX_شمار		16
#define FDT_ریشه_REGION_INHERIT_M_ONLY		0
#define FDT_ریشه_REGION_INHERIT_ALL		1

ساختار parse_region_data {
	ساختار sbi_domain *dom;
	u32 region_شمار;
	u32 max_regions;
};

ایستا صحیح __fdt_parse_region(ثابت تهی *fdt, صحیح domain_offset,
			      صحیح region_offset, u32 region_access,
			      تهی *opaque)
{
	صحیح len;
	u32 val32;
	u64 val64;
	ثابت u32 *val;
	unsigned long base, order, flags;
	ساختار parse_region_data *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	اگر (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		بازگشت SBI_EINVAL;

	/* Find next region of the domain */
	اگر (preg->max_regions <= preg->region_شمار)
		بازگشت SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	اگر (!val || len != 8)
		بازگشت SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	اگر (!val || len != 4)
		بازگشت SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	اگر (val32 < 3 || __riscv_xlen < val32)
		بازگشت SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	اگر (fdt_get_property(fdt, region_offset, "mmio", پوچ))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_شمار]);

	preg->region_شمار++;

	بازگشت 0;
}

ایستا صحیح __fdt_parse_domain(ثابت تهی *fdt, صحیح domain_offset, تهی *opaque)
{
	u32 val32;
	u64 val64;
	ثابت u32 *val;
	ثابت نویسه *inherit;
	ساختار sbi_domain *dom;
	ساختار sbi_hartmask *mask;
	ساختار parse_region_data preg;
	ساختار sbi_domain_memregion *reg;
	صحیح inheritance_mode = FDT_ریشه_REGION_INHERIT_M_ONLY;
	صحیح i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(اندازه(*dom));
	اگر (!dom)
		بازگشت SBI_ENOMEM;

	dom->regions = sbi_calloc(اندازه(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_شمار + 1);
	اگر (!dom->regions) {
		err = SBI_ENOMEM;
		برو fail_free_domain;
	}
	preg.dom = dom;
	preg.region_شمار = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_شمار;

	mask = sbi_zalloc(اندازه(*mask));
	اگر (!mask) {
		err = SBI_ENOMEM;
		برو fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_offset, پوچ),
		    اندازه(dom->name));
	dom->name[اندازه(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / اندازه(u32);
	اگر (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	وگرنه
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / اندازه(u32);
	اگر (val && len) {
		برای (i = 0; i < len; i++) {
			cpu_offset = fdt_گره_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			اگر (cpu_offset < 0) {
				err = cpu_offset;
				برو fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			اگر (err)
				برو fail_free_all;

			اگر (!fdt_گره_is_enabled(fdt, cpu_offset))
				ادامه;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	اگر (err)
		برو fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	اگر (inherit && len > 0) {
		اگر (!strcmp(inherit, "all"))
			inheritance_mode = FDT_ریشه_REGION_INHERIT_ALL;
		وگرنه اگر (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_ریشه_REGION_INHERIT_M_ONLY;
		وگرنه {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			برو fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&ریشه, reg) {
		bool copy = false;

		انتخاب (inheritance_mode) {
		حالت FDT_ریشه_REGION_INHERIT_ALL:
			copy = true;
			بشکن;
		حالت FDT_ریشه_REGION_INHERIT_M_ONLY:
			اگر (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			بشکن;
		}

		اگر (!copy)
			ادامه;

		اگر (preg.max_regions <= preg.region_شمار) {
			err = SBI_EINVAL;
			برو fail_free_all;
		}
		memcpy(&dom->regions[preg.region_شمار++], reg, اندازه(*reg));
	}
	dom->fw_region_inited = ریشه.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	اگر (val && len >= 4) {
		cpu_offset = fdt_گره_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		اگر (cpu_offset >= 0 && fdt_گره_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	اگر (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} وگرنه {
		val64 = ریشه.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	اگر (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} وگرنه {
		val64 = ریشه.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	اگر (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		اگر (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} وگرنه {
		val32 = ریشه.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	اگر (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", پوچ))
		dom->system_reset_allowed = true;
	وگرنه
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	اگر (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", پوچ))
		dom->system_suspend_allowed = true;
	وگرنه
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	اگر (err)
		برو fail_free_all;

	بازگشت 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	بازگشت err;
}

صحیح fdt_domains_populate(ثابت تهی *fdt)
{
	/* Sanity checks */
	اگر (!fdt)
		بازگشت SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	بازگشت fdt_iterate_each_domain_ro(fdt, پوچ, __fdt_parse_domain);
}
