// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <libfdt_env.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hartmask.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_domain.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>

పూర్ణాంకం fdt_iterate_each_domain(శూన్య *fdt, శూన్య *opaque,
			    పూర్ణాంకం (*fn)(శూన్య *fdt, పూర్ణాంకం domain_స్థానభ్రంశం,
				       శూన్య *opaque))
{
	పూర్ణాంకం rc, doffset, poffset;

	ఒకవేళ (!fdt || !fn)
		తిరిగి SBI_EINVAL;

	poffset = fdt_మార్గం_స్థానభ్రంశం(fdt, "/chosen");
	ఒకవేళ (poffset < 0)
		తిరిగి 0;
	poffset = fdt_కణుపు_స్థానభ్రంశం_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ఒకవేళ (poffset < 0)
		తిరిగి 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		ఒకవేళ (fdt_కణుపు_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			కొనసాగు;

		rc = fn(fdt, doffset, opaque);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం fdt_iterate_each_domain_ro(స్థిరరాశి శూన్య *fdt, శూన్య *opaque,
				      పూర్ణాంకం (*fn)(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం domain_స్థానభ్రంశం,
						శూన్య *opaque))
{
	తిరిగి fdt_iterate_each_domain((శూన్య *)fdt, opaque,
				       (పూర్ణాంకం (*)(శూన్య *, పూర్ణాంకం,  శూన్య *))fn);
}

పూర్ణాంకం fdt_iterate_each_memregion(శూన్య *fdt, పూర్ణాంకం domain_స్థానభ్రంశం, శూన్య *opaque,
			       పూర్ణాంకం (*fn)(శూన్య *fdt, పూర్ణాంకం domain_స్థానభ్రంశం,
					 పూర్ణాంకం region_స్థానభ్రంశం, u32 region_access,
					 శూన్య *opaque))
{
	u32 i, rcount;
	పూర్ణాంకం rc, len, region_స్థానభ్రంశం;
	స్థిరరాశి u32 *regions;

	ఒకవేళ (!fdt || (domain_స్థానభ్రంశం < 0) || !fn)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (fdt_కణుపు_check_compatible(fdt, domain_స్థానభ్రంశం,
				      "opensbi,domain,instance"))
		తిరిగి SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_స్థానభ్రంశం, "regions", &len);
	ఒకవేళ (!regions)
		తిరిగి 0;

	rcount = (u32)len / (పరిమాణం(u32) * 2);
	కొరకు (i = 0; i < rcount; i++) {
		region_స్థానభ్రంశం = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt,
						fdt32_గమ్యం_cpu(regions[2 * i]));
		ఒకవేళ (region_స్థానభ్రంశం < 0)
			తిరిగి region_స్థానభ్రంశం;

		ఒకవేళ (fdt_కణుపు_check_compatible(fdt, region_స్థానభ్రంశం,
					      "opensbi,domain,memregion"))
			తిరిగి SBI_EINVAL;

		rc = fn(fdt, domain_స్థానభ్రంశం, region_స్థానభ్రంశం,
			fdt32_గమ్యం_cpu(regions[(2 * i) + 1]), opaque);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం fdt_iterate_each_memregion_ro(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం domain_స్థానభ్రంశం, శూన్య *opaque,
					 పూర్ణాంకం (*fn)(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం domain_స్థానభ్రంశం,
						   పూర్ణాంకం region_స్థానభ్రంశం, u32 region_access,
						   శూన్య *opaque))
{
	తిరిగి fdt_iterate_each_memregion((శూన్య *)fdt, domain_స్థానభ్రంశం, opaque,
					  (పూర్ణాంకం (*)(శూన్య *, పూర్ణాంకం, పూర్ణాంకం, u32, శూన్య *))fn);
}

నిర్మాణం __fixup_find_domain_offset_info {
	స్థిరరాశి అక్షరం *name;
	పూర్ణాంకం *doffset;
};

స్థిర పూర్ణాంకం __fixup_find_domain_offset(శూన్య *fdt, పూర్ణాంకం doff, శూన్య *p)
{
	నిర్మాణం __fixup_find_domain_offset_info *fdo = p;

	ఒకవేళ (!అక్షరశ్రేణి_పోలిక(fdo->name, fdt_get_name(fdt, doff, ఖాళీ), అక్షరశ్రేణి_పొడవు(fdo->name)))
		*fdo->doffset = doff;

	తిరిగి 0;
}

#నిర్వచించు DISABLE_DEVICES_ముసుగు	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

స్థిర పూర్ణాంకం __fixup_count_disable_devices(శూన్య *fdt, పూర్ణాంకం doff, పూర్ణాంకం roff,
					 u32 perm, శూన్య *p)
{
	పూర్ణాంకం len;
	u32 *dcount = p;

	ఒకవేళ (perm & DISABLE_DEVICES_ముసుగు)
		తిరిగి 0;

	len = 0;
	ఒకవేళ (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / పరిమాణం(u32);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం __fixup_disable_devices(శూన్య *fdt, పూర్ణాంకం doff, పూర్ణాంకం roff,
				   u32 raccess, శూన్య *p)
{
	పూర్ణాంకం i, len, coff;
	స్థిరరాశి u32 *devices;

	ఒకవేళ (raccess & DISABLE_DEVICES_ముసుగు)
		తిరిగి 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	ఒకవేళ (!devices)
		తిరిగి 0;
	len = len / పరిమాణం(u32);

	కొరకు (i = 0; i < len; i++) {
		coff = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt,
					fdt32_గమ్యం_cpu(devices[i]));
		ఒకవేళ (coff < 0)
			తిరిగి coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	తిరిగి 0;
}

శూన్య fdt_domain_fixup(శూన్య *fdt)
{
	u32 dcount;
	పూర్ణాంకం err, poffset, doffset;
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();
	నిర్మాణం __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	ఒకవేళ (!dom->సూచీ_2)
		వెళ్ళు skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ఒకవేళ (doffset < 0)
		వెళ్ళు skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	ఒకవేళ (!dcount)
		వెళ్ళు skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	ఒకవేళ (err < 0)
		తిరిగి;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ఒకవేళ (doffset < 0)
		వెళ్ళు skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, ఖాళీ,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_మార్గం_స్థానభ్రంశం(fdt, "/chosen");
	ఒకవేళ (poffset < 0)
		తిరిగి;
	poffset = fdt_కణుపు_స్థానభ్రంశం_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ఒకవేళ (poffset < 0)
		తిరిగి;
	fdt_nop_కణుపు(fdt, poffset);
}

#నిర్వచించు FDT_DOMAIN_REGION_MAX_లెక్క		16
#నిర్వచించు FDT_మూలం_REGION_INHERIT_M_ONLY		0
#నిర్వచించు FDT_మూలం_REGION_INHERIT_ALL		1

నిర్మాణం parse_region_దత్తాంశం {
	నిర్మాణం sbi_domain *dom;
	u32 region_లెక్క;
	u32 max_regions;
};

స్థిర పూర్ణాంకం __fdt_parse_region(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం domain_స్థానభ్రంశం,
			      పూర్ణాంకం region_స్థానభ్రంశం, u32 region_access,
			      శూన్య *opaque)
{
	పూర్ణాంకం len;
	u32 val32;
	u64 val64;
	స్థిరరాశి u32 *val;
	గుర్తులేని దీర్ఘ పునాది_2, క్రమం, flags;
	నిర్మాణం parse_region_దత్తాంశం *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	ఒకవేళ (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_ముసుగు)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_ముసుగు))
		తిరిగి SBI_EINVAL;

	/* Find next region of the domain */
	ఒకవేళ (preg->max_regions <= preg->region_లెక్క)
		తిరిగి SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_స్థానభ్రంశం, "base", &len);
	ఒకవేళ (!val || len != 8)
		తిరిగి SBI_EINVAL;
	val64 = fdt32_గమ్యం_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_గమ్యం_cpu(val[1]);
	పునాది_2 = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_స్థానభ్రంశం, "order", &len);
	ఒకవేళ (!val || len != 4)
		తిరిగి SBI_EINVAL;
	val32 = fdt32_గమ్యం_cpu(*val);
	ఒకవేళ (val32 < 3 || __riscv_xlen < val32)
		తిరిగి SBI_EINVAL;
	క్రమం = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_ముసుగు
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	ఒకవేళ (fdt_get_property(fdt, region_స్థానభ్రంశం, "mmio", ఖాళీ))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(పునాది_2, (క్రమం == __riscv_xlen) ? ~0UL : బిట్టు(క్రమం),
				  flags, &preg->dom->regions[preg->region_లెక్క]);

	preg->region_లెక్క++;

	తిరిగి 0;
}

