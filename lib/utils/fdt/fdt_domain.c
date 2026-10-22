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

صحیح fdt_iterate_each_domain(خالی *fdt, خالی *opaque,
			    صحیح (*fn)(خالی *fdt, صحیح domain_انحراف,
				       خالی *opaque))
{
	صحیح rc, doffset, poffset;

	اگر (!fdt || !fn)
		واپس SBI_EINVAL;

	poffset = fdt_راستہ_انحراف(fdt, "/chosen");
	اگر (poffset < 0)
		واپس 0;
	poffset = fdt_گرہ_انحراف_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	اگر (poffset < 0)
		واپس 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		اگر (fdt_گرہ_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			جاری;

		rc = fn(fdt, doffset, opaque);
		اگر (rc)
			واپس rc;
	}

	واپس 0;
}

ساکن صحیح fdt_iterate_each_domain_ro(ثابت خالی *fdt, خالی *opaque,
				      صحیح (*fn)(ثابت خالی *fdt, صحیح domain_انحراف,
						خالی *opaque))
{
	واپس fdt_iterate_each_domain((خالی *)fdt, opaque,
				       (صحیح (*)(خالی *, صحیح,  خالی *))fn);
}

صحیح fdt_iterate_each_memregion(خالی *fdt, صحیح domain_انحراف, خالی *opaque,
			       صحیح (*fn)(خالی *fdt, صحیح domain_انحراف,
					 صحیح region_انحراف, u32 region_access,
					 خالی *opaque))
{
	u32 i, rcount;
	صحیح rc, len, region_انحراف;
	ثابت u32 *regions;

	اگر (!fdt || (domain_انحراف < 0) || !fn)
		واپس SBI_EINVAL;

	اگر (fdt_گرہ_check_compatible(fdt, domain_انحراف,
				      "opensbi,domain,instance"))
		واپس SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_انحراف, "regions", &len);
	اگر (!regions)
		واپس 0;

	rcount = (u32)len / (حجم(u32) * 2);
	برائے (i = 0; i < rcount; i++) {
		region_انحراف = fdt_گرہ_انحراف_by_phandle(fdt,
						fdt32_مقصد_cpu(regions[2 * i]));
		اگر (region_انحراف < 0)
			واپس region_انحراف;

		اگر (fdt_گرہ_check_compatible(fdt, region_انحراف,
					      "opensbi,domain,memregion"))
			واپس SBI_EINVAL;

		rc = fn(fdt, domain_انحراف, region_انحراف,
			fdt32_مقصد_cpu(regions[(2 * i) + 1]), opaque);
		اگر (rc)
			واپس rc;
	}

	واپس 0;
}

ساکن صحیح fdt_iterate_each_memregion_ro(ثابت خالی *fdt, صحیح domain_انحراف, خالی *opaque,
					 صحیح (*fn)(ثابت خالی *fdt, صحیح domain_انحراف,
						   صحیح region_انحراف, u32 region_access,
						   خالی *opaque))
{
	واپس fdt_iterate_each_memregion((خالی *)fdt, domain_انحراف, opaque,
					  (صحیح (*)(خالی *, صحیح, صحیح, u32, خالی *))fn);
}

ساخت __fixup_find_domain_offset_info {
	ثابت حرف *name;
	صحیح *doffset;
};

ساکن صحیح __fixup_find_domain_offset(خالی *fdt, صحیح doff, خالی *p)
{
	ساخت __fixup_find_domain_offset_info *fdo = p;

	اگر (!لڑی_موازنہ(fdo->name, fdt_get_name(fdt, doff, تہی), لڑی_لمبائی(fdo->name)))
		*fdo->doffset = doff;

	واپس 0;
}

#تعریف_کرو DISABLE_DEVICES_نقاب	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

