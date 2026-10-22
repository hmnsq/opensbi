// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <libfdt_env.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hartmask.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_iterate_each_domain(ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *opaque,
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*fn)(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା domain_offset,
				       ଶୂନ୍ୟ *opaque))
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, doffset, poffset;

	ଯଦି (!fdt || !fn)
		ଫେରନ୍ତୁ SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	ଯଦି (poffset < 0)
		ଫେରନ୍ତୁ 0;
	poffset = fdt_ଗଣ୍ଠି_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ଯଦି (poffset < 0)
		ଫେରନ୍ତୁ 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		ଯଦି (fdt_ଗଣ୍ଠି_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			ଜାରି;

		rc = fn(fdt, doffset, opaque);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_iterate_each_domain_ro(ଅଚଳ ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *opaque,
				      ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*fn)(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା domain_offset,
						ଶୂନ୍ୟ *opaque))
{
	ଫେରନ୍ତୁ fdt_iterate_each_domain((ଶୂନ୍ୟ *)fdt, opaque,
				       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*)(ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା,  ଶୂନ୍ୟ *))fn);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_iterate_each_memregion(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା domain_offset, ଶୂନ୍ୟ *opaque,
			       ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*fn)(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା domain_offset,
					 ପୂର୍ଣ୍ଣସଂଖ୍ୟା region_offset, u32 region_access,
					 ଶୂନ୍ୟ *opaque))
{
	u32 i, rcount;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, len, region_offset;
	ଅଚଳ u32 *regions;

	ଯଦି (!fdt || (domain_offset < 0) || !fn)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (fdt_ଗଣ୍ଠି_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		ଫେରନ୍ତୁ SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	ଯଦି (!regions)
		ଫେରନ୍ତୁ 0;

	rcount = (u32)len / (ଆକାର(u32) * 2);
	ପାଇଁ (i = 0; i < rcount; i++) {
		region_offset = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		ଯଦି (region_offset < 0)
			ଫେରନ୍ତୁ region_offset;

		ଯଦି (fdt_ଗଣ୍ଠି_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			ଫେରନ୍ତୁ SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_iterate_each_memregion_ro(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା domain_offset, ଶୂନ୍ୟ *opaque,
					 ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*fn)(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା domain_offset,
						   ପୂର୍ଣ୍ଣସଂଖ୍ୟା region_offset, u32 region_access,
						   ଶୂନ୍ୟ *opaque))
{
	ଫେରନ୍ତୁ fdt_iterate_each_memregion((ଶୂନ୍ୟ *)fdt, domain_offset, opaque,
					  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*)(ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, u32, ଶୂନ୍ୟ *))fn);
}

ସଂରଚନା __fixup_find_domain_offset_info {
	ଅଚଳ ଅକ୍ଷର *name;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *doffset;
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା __fixup_find_domain_offset(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା doff, ଶୂନ୍ୟ *p)
{
	ସଂରଚନା __fixup_find_domain_offset_info *fdo = p;

	ଯଦି (!strncmp(fdo->name, fdt_get_name(fdt, doff, ଖାଲି), strlen(fdo->name)))
		*fdo->doffset = doff;

	ଫେରନ୍ତୁ 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା __fixup_count_disable_devices(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା doff, ପୂର୍ଣ୍ଣସଂଖ୍ୟା roff,
					 u32 perm, ଶୂନ୍ୟ *p)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
	u32 *dcount = p;

	ଯଦି (perm & DISABLE_DEVICES_MASK)
		ଫେରନ୍ତୁ 0;

	len = 0;
	ଯଦି (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / ଆକାର(u32);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା __fixup_disable_devices(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା doff, ପୂର୍ଣ୍ଣସଂଖ୍ୟା roff,
				   u32 raccess, ଶୂନ୍ୟ *p)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, len, coff;
	ଅଚଳ u32 *devices;

	ଯଦି (raccess & DISABLE_DEVICES_MASK)
		ଫେରନ୍ତୁ 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	ଯଦି (!devices)
		ଫେରନ୍ତୁ 0;
	len = len / ଆକାର(u32);

	ପାଇଁ (i = 0; i < len; i++) {
		coff = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		ଯଦି (coff < 0)
			ଫେରନ୍ତୁ coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ fdt_domain_fixup(ଶୂନ୍ୟ *fdt)
{
	u32 dcount;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err, poffset, doffset;
	ସଂରଚନା sbi_domain *dom = sbi_domain_thishart_ptr();
	ସଂରଚନା __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	ଯଦି (!dom->index)
		ଯାଅ skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ଯଦି (doffset < 0)
		ଯାଅ skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	ଯଦି (!dcount)
		ଯାଅ skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	ଯଦି (err < 0)
		ଫେରନ୍ତୁ;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ଯଦି (doffset < 0)
		ଯାଅ skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, ଖାଲି,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	ଯଦି (poffset < 0)
		ଫେରନ୍ତୁ;
	poffset = fdt_ଗଣ୍ଠି_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ଯଦି (poffset < 0)
		ଫେରନ୍ତୁ;
	fdt_nop_ଗଣ୍ଠି(fdt, poffset);
}

#define FDT_DOMAIN_REGION_MAX_ଗଣନା		16
#define FDT_ମୂଳ_REGION_INHERIT_M_ONLY		0
#define FDT_ମୂଳ_REGION_INHERIT_ALL		1

ସଂରଚନା parse_region_data {
	ସଂରଚନା sbi_domain *dom;
	u32 region_ଗଣନା;
	u32 max_regions;
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା __fdt_parse_region(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା domain_offset,
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା region_offset, u32 region_access,
			      ଶୂନ୍ୟ *opaque)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
	u32 val32;
	u64 val64;
	ଅଚଳ u32 *val;
	unsigned long base, order, flags;
	ସଂରଚନା parse_region_data *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	ଯଦି (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Find next region of the domain */
	ଯଦି (preg->max_regions <= preg->region_ଗଣନା)
		ଫେରନ୍ତୁ SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	ଯଦି (!val || len != 8)
		ଫେରନ୍ତୁ SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	ଯଦି (!val || len != 4)
		ଫେରନ୍ତୁ SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	ଯଦି (val32 < 3 || __riscv_xlen < val32)
		ଫେରନ୍ତୁ SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	ଯଦି (fdt_get_property(fdt, region_offset, "mmio", ଖାଲି))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_ଗଣନା]);

	preg->region_ଗଣନା++;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା __fdt_parse_domain(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା domain_offset, ଶୂନ୍ୟ *opaque)
{
	u32 val32;
	u64 val64;
	ଅଚଳ u32 *val;
	ଅଚଳ ଅକ୍ଷର *inherit;
	ସଂରଚନା sbi_domain *dom;
	ସଂରଚନା sbi_hartmask *mask;
	ସଂରଚନା parse_region_data preg;
	ସଂରଚନା sbi_domain_memregion *reg;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା inheritance_mode = FDT_ମୂଳ_REGION_INHERIT_M_ONLY;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(ଆକାର(*dom));
	ଯଦି (!dom)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	dom->regions = sbi_calloc(ଆକାର(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_ଗଣନା + 1);
	ଯଦି (!dom->regions) {
		err = SBI_ENOMEM;
		ଯାଅ fail_free_domain;
	}
	preg.dom = dom;
	preg.region_ଗଣନା = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_ଗଣନା;

	mask = sbi_zalloc(ଆକାର(*mask));
	ଯଦି (!mask) {
		err = SBI_ENOMEM;
		ଯାଅ fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_offset, ଖାଲି),
		    ଆକାର(dom->name));
	dom->name[ଆକାର(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / ଆକାର(u32);
	ଯଦି (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	ଅନ୍ୟଥା
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / ଆକାର(u32);
	ଯଦି (val && len) {
		ପାଇଁ (i = 0; i < len; i++) {
			cpu_offset = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			ଯଦି (cpu_offset < 0) {
				err = cpu_offset;
				ଯାଅ fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			ଯଦି (err)
				ଯାଅ fail_free_all;

			ଯଦି (!fdt_ଗଣ୍ଠି_is_enabled(fdt, cpu_offset))
				ଜାରି;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	ଯଦି (err)
		ଯାଅ fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	ଯଦି (inherit && len > 0) {
		ଯଦି (!strcmp(inherit, "all"))
			inheritance_mode = FDT_ମୂଳ_REGION_INHERIT_ALL;
		ଅନ୍ୟଥା ଯଦି (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_ମୂଳ_REGION_INHERIT_M_ONLY;
		ଅନ୍ୟଥା {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			ଯାଅ fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&ମୂଳ, reg) {
		bool copy = false;

		ବାଛ (inheritance_mode) {
		ସ୍ଥିତି FDT_ମୂଳ_REGION_INHERIT_ALL:
			copy = true;
			ଭାଙ୍ଗ;
		ସ୍ଥିତି FDT_ମୂଳ_REGION_INHERIT_M_ONLY:
			ଯଦି (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			ଭାଙ୍ଗ;
		}

		ଯଦି (!copy)
			ଜାରି;

		ଯଦି (preg.max_regions <= preg.region_ଗଣନା) {
			err = SBI_EINVAL;
			ଯାଅ fail_free_all;
		}
		memcpy(&dom->regions[preg.region_ଗଣନା++], reg, ଆକାର(*reg));
	}
	dom->fw_region_inited = ମୂଳ.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	ଯଦି (val && len >= 4) {
		cpu_offset = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		ଯଦି (cpu_offset >= 0 && fdt_ଗଣ୍ଠି_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	ଯଦି (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} ଅନ୍ୟଥା {
		val64 = ମୂଳ.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	ଯଦି (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} ଅନ୍ୟଥା {
		val64 = ମୂଳ.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	ଯଦି (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		ଯଦି (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} ଅନ୍ୟଥା {
		val32 = ମୂଳ.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	ଯଦି (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", ଖାଲି))
		dom->system_reset_allowed = true;
	ଅନ୍ୟଥା
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	ଯଦି (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", ଖାଲି))
		dom->system_suspend_allowed = true;
	ଅନ୍ୟଥା
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	ଯଦି (err)
		ଯାଅ fail_free_all;

	ଫେରନ୍ତୁ 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	ଫେରନ୍ତୁ err;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_domains_populate(ଅଚଳ ଶୂନ୍ୟ *fdt)
{
	/* Sanity checks */
	ଯଦି (!fdt)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	ଫେରନ୍ତୁ fdt_iterate_each_domain_ro(fdt, ଖାଲି, __fdt_parse_domain);
}
