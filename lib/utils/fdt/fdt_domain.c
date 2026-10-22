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

ಪೂರ್ಣಾಂಕ fdt_iterate_each_domain(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *opaque,
			    ಪೂರ್ಣಾಂಕ (*fn)(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ domain_ಸ್ಥಾನಾಂತರ,
				       ಶೂನ್ಯ *opaque))
{
	ಪೂರ್ಣಾಂಕ rc, doffset, poffset;

	ಆದರೆ (!fdt || !fn)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	poffset = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/chosen");
	ಆದರೆ (poffset < 0)
		ಹಿಂತಿರುಗಿ 0;
	poffset = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ಆದರೆ (poffset < 0)
		ಹಿಂತಿರುಗಿ 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		ಆದರೆ (fdt_ಗಂಟು_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			ಮುಂದುವರಿ;

		rc = fn(fdt, doffset, opaque);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_iterate_each_domain_ro(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *opaque,
				      ಪೂರ್ಣಾಂಕ (*fn)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ domain_ಸ್ಥಾನಾಂತರ,
						ಶೂನ್ಯ *opaque))
{
	ಹಿಂತಿರುಗಿ fdt_iterate_each_domain((ಶೂನ್ಯ *)fdt, opaque,
				       (ಪೂರ್ಣಾಂಕ (*)(ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ,  ಶೂನ್ಯ *))fn);
}

ಪೂರ್ಣಾಂಕ fdt_iterate_each_memregion(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ domain_ಸ್ಥಾನಾಂತರ, ಶೂನ್ಯ *opaque,
			       ಪೂರ್ಣಾಂಕ (*fn)(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ domain_ಸ್ಥಾನಾಂತರ,
					 ಪೂರ್ಣಾಂಕ region_ಸ್ಥಾನಾಂತರ, u32 region_access,
					 ಶೂನ್ಯ *opaque))
{
	u32 i, rcount;
	ಪೂರ್ಣಾಂಕ rc, len, region_ಸ್ಥಾನಾಂತರ;
	ಸ್ಥಿರಾಂಕ u32 *regions;

	ಆದರೆ (!fdt || (domain_ಸ್ಥಾನಾಂತರ < 0) || !fn)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (fdt_ಗಂಟು_check_compatible(fdt, domain_ಸ್ಥಾನಾಂತರ,
				      "opensbi,domain,instance"))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_ಸ್ಥಾನಾಂತರ, "regions", &len);
	ಆದರೆ (!regions)
		ಹಿಂತಿರುಗಿ 0;

	rcount = (u32)len / (ಗಾತ್ರ(u32) * 2);
	ಗಾಗಿ (i = 0; i < rcount; i++) {
		region_ಸ್ಥಾನಾಂತರ = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt,
						fdt32_ಗಮ್ಯ_cpu(regions[2 * i]));
		ಆದರೆ (region_ಸ್ಥಾನಾಂತರ < 0)
			ಹಿಂತಿರುಗಿ region_ಸ್ಥಾನಾಂತರ;

		ಆದರೆ (fdt_ಗಂಟು_check_compatible(fdt, region_ಸ್ಥಾನಾಂತರ,
					      "opensbi,domain,memregion"))
			ಹಿಂತಿರುಗಿ SBI_EINVAL;

		rc = fn(fdt, domain_ಸ್ಥಾನಾಂತರ, region_ಸ್ಥಾನಾಂತರ,
			fdt32_ಗಮ್ಯ_cpu(regions[(2 * i) + 1]), opaque);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_iterate_each_memregion_ro(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ domain_ಸ್ಥಾನಾಂತರ, ಶೂನ್ಯ *opaque,
					 ಪೂರ್ಣಾಂಕ (*fn)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ domain_ಸ್ಥಾನಾಂತರ,
						   ಪೂರ್ಣಾಂಕ region_ಸ್ಥಾನಾಂತರ, u32 region_access,
						   ಶೂನ್ಯ *opaque))
{
	ಹಿಂತಿರುಗಿ fdt_iterate_each_memregion((ಶೂನ್ಯ *)fdt, domain_ಸ್ಥಾನಾಂತರ, opaque,
					  (ಪೂರ್ಣಾಂಕ (*)(ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, u32, ಶೂನ್ಯ *))fn);
}