ساکن صحیح __fixup_count_disable_devices(خالی *fdt, صحیح doff, صحیح roff,
					 u32 perm, خالی *p)
{
	صحیح len;
	u32 *dcount = p;

	اگر (perm & DISABLE_DEVICES_نقاب)
		واپس 0;

	len = 0;
	اگر (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / حجم(u32);

	واپس 0;
}

ساکن صحیح __fixup_disable_devices(خالی *fdt, صحیح doff, صحیح roff,
				   u32 raccess, خالی *p)
{
	صحیح i, len, coff;
	ثابت u32 *devices;

	اگر (raccess & DISABLE_DEVICES_نقاب)
		واپس 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	اگر (!devices)
		واپس 0;
	len = len / حجم(u32);

	برائے (i = 0; i < len; i++) {
		coff = fdt_گرہ_انحراف_by_phandle(fdt,
					fdt32_مقصد_cpu(devices[i]));
		اگر (coff < 0)
			واپس coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	واپس 0;
}

خالی fdt_domain_fixup(خالی *fdt)
{
	u32 dcount;
	صحیح err, poffset, doffset;
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();
	ساخت __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	اگر (!dom->اشاریہ_2)
		جاؤ skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	اگر (doffset < 0)
		جاؤ skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	اگر (!dcount)
		جاؤ skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	اگر (err < 0)
		واپس;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	اگر (doffset < 0)
		جاؤ skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, تہی,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_راستہ_انحراف(fdt, "/chosen");
	اگر (poffset < 0)
		واپس;
	poffset = fdt_گرہ_انحراف_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	اگر (poffset < 0)
		واپس;
	fdt_nop_گرہ(fdt, poffset);
}

#تعریف_کرو FDT_DOMAIN_REGION_MAX_گنتی		16
#تعریف_کرو FDT_جڑ_REGION_INHERIT_M_ONLY		0
#تعریف_کرو FDT_جڑ_REGION_INHERIT_ALL		1

ساخت parse_region_ڈیٹا {
	ساخت sbi_domain *dom;
	u32 region_گنتی;
	u32 max_regions;
};

ساکن صحیح __fdt_parse_region(ثابت خالی *fdt, صحیح domain_انحراف,
			      صحیح region_انحراف, u32 region_access,
			      خالی *opaque)
{
	صحیح len;
	u32 val32;
	u64 val64;
	ثابت u32 *val;
	غیرعلامتی طویل بنیاد_2, ترتیب, flags;
	ساخت parse_region_ڈیٹا *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	اگر (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_نقاب)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_نقاب))
		واپس SBI_EINVAL;

	/* Find next region of the domain */
	اگر (preg->max_regions <= preg->region_گنتی)
		واپس SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_انحراف, "base", &len);
	اگر (!val || len != 8)
		واپس SBI_EINVAL;
	val64 = fdt32_مقصد_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_مقصد_cpu(val[1]);
	بنیاد_2 = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_انحراف, "order", &len);
	اگر (!val || len != 4)
		واپس SBI_EINVAL;
	val32 = fdt32_مقصد_cpu(*val);
	اگر (val32 < 3 || __riscv_xlen < val32)
		واپس SBI_EINVAL;
	ترتیب = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_نقاب
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	اگر (fdt_get_property(fdt, region_انحراف, "mmio", تہی))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(بنیاد_2, (ترتیب == __riscv_xlen) ? ~0UL : ثنائی_ہندسہ(ترتیب),
				  flags, &preg->dom->regions[preg->region_گنتی]);

	preg->region_گنتی++;

	واپس 0;
}

