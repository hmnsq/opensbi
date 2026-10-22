// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt_env.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hartmask.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>

ಪೂರ್ಣಾಙ್ಕ fdt_iterate_each_domain(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *opaque,
			    ಪೂರ್ಣಾಙ್ಕ (*fn)(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ domain_offset,
				       ಶೂನ್ಯ *opaque))
{
	ಪೂರ್ಣಾಙ್ಕ rc, doffset, poffset;

	ಯದಿ (!fdt || !fn)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	poffset = fdt_path_offset(fdt, "/chosen");
	ಯದಿ (poffset < 0)
		ಪ್ರತ್ಯಾಗಮ 0;
	poffset = fdt_ಗಂಟು_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ಯದಿ (poffset < 0)
		ಪ್ರತ್ಯಾಗಮ 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		ಯದಿ (fdt_ಗಂಟು_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			ಅನುವರ್ತ;

		rc = fn(fdt, doffset, opaque);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_iterate_each_domain_ro(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *opaque,
				      ಪೂರ್ಣಾಙ್ಕ (*fn)(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ domain_offset,
						ಶೂನ್ಯ *opaque))
{
	ಪ್ರತ್ಯಾಗಮ fdt_iterate_each_domain((ಶೂನ್ಯ *)fdt, opaque,
				       (ಪೂರ್ಣಾಙ್ಕ (*)(ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ,  ಶೂನ್ಯ *))fn);
}

ಪೂರ್ಣಾಙ್ಕ fdt_iterate_each_memregion(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ domain_offset, ಶೂನ್ಯ *opaque,
			       ಪೂರ್ಣಾಙ್ಕ (*fn)(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ domain_offset,
					 ಪೂರ್ಣಾಙ್ಕ region_offset, u32 region_access,
					 ಶೂನ್ಯ *opaque))
{
	u32 i, rcount;
	ಪೂರ್ಣಾಙ್ಕ rc, len, region_offset;
	ಧ್ರುವ u32 *regions;

	ಯದಿ (!fdt || (domain_offset < 0) || !fn)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (fdt_ಗಂಟು_check_compatible(fdt, domain_offset,
				      "opensbi,domain,instance"))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_offset, "regions", &len);
	ಯದಿ (!regions)
		ಪ್ರತ್ಯಾಗಮ 0;

	rcount = (u32)len / (ಪರಿಮಾಣ(u32) * 2);
	ಪ್ರತಿ (i = 0; i < rcount; i++) {
		region_offset = fdt_ಗಂಟು_offset_by_phandle(fdt,
						fdt32_to_cpu(regions[2 * i]));
		ಯದಿ (region_offset < 0)
			ಪ್ರತ್ಯಾಗಮ region_offset;

		ಯದಿ (fdt_ಗಂಟು_check_compatible(fdt, region_offset,
					      "opensbi,domain,memregion"))
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

		rc = fn(fdt, domain_offset, region_offset,
			fdt32_to_cpu(regions[(2 * i) + 1]), opaque);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_iterate_each_memregion_ro(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ domain_offset, ಶೂನ್ಯ *opaque,
					 ಪೂರ್ಣಾಙ್ಕ (*fn)(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ domain_offset,
						   ಪೂರ್ಣಾಙ್ಕ region_offset, u32 region_access,
						   ಶೂನ್ಯ *opaque))
{
	ಪ್ರತ್ಯಾಗಮ fdt_iterate_each_memregion((ಶೂನ್ಯ *)fdt, domain_offset, opaque,
					  (ಪೂರ್ಣಾಙ್ಕ (*)(ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, u32, ಶೂನ್ಯ *))fn);
}

ಸಂರಚನಾ __fixup_find_domain_offset_info {
	ಧ್ರುವ ವರ್ಣ *name;
	ಪೂರ್ಣಾಙ್ಕ *doffset;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ __fixup_find_domain_offset(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ doff, ಶೂನ್ಯ *p)
{
	ಸಂರಚನಾ __fixup_find_domain_offset_info *fdo = p;

	ಯದಿ (!strncmp(fdo->name, fdt_get_name(fdt, doff, ರಿಕ್ತ), strlen(fdo->name)))
		*fdo->doffset = doff;

	ಪ್ರತ್ಯಾಗಮ 0;
}

#define DISABLE_DEVICES_MASK	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ __fixup_count_disable_devices(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ doff, ಪೂರ್ಣಾಙ್ಕ roff,
					 u32 perm, ಶೂನ್ಯ *p)
{
	ಪೂರ್ಣಾಙ್ಕ len;
	u32 *dcount = p;

	ಯದಿ (perm & DISABLE_DEVICES_MASK)
		ಪ್ರತ್ಯಾಗಮ 0;

	len = 0;
	ಯದಿ (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / ಪರಿಮಾಣ(u32);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ __fixup_disable_devices(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ doff, ಪೂರ್ಣಾಙ್ಕ roff,
				   u32 raccess, ಶೂನ್ಯ *p)
{
	ಪೂರ್ಣಾಙ್ಕ i, len, coff;
	ಧ್ರುವ u32 *devices;

	ಯದಿ (raccess & DISABLE_DEVICES_MASK)
		ಪ್ರತ್ಯಾಗಮ 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	ಯದಿ (!devices)
		ಪ್ರತ್ಯಾಗಮ 0;
	len = len / ಪರಿಮಾಣ(u32);

	ಪ್ರತಿ (i = 0; i < len; i++) {
		coff = fdt_ಗಂಟು_offset_by_phandle(fdt,
					fdt32_to_cpu(devices[i]));
		ಯದಿ (coff < 0)
			ಪ್ರತ್ಯಾಗಮ coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ fdt_domain_fixup(ಶೂನ್ಯ *fdt)
{
	u32 dcount;
	ಪೂರ್ಣಾಙ್ಕ err, poffset, doffset;
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಸಂರಚನಾ __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	ಯದಿ (!dom->index)
		ಗಚ್ಛ skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ಯದಿ (doffset < 0)
		ಗಚ್ಛ skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	ಯದಿ (!dcount)
		ಗಚ್ಛ skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	ಯದಿ (err < 0)
		ಪ್ರತ್ಯಾಗಮ;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ಯದಿ (doffset < 0)
		ಗಚ್ಛ skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, ರಿಕ್ತ,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_path_offset(fdt, "/chosen");
	ಯದಿ (poffset < 0)
		ಪ್ರತ್ಯಾಗಮ;
	poffset = fdt_ಗಂಟು_offset_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ಯದಿ (poffset < 0)
		ಪ್ರತ್ಯಾಗಮ;
	fdt_nop_ಗಂಟು(fdt, poffset);
}

#define FDT_DOMAIN_REGION_MAX_ಎಣಿಕೆ		16
#define FDT_ಬೇರು_REGION_INHERIT_M_ONLY		0
#define FDT_ಬೇರು_REGION_INHERIT_ALL		1

ಸಂರಚನಾ parse_region_data {
	ಸಂರಚನಾ sbi_domain *dom;
	u32 region_ಎಣಿಕೆ;
	u32 max_regions;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ __fdt_parse_region(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ domain_offset,
			      ಪೂರ್ಣಾಙ್ಕ region_offset, u32 region_access,
			      ಶೂನ್ಯ *opaque)
{
	ಪೂರ್ಣಾಙ್ಕ len;
	u32 val32;
	u64 val64;
	ಧ್ರುವ u32 *val;
	unsigned long base, order, flags;
	ಸಂರಚನಾ parse_region_data *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	ಯದಿ (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* Find next region of the domain */
	ಯದಿ (preg->max_regions <= preg->region_ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_offset, "base", &len);
	ಯದಿ (!val || len != 8)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	val64 = fdt32_to_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	base = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_offset, "order", &len);
	ಯದಿ (!val || len != 4)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	val32 = fdt32_to_cpu(*val);
	ಯದಿ (val32 < 3 || __riscv_xlen < val32)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	order = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_MASK
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	ಯದಿ (fdt_get_property(fdt, region_offset, "mmio", ರಿಕ್ತ))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(base, (order == __riscv_xlen) ? ~0UL : BIT(order),
				  flags, &preg->dom->regions[preg->region_ಎಣಿಕೆ]);

	preg->region_ಎಣಿಕೆ++;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ __fdt_parse_domain(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ domain_offset, ಶೂನ್ಯ *opaque)
{
	u32 val32;
	u64 val64;
	ಧ್ರುವ u32 *val;
	ಧ್ರುವ ವರ್ಣ *inherit;
	ಸಂರಚನಾ sbi_domain *dom;
	ಸಂರಚನಾ sbi_hartmask *mask;
	ಸಂರಚನಾ parse_region_data preg;
	ಸಂರಚನಾ sbi_domain_memregion *reg;
	ಪೂರ್ಣಾಙ್ಕ inheritance_mode = FDT_ಬೇರು_REGION_INHERIT_M_ONLY;
	ಪೂರ್ಣಾಙ್ಕ i, err = 0, len, cpu_offset;

	dom = sbi_zalloc(ಪರಿಮಾಣ(*dom));
	ಯದಿ (!dom)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	dom->regions = sbi_calloc(ಪರಿಮಾಣ(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_ಎಣಿಕೆ + 1);
	ಯದಿ (!dom->regions) {
		err = SBI_ENOMEM;
		ಗಚ್ಛ fail_free_domain;
	}
	preg.dom = dom;
	preg.region_ಎಣಿಕೆ = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_ಎಣಿಕೆ;

	mask = sbi_zalloc(ಪರಿಮಾಣ(*mask));
	ಯದಿ (!mask) {
		err = SBI_ENOMEM;
		ಗಚ್ಛ fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_offset, ರಿಕ್ತ),
		    ಪರಿಮಾಣ(dom->name));
	dom->name[ಪರಿಮಾಣ(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_offset, "init-order", &len);
	len = len / ಪರಿಮಾಣ(u32);
	ಯದಿ (val && len)
		dom->init_order = fdt32_to_cpu(val[0]);
	ಅನ್ಯಥಾ
		dom->init_order = (u32)domain_offset;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(mask);
	dom->possible_harts = mask;
	val = fdt_getprop(fdt, domain_offset, "possible-harts", &len);
	len = len / ಪರಿಮಾಣ(u32);
	ಯದಿ (val && len) {
		ಪ್ರತಿ (i = 0; i < len; i++) {
			cpu_offset = fdt_ಗಂಟು_offset_by_phandle(fdt,
							fdt32_to_cpu(val[i]));
			ಯದಿ (cpu_offset < 0) {
				err = cpu_offset;
				ಗಚ್ಛ fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_offset, &val32);
			ಯದಿ (err)
				ಗಚ್ಛ fail_free_all;

			ಯದಿ (!fdt_ಗಂಟು_is_enabled(fdt, cpu_offset))
				ಅನುವರ್ತ;

			sbi_hartmask_set_hartid(val32, mask);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_offset, &preg,
					    __fdt_parse_region);
	ಯದಿ (err)
		ಗಚ್ಛ fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_offset,
			       "root-regions-inheritance", &len);
	ಯದಿ (inherit && len > 0) {
		ಯದಿ (!strcmp(inherit, "all"))
			inheritance_mode = FDT_ಬೇರು_REGION_INHERIT_ALL;
		ಅನ್ಯಥಾ ಯದಿ (!strcmp(inherit, "m-only"))
			inheritance_mode = FDT_ಬೇರು_REGION_INHERIT_M_ONLY;
		ಅನ್ಯಥಾ {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			ಗಚ್ಛ fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&ಬೇರು, reg) {
		bool copy = false;

		ವಿಕಲ್ಪ (inheritance_mode) {
		ಪಕ್ಷ FDT_ಬೇರು_REGION_INHERIT_ALL:
			copy = true;
			ಭಙ್ಗ;
		ಪಕ್ಷ FDT_ಬೇರು_REGION_INHERIT_M_ONLY:
			ಯದಿ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			ಭಙ್ಗ;
		}

		ಯದಿ (!copy)
			ಅನುವರ್ತ;

		ಯದಿ (preg.max_regions <= preg.region_ಎಣಿಕೆ) {
			err = SBI_EINVAL;
			ಗಚ್ಛ fail_free_all;
		}
		memcpy(&dom->regions[preg.region_ಎಣಿಕೆ++], reg, ಪರಿಮಾಣ(*reg));
	}
	dom->fw_region_inited = ಬೇರು.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_offset, "boot-hart", &len);
	ಯದಿ (val && len >= 4) {
		cpu_offset = fdt_ಗಂಟು_offset_by_phandle(fdt,
							 fdt32_to_cpu(*val));
		ಯದಿ (cpu_offset >= 0 && fdt_ಗಂಟು_is_enabled(fdt, cpu_offset))
			fdt_parse_hart_id(fdt, cpu_offset, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-arg1", &len);
	ಯದಿ (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} ಅನ್ಯಥಾ {
		val64 = ಬೇರು.next_arg1;
	}
	dom->next_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_offset, "next-addr", &len);
	ಯದಿ (val && len >= 8) {
		val64 = fdt32_to_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_to_cpu(val[1]);
	} ಅನ್ಯಥಾ {
		val64 = ಬೇರು.next_addr;
	}
	dom->next_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_offset, "next-mode", &len);
	ಯದಿ (val && len >= 4) {
		val32 = fdt32_to_cpu(val[0]);
		ಯದಿ (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} ಅನ್ಯಥಾ {
		val32 = ಬೇರು.next_mode;
	}
	dom->next_mode = val32;

	/* Read "system-reset-allowed" DT property */
	ಯದಿ (fdt_get_property(fdt, domain_offset,
			     "system-reset-allowed", ರಿಕ್ತ))
		dom->system_reset_allowed = true;
	ಅನ್ಯಥಾ
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	ಯದಿ (fdt_get_property(fdt, domain_offset,
			     "system-suspend-allowed", ರಿಕ್ತ))
		dom->system_suspend_allowed = true;
	ಅನ್ಯಥಾ
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_register(dom);
	ಯದಿ (err)
		ಗಚ್ಛ fail_free_all;

	ಪ್ರತ್ಯಾಗಮ 0;

fail_free_all:
	sbi_free(mask);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	ಪ್ರತ್ಯಾಗಮ err;
}

ಪೂರ್ಣಾಙ್ಕ fdt_domains_populate(ಧ್ರುವ ಶೂನ್ಯ *fdt)
{
	/* Sanity checks */
	ಯದಿ (!fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	ಪ್ರತ್ಯಾಗಮ fdt_iterate_each_domain_ro(fdt, ರಿಕ್ತ, __fdt_parse_domain);
}