ರಚನೆ __fixup_find_domain_offset_info {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
	ಪೂರ್ಣಾಂಕ *doffset;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ __fixup_find_domain_offset(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ doff, ಶೂನ್ಯ *p)
{
	ರಚನೆ __fixup_find_domain_offset_info *fdo = p;

	ಆದರೆ (!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(fdo->name, fdt_get_name(fdt, doff, ಖಾಲಿ), ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(fdo->name)))
		*fdo->doffset = doff;

	ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿಸು DISABLE_DEVICES_ಮುಖವಾಡ	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ __fixup_count_disable_devices(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ doff, ಪೂರ್ಣಾಂಕ roff,
					 u32 perm, ಶೂನ್ಯ *p)
{
	ಪೂರ್ಣಾಂಕ len;
	u32 *dcount = p;

	ಆದರೆ (perm & DISABLE_DEVICES_ಮುಖವಾಡ)
		ಹಿಂತಿರುಗಿ 0;

	len = 0;
	ಆದರೆ (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / ಗಾತ್ರ(u32);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ __fixup_disable_devices(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ doff, ಪೂರ್ಣಾಂಕ roff,
				   u32 raccess, ಶೂನ್ಯ *p)
{
	ಪೂರ್ಣಾಂಕ i, len, coff;
	ಸ್ಥಿರಾಂಕ u32 *devices;

	ಆದರೆ (raccess & DISABLE_DEVICES_ಮುಖವಾಡ)
		ಹಿಂತಿರುಗಿ 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	ಆದರೆ (!devices)
		ಹಿಂತಿರುಗಿ 0;
	len = len / ಗಾತ್ರ(u32);

	ಗಾಗಿ (i = 0; i < len; i++) {
		coff = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt,
					fdt32_ಗಮ್ಯ_cpu(devices[i]));
		ಆದರೆ (coff < 0)
			ಹಿಂತಿರುಗಿ coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ fdt_domain_fixup(ಶೂನ್ಯ *fdt)
{
	u32 dcount;
	ಪೂರ್ಣಾಂಕ err, poffset, doffset;
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();
	ರಚನೆ __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	ಆದರೆ (!dom->ಸೂಚ್ಯಂಕ_2)
		ಹೋಗು skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ಆದರೆ (doffset < 0)
		ಹೋಗು skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	ಆದರೆ (!dcount)
		ಹೋಗು skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	ಆದರೆ (err < 0)
		ಹಿಂತಿರುಗಿ;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ಆದರೆ (doffset < 0)
		ಹೋಗು skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, ಖಾಲಿ,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/chosen");
	ಆದರೆ (poffset < 0)
		ಹಿಂತಿರುಗಿ;
	poffset = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ಆದರೆ (poffset < 0)
		ಹಿಂತಿರುಗಿ;
	fdt_nop_ಗಂಟು(fdt, poffset);
}

#ವ್ಯಾಖ್ಯಾನಿಸು FDT_DOMAIN_REGION_MAX_ಎಣಿಕೆ		16
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_ಬೇರು_REGION_INHERIT_M_ONLY		0
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_ಬೇರು_REGION_INHERIT_ALL		1

ರಚನೆ parse_region_ದತ್ತಾಂಶ {
	ರಚನೆ sbi_domain *dom;
	u32 region_ಎಣಿಕೆ;
	u32 max_regions;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ __fdt_parse_region(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ domain_ಸ್ಥಾನಾಂತರ,
			      ಪೂರ್ಣಾಂಕ region_ಸ್ಥಾನಾಂತರ, u32 region_access,
			      ಶೂನ್ಯ *opaque)
{
	ಪೂರ್ಣಾಂಕ len;
	u32 val32;
	u64 val64;
	ಸ್ಥಿರಾಂಕ u32 *val;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2, ಕ್ರಮ, flags;
	ರಚನೆ parse_region_ದತ್ತಾಂಶ *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	ಆದರೆ (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_ಮುಖವಾಡ)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_ಮುಖವಾಡ))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Find next region of the domain */
	ಆದರೆ (preg->max_regions <= preg->region_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_ಸ್ಥಾನಾಂತರ, "base", &len);
	ಆದರೆ (!val || len != 8)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	val64 = fdt32_ಗಮ್ಯ_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_ಗಮ್ಯ_cpu(val[1]);
	ತಳ_2 = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_ಸ್ಥಾನಾಂತರ, "order", &len);
	ಆದರೆ (!val || len != 4)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	val32 = fdt32_ಗಮ್ಯ_cpu(*val);
	ಆದರೆ (val32 < 3 || __riscv_xlen < val32)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಕ್ರಮ = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_ಮುಖವಾಡ
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	ಆದರೆ (fdt_get_property(fdt, region_ಸ್ಥಾನಾಂತರ, "mmio", ಖಾಲಿ))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(ತಳ_2, (ಕ್ರಮ == __riscv_xlen) ? ~0UL : ಬಿಟ್(ಕ್ರಮ),
				  flags, &preg->dom->regions[preg->region_ಎಣಿಕೆ]);

	preg->region_ಎಣಿಕೆ++;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ __fdt_parse_domain(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ domain_ಸ್ಥಾನಾಂತರ, ಶೂನ್ಯ *opaque)
{
	u32 val32;
	u64 val64;
	ಸ್ಥಿರಾಂಕ u32 *val;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *inherit;
	ರಚನೆ sbi_domain *dom;
	ರಚನೆ sbi_hartmask *ಮುಖವಾಡ;
	ರಚನೆ parse_region_ದತ್ತಾಂಶ preg;
	ರಚನೆ sbi_domain_memregion *reg;
	ಪೂರ್ಣಾಂಕ inheritance_ಬಹುಲಕ = FDT_ಬೇರು_REGION_INHERIT_M_ONLY;
	ಪೂರ್ಣಾಂಕ i, err = 0, len, cpu_ಸ್ಥಾನಾಂತರ;

	dom = sbi_zalloc(ಗಾತ್ರ(*dom));
	ಆದರೆ (!dom)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	dom->regions = sbi_calloc(ಗಾತ್ರ(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_ಎಣಿಕೆ + 1);
	ಆದರೆ (!dom->regions) {
		err = SBI_ENOMEM;
		ಹೋಗು fail_free_domain;
	}
	preg.dom = dom;
	preg.region_ಎಣಿಕೆ = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_ಎಣಿಕೆ;

	ಮುಖವಾಡ = sbi_zalloc(ಗಾತ್ರ(*ಮುಖವಾಡ));
	ಆದರೆ (!ಮುಖವಾಡ) {
		err = SBI_ENOMEM;
		ಹೋಗು fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_ಸ್ಥಾನಾಂತರ, ಖಾಲಿ),
		    ಗಾತ್ರ(dom->name));
	dom->name[ಗಾತ್ರ(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_ಸ್ಥಾನಾಂತರ, "init-order", &len);
	len = len / ಗಾತ್ರ(u32);
	ಆದರೆ (val && len)
		dom->init_ಕ್ರಮ = fdt32_ಗಮ್ಯ_cpu(val[0]);
	ಇಲ್ಲದಿದ್ದರೆ
		dom->init_ಕ್ರಮ = (u32)domain_ಸ್ಥಾನಾಂತರ;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(ಮುಖವಾಡ);
	dom->possible_harts = ಮುಖವಾಡ;
	val = fdt_getprop(fdt, domain_ಸ್ಥಾನಾಂತರ, "possible-harts", &len);
	len = len / ಗಾತ್ರ(u32);
	ಆದರೆ (val && len) {
		ಗಾಗಿ (i = 0; i < len; i++) {
			cpu_ಸ್ಥಾನಾಂತರ = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt,
							fdt32_ಗಮ್ಯ_cpu(val[i]));
			ಆದರೆ (cpu_ಸ್ಥಾನಾಂತರ < 0) {
				err = cpu_ಸ್ಥಾನಾಂತರ;
				ಹೋಗು fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_ಸ್ಥಾನಾಂತರ, &val32);
			ಆದರೆ (err)
				ಹೋಗು fail_free_all;

			ಆದರೆ (!fdt_ಗಂಟು_is_enabled(fdt, cpu_ಸ್ಥಾನಾಂತರ))
				ಮುಂದುವರಿ;

			sbi_hartmask_ಗಣ_hartid(val32, ಮುಖವಾಡ);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_ಸ್ಥಾನಾಂತರ, &preg,
					    __fdt_parse_region);
	ಆದರೆ (err)
		ಹೋಗು fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_ಸ್ಥಾನಾಂತರ,
			       "root-regions-inheritance", &len);
	ಆದರೆ (inherit && len > 0) {
		ಆದರೆ (!strcmp(inherit, "all"))
			inheritance_ಬಹುಲಕ = FDT_ಬೇರು_REGION_INHERIT_ALL;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!strcmp(inherit, "m-only"))
			inheritance_ಬಹುಲಕ = FDT_ಬೇರು_REGION_INHERIT_M_ONLY;
		ಇಲ್ಲದಿದ್ದರೆ {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			ಹೋಗು fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&ಬೇರು, reg) {
		bool copy = false;

		ಆಯ್ಕೆ (inheritance_ಬಹುಲಕ) {
		ಸ್ಥಿತಿ FDT_ಬೇರು_REGION_INHERIT_ALL:
			copy = true;
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ FDT_ಬೇರು_REGION_INHERIT_M_ONLY:
			ಆದರೆ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			ನಿಲ್ಲಿಸು;
		}

		ಆದರೆ (!copy)
			ಮುಂದುವರಿ;

		ಆದರೆ (preg.max_regions <= preg.region_ಎಣಿಕೆ) {
			err = SBI_EINVAL;
			ಹೋಗು fail_free_all;
		}
		ಸ್ಮರಣೆ_ನಕಲು(&dom->regions[preg.region_ಎಣಿಕೆ++], reg, ಗಾತ್ರ(*reg));
	}
	dom->fw_region_inited = ಬೇರು.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_ಸ್ಥಾನಾಂತರ, "boot-hart", &len);
	ಆದರೆ (val && len >= 4) {
		cpu_ಸ್ಥಾನಾಂತರ = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt,
							 fdt32_ಗಮ್ಯ_cpu(*val));
		ಆದರೆ (cpu_ಸ್ಥಾನಾಂತರ >= 0 && fdt_ಗಂಟು_is_enabled(fdt, cpu_ಸ್ಥಾನಾಂತರ))
			fdt_parse_hart_id(fdt, cpu_ಸ್ಥಾನಾಂತರ, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_ಸ್ಥಾನಾಂತರ, "next-arg1", &len);
	ಆದರೆ (val && len >= 8) {
		val64 = fdt32_ಗಮ್ಯ_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_ಗಮ್ಯ_cpu(val[1]);
	} ಇಲ್ಲದಿದ್ದರೆ {
		val64 = ಬೇರು.ಮುಂದಿನ_arg1;
	}
	dom->ಮುಂದಿನ_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_ಸ್ಥಾನಾಂತರ, "next-addr", &len);
	ಆದರೆ (val && len >= 8) {
		val64 = fdt32_ಗಮ್ಯ_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_ಗಮ್ಯ_cpu(val[1]);
	} ಇಲ್ಲದಿದ್ದರೆ {
		val64 = ಬೇರು.ಮುಂದಿನ_addr;
	}
	dom->ಮುಂದಿನ_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_ಸ್ಥಾನಾಂತರ, "next-mode", &len);
	ಆದರೆ (val && len >= 4) {
		val32 = fdt32_ಗಮ್ಯ_cpu(val[0]);
		ಆದರೆ (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} ಇಲ್ಲದಿದ್ದರೆ {
		val32 = ಬೇರು.ಮುಂದಿನ_ಬಹುಲಕ;
	}
	dom->ಮುಂದಿನ_ಬಹುಲಕ = val32;

	/* Read "system-reset-allowed" DT property */
	ಆದರೆ (fdt_get_property(fdt, domain_ಸ್ಥಾನಾಂತರ,
			     "system-reset-allowed", ಖಾಲಿ))
		dom->system_reset_allowed = true;
	ಇಲ್ಲದಿದ್ದರೆ
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	ಆದರೆ (fdt_get_property(fdt, domain_ಸ್ಥಾನಾಂತರ,
			     "system-suspend-allowed", ಖಾಲಿ))
		dom->system_suspend_allowed = true;
	ಇಲ್ಲದಿದ್ದರೆ
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_ರಿಜಿಸ್ಟರ್(dom);
	ಆದರೆ (err)
		ಹೋಗು fail_free_all;

	ಹಿಂತಿರುಗಿ 0;

fail_free_all:
	sbi_free(ಮುಖವಾಡ);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	ಹಿಂತಿರುಗಿ err;
}

ಪೂರ್ಣಾಂಕ fdt_domains_populate(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt)
{
	/* Sanity checks */
	ಆದರೆ (!fdt)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	ಹಿಂತಿರುಗಿ fdt_iterate_each_domain_ro(fdt, ಖಾಲಿ, __fdt_parse_domain);
}