ساکن صحیح __fdt_parse_domain(ثابت خالی *fdt, صحیح domain_انحراف, خالی *opaque)
{
	u32 val32;
	u64 val64;
	ثابت u32 *val;
	ثابت حرف *inherit;
	ساخت sbi_domain *dom;
	ساخت sbi_hartmask *نقاب;
	ساخت parse_region_ڈیٹا preg;
	ساخت sbi_domain_memregion *reg;
	صحیح inheritance_منوال = FDT_جڑ_REGION_INHERIT_M_ONLY;
	صحیح i, err = 0, len, cpu_انحراف;

	dom = sbi_zalloc(حجم(*dom));
	اگر (!dom)
		واپس SBI_ENOMEM;

	dom->regions = sbi_calloc(حجم(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_گنتی + 1);
	اگر (!dom->regions) {
		err = SBI_ENOMEM;
		جاؤ fail_free_domain;
	}
	preg.dom = dom;
	preg.region_گنتی = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_گنتی;

	نقاب = sbi_zalloc(حجم(*نقاب));
	اگر (!نقاب) {
		err = SBI_ENOMEM;
		جاؤ fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_انحراف, تہی),
		    حجم(dom->name));
	dom->name[حجم(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_انحراف, "init-order", &len);
	len = len / حجم(u32);
	اگر (val && len)
		dom->init_ترتیب = fdt32_مقصد_cpu(val[0]);
	ورنہ
		dom->init_ترتیب = (u32)domain_انحراف;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(نقاب);
	dom->possible_harts = نقاب;
	val = fdt_getprop(fdt, domain_انحراف, "possible-harts", &len);
	len = len / حجم(u32);
	اگر (val && len) {
		برائے (i = 0; i < len; i++) {
			cpu_انحراف = fdt_گرہ_انحراف_by_phandle(fdt,
							fdt32_مقصد_cpu(val[i]));
			اگر (cpu_انحراف < 0) {
				err = cpu_انحراف;
				جاؤ fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_انحراف, &val32);
			اگر (err)
				جاؤ fail_free_all;

			اگر (!fdt_گرہ_is_enabled(fdt, cpu_انحراف))
				جاری;

			sbi_hartmask_سیٹ_hartid(val32, نقاب);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_انحراف, &preg,
					    __fdt_parse_region);
	اگر (err)
		جاؤ fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_انحراف,
			       "root-regions-inheritance", &len);
	اگر (inherit && len > 0) {
		اگر (!strcmp(inherit, "all"))
			inheritance_منوال = FDT_جڑ_REGION_INHERIT_ALL;
		ورنہ اگر (!strcmp(inherit, "m-only"))
			inheritance_منوال = FDT_جڑ_REGION_INHERIT_M_ONLY;
		ورنہ {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			جاؤ fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&جڑ, reg) {
		bool copy = false;

		انتخاب (inheritance_منوال) {
		حالت FDT_جڑ_REGION_INHERIT_ALL:
			copy = true;
			توڑو;
		حالت FDT_جڑ_REGION_INHERIT_M_ONLY:
			اگر (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			توڑو;
		}

		اگر (!copy)
			جاری;

		اگر (preg.max_regions <= preg.region_گنتی) {
			err = SBI_EINVAL;
			جاؤ fail_free_all;
		}
		حافظہ_نقل(&dom->regions[preg.region_گنتی++], reg, حجم(*reg));
	}
	dom->fw_region_inited = جڑ.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_انحراف, "boot-hart", &len);
	اگر (val && len >= 4) {
		cpu_انحراف = fdt_گرہ_انحراف_by_phandle(fdt,
							 fdt32_مقصد_cpu(*val));
		اگر (cpu_انحراف >= 0 && fdt_گرہ_is_enabled(fdt, cpu_انحراف))
			fdt_parse_hart_id(fdt, cpu_انحراف, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_انحراف, "next-arg1", &len);
	اگر (val && len >= 8) {
		val64 = fdt32_مقصد_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_مقصد_cpu(val[1]);
	} ورنہ {
		val64 = جڑ.اگلا_arg1;
	}
	dom->اگلا_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_انحراف, "next-addr", &len);
	اگر (val && len >= 8) {
		val64 = fdt32_مقصد_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_مقصد_cpu(val[1]);
	} ورنہ {
		val64 = جڑ.اگلا_addr;
	}
	dom->اگلا_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_انحراف, "next-mode", &len);
	اگر (val && len >= 4) {
		val32 = fdt32_مقصد_cpu(val[0]);
		اگر (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} ورنہ {
		val32 = جڑ.اگلا_منوال;
	}
	dom->اگلا_منوال = val32;

	/* Read "system-reset-allowed" DT property */
	اگر (fdt_get_property(fdt, domain_انحراف,
			     "system-reset-allowed", تہی))
		dom->system_reset_allowed = true;
	ورنہ
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	اگر (fdt_get_property(fdt, domain_انحراف,
			     "system-suspend-allowed", تہی))
		dom->system_suspend_allowed = true;
	ورنہ
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_رجسٹر(dom);
	اگر (err)
		جاؤ fail_free_all;

	واپس 0;

fail_free_all:
	sbi_free(نقاب);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	واپس err;
}

صحیح fdt_domains_populate(ثابت خالی *fdt)
{
	/* Sanity checks */
	اگر (!fdt)
		واپس SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	واپس fdt_iterate_each_domain_ro(fdt, تہی, __fdt_parse_domain);
}