స్థిర పూర్ణాంకం __fdt_parse_domain(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం domain_స్థానభ్రంశం, శూన్య *opaque)
{
	u32 val32;
	u64 val64;
	స్థిరరాశి u32 *val;
	స్థిరరాశి అక్షరం *inherit;
	నిర్మాణం sbi_domain *dom;
	నిర్మాణం sbi_hartmask *ముసుగు;
	నిర్మాణం parse_region_దత్తాంశం preg;
	నిర్మాణం sbi_domain_memregion *reg;
	పూర్ణాంకం inheritance_బహుళకం = FDT_మూలం_REGION_INHERIT_M_ONLY;
	పూర్ణాంకం i, err = 0, len, cpu_స్థానభ్రంశం;

	dom = sbi_zalloc(పరిమాణం(*dom));
	ఒకవేళ (!dom)
		తిరిగి SBI_ENOMEM;

	dom->regions = sbi_calloc(పరిమాణం(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_లెక్క + 1);
	ఒకవేళ (!dom->regions) {
		err = SBI_ENOMEM;
		వెళ్ళు fail_free_domain;
	}
	preg.dom = dom;
	preg.region_లెక్క = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_లెక్క;

	ముసుగు = sbi_zalloc(పరిమాణం(*ముసుగు));
	ఒకవేళ (!ముసుగు) {
		err = SBI_ENOMEM;
		వెళ్ళు fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_స్థానభ్రంశం, ఖాళీ),
		    పరిమాణం(dom->name));
	dom->name[పరిమాణం(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_స్థానభ్రంశం, "init-order", &len);
	len = len / పరిమాణం(u32);
	ఒకవేళ (val && len)
		dom->init_క్రమం = fdt32_గమ్యం_cpu(val[0]);
	లేకపోతే
		dom->init_క్రమం = (u32)domain_స్థానభ్రంశం;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(ముసుగు);
	dom->possible_harts = ముసుగు;
	val = fdt_getprop(fdt, domain_స్థానభ్రంశం, "possible-harts", &len);
	len = len / పరిమాణం(u32);
	ఒకవేళ (val && len) {
		కొరకు (i = 0; i < len; i++) {
			cpu_స్థానభ్రంశం = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt,
							fdt32_గమ్యం_cpu(val[i]));
			ఒకవేళ (cpu_స్థానభ్రంశం < 0) {
				err = cpu_స్థానభ్రంశం;
				వెళ్ళు fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_స్థానభ్రంశం, &val32);
			ఒకవేళ (err)
				వెళ్ళు fail_free_all;

			ఒకవేళ (!fdt_కణుపు_is_enabled(fdt, cpu_స్థానభ్రంశం))
				కొనసాగు;

			sbi_hartmask_సమితి_hartid(val32, ముసుగు);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_స్థానభ్రంశం, &preg,
					    __fdt_parse_region);
	ఒకవేళ (err)
		వెళ్ళు fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_స్థానభ్రంశం,
			       "root-regions-inheritance", &len);
	ఒకవేళ (inherit && len > 0) {
		ఒకవేళ (!strcmp(inherit, "all"))
			inheritance_బహుళకం = FDT_మూలం_REGION_INHERIT_ALL;
		లేకపోతే ఒకవేళ (!strcmp(inherit, "m-only"))
			inheritance_బహుళకం = FDT_మూలం_REGION_INHERIT_M_ONLY;
		లేకపోతే {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			వెళ్ళు fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&మూలం, reg) {
		bool copy = false;

		ఎంపిక (inheritance_బహుళకం) {
		స్థితి FDT_మూలం_REGION_INHERIT_ALL:
			copy = true;
			ఆపు;
		స్థితి FDT_మూలం_REGION_INHERIT_M_ONLY:
			ఒకవేళ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			ఆపు;
		}

		ఒకవేళ (!copy)
			కొనసాగు;

		ఒకవేళ (preg.max_regions <= preg.region_లెక్క) {
			err = SBI_EINVAL;
			వెళ్ళు fail_free_all;
		}
		జ్ఞాపకం_నకలు(&dom->regions[preg.region_లెక్క++], reg, పరిమాణం(*reg));
	}
	dom->fw_region_inited = మూలం.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_స్థానభ్రంశం, "boot-hart", &len);
	ఒకవేళ (val && len >= 4) {
		cpu_స్థానభ్రంశం = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt,
							 fdt32_గమ్యం_cpu(*val));
		ఒకవేళ (cpu_స్థానభ్రంశం >= 0 && fdt_కణుపు_is_enabled(fdt, cpu_స్థానభ్రంశం))
			fdt_parse_hart_id(fdt, cpu_స్థానభ్రంశం, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_స్థానభ్రంశం, "next-arg1", &len);
	ఒకవేళ (val && len >= 8) {
		val64 = fdt32_గమ్యం_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_గమ్యం_cpu(val[1]);
	} లేకపోతే {
		val64 = మూలం.తదుపరి_arg1;
	}
	dom->తదుపరి_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_స్థానభ్రంశం, "next-addr", &len);
	ఒకవేళ (val && len >= 8) {
		val64 = fdt32_గమ్యం_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_గమ్యం_cpu(val[1]);
	} లేకపోతే {
		val64 = మూలం.తదుపరి_addr;
	}
	dom->తదుపరి_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_స్థానభ్రంశం, "next-mode", &len);
	ఒకవేళ (val && len >= 4) {
		val32 = fdt32_గమ్యం_cpu(val[0]);
		ఒకవేళ (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} లేకపోతే {
		val32 = మూలం.తదుపరి_బహుళకం;
	}
	dom->తదుపరి_బహుళకం = val32;

	/* Read "system-reset-allowed" DT property */
	ఒకవేళ (fdt_get_property(fdt, domain_స్థానభ్రంశం,
			     "system-reset-allowed", ఖాళీ))
		dom->system_reset_allowed = true;
	లేకపోతే
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	ఒకవేళ (fdt_get_property(fdt, domain_స్థానభ్రంశం,
			     "system-suspend-allowed", ఖాళీ))
		dom->system_suspend_allowed = true;
	లేకపోతే
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_రిజిస్టరు(dom);
	ఒకవేళ (err)
		వెళ్ళు fail_free_all;

	తిరిగి 0;

fail_free_all:
	sbi_free(ముసుగు);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	తిరిగి err;
}

పూర్ణాంకం fdt_domains_populate(స్థిరరాశి శూన్య *fdt)
{
	/* Sanity checks */
	ఒకవేళ (!fdt)
		తిరిగి SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	తిరిగి fdt_iterate_each_domain_ro(fdt, ఖాళీ, __fdt_parse_domain);
}
