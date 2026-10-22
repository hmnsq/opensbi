// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_domain.c - Flat Device Tree Domain helper routines
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt_env.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hartmask.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>

ਪੂਰਨ fdt_iterate_each_domain(ਖਾਲੀ *fdt, ਖਾਲੀ *opaque,
			    ਪੂਰਨ (*fn)(ਖਾਲੀ *fdt, ਪੂਰਨ domain_ਵਿੱਥ,
				       ਖਾਲੀ *opaque))
{
	ਪੂਰਨ rc, doffset, poffset;

	ਜੇ (!fdt || !fn)
		ਵਾਪਸ SBI_EINVAL;

	poffset = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/chosen");
	ਜੇ (poffset < 0)
		ਵਾਪਸ 0;
	poffset = fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ਜੇ (poffset < 0)
		ਵਾਪਸ 0;

	fdt_for_each_subnode(doffset, fdt, poffset) {
		ਜੇ (fdt_ਗੰਢ_check_compatible(fdt, doffset,
					      "opensbi,domain,instance"))
			ਜਾਰੀ;

		rc = fn(fdt, doffset, opaque);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ fdt_iterate_each_domain_ro(ਅਚਲ ਖਾਲੀ *fdt, ਖਾਲੀ *opaque,
				      ਪੂਰਨ (*fn)(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ domain_ਵਿੱਥ,
						ਖਾਲੀ *opaque))
{
	ਵਾਪਸ fdt_iterate_each_domain((ਖਾਲੀ *)fdt, opaque,
				       (ਪੂਰਨ (*)(ਖਾਲੀ *, ਪੂਰਨ,  ਖਾਲੀ *))fn);
}

ਪੂਰਨ fdt_iterate_each_memregion(ਖਾਲੀ *fdt, ਪੂਰਨ domain_ਵਿੱਥ, ਖਾਲੀ *opaque,
			       ਪੂਰਨ (*fn)(ਖਾਲੀ *fdt, ਪੂਰਨ domain_ਵਿੱਥ,
					 ਪੂਰਨ region_ਵਿੱਥ, u32 region_access,
					 ਖਾਲੀ *opaque))
{
	u32 i, rcount;
	ਪੂਰਨ rc, len, region_ਵਿੱਥ;
	ਅਚਲ u32 *regions;

	ਜੇ (!fdt || (domain_ਵਿੱਥ < 0) || !fn)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (fdt_ਗੰਢ_check_compatible(fdt, domain_ਵਿੱਥ,
				      "opensbi,domain,instance"))
		ਵਾਪਸ SBI_EINVAL;

	regions = fdt_getprop(fdt, domain_ਵਿੱਥ, "regions", &len);
	ਜੇ (!regions)
		ਵਾਪਸ 0;

	rcount = (u32)len / (ਆਕਾਰ(u32) * 2);
	ਲਈ (i = 0; i < rcount; i++) {
		region_ਵਿੱਥ = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt,
						fdt32_ਮੰਜ਼ਿਲ_cpu(regions[2 * i]));
		ਜੇ (region_ਵਿੱਥ < 0)
			ਵਾਪਸ region_ਵਿੱਥ;

		ਜੇ (fdt_ਗੰਢ_check_compatible(fdt, region_ਵਿੱਥ,
					      "opensbi,domain,memregion"))
			ਵਾਪਸ SBI_EINVAL;

		rc = fn(fdt, domain_ਵਿੱਥ, region_ਵਿੱਥ,
			fdt32_ਮੰਜ਼ਿਲ_cpu(regions[(2 * i) + 1]), opaque);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ fdt_iterate_each_memregion_ro(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ domain_ਵਿੱਥ, ਖਾਲੀ *opaque,
					 ਪੂਰਨ (*fn)(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ domain_ਵਿੱਥ,
						   ਪੂਰਨ region_ਵਿੱਥ, u32 region_access,
						   ਖਾਲੀ *opaque))
{
	ਵਾਪਸ fdt_iterate_each_memregion((ਖਾਲੀ *)fdt, domain_ਵਿੱਥ, opaque,
					  (ਪੂਰਨ (*)(ਖਾਲੀ *, ਪੂਰਨ, ਪੂਰਨ, u32, ਖਾਲੀ *))fn);
}

ਬਣਤਰ __fixup_find_domain_offset_info {
	ਅਚਲ ਅੱਖਰ *name;
	ਪੂਰਨ *doffset;
};

ਸਥਿਰ ਪੂਰਨ __fixup_find_domain_offset(ਖਾਲੀ *fdt, ਪੂਰਨ doff, ਖਾਲੀ *p)
{
	ਬਣਤਰ __fixup_find_domain_offset_info *fdo = p;

	ਜੇ (!ਲੜੀ_ਤੁਲਨਾ(fdo->name, fdt_get_name(fdt, doff, ਨਿਰਾ), ਲੜੀ_ਲੰਬਾਈ(fdo->name)))
		*fdo->doffset = doff;

	ਵਾਪਸ 0;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DISABLE_DEVICES_ਨਕਾਬ	(SBI_DOMAIN_MEMREGION_READABLE | \
				 SBI_DOMAIN_MEMREGION_WRITEABLE | \
				 SBI_DOMAIN_MEMREGION_EXECUTABLE)

ਸਥਿਰ ਪੂਰਨ __fixup_count_disable_devices(ਖਾਲੀ *fdt, ਪੂਰਨ doff, ਪੂਰਨ roff,
					 u32 perm, ਖਾਲੀ *p)
{
	ਪੂਰਨ len;
	u32 *dcount = p;

	ਜੇ (perm & DISABLE_DEVICES_ਨਕਾਬ)
		ਵਾਪਸ 0;

	len = 0;
	ਜੇ (fdt_getprop(fdt, roff, "devices", &len))
		*dcount += len / ਆਕਾਰ(u32);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ __fixup_disable_devices(ਖਾਲੀ *fdt, ਪੂਰਨ doff, ਪੂਰਨ roff,
				   u32 raccess, ਖਾਲੀ *p)
{
	ਪੂਰਨ i, len, coff;
	ਅਚਲ u32 *devices;

	ਜੇ (raccess & DISABLE_DEVICES_ਨਕਾਬ)
		ਵਾਪਸ 0;

	len = 0;
	devices = fdt_getprop(fdt, roff, "devices", &len);
	ਜੇ (!devices)
		ਵਾਪਸ 0;
	len = len / ਆਕਾਰ(u32);

	ਲਈ (i = 0; i < len; i++) {
		coff = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt,
					fdt32_ਮੰਜ਼ਿਲ_cpu(devices[i]));
		ਜੇ (coff < 0)
			ਵਾਪਸ coff;

		fdt_setprop_string(fdt, coff, "status", "disabled");
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ fdt_domain_fixup(ਖਾਲੀ *fdt)
{
	u32 dcount;
	ਪੂਰਨ err, poffset, doffset;
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();
	ਬਣਤਰ __fixup_find_domain_offset_info fdo;

	/* Skip device disable for root domain */
	ਜੇ (!dom->ਸੂਚਕਾਂਕ_2)
		ਜਾਓ skip_device_disable;

	/* Find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ਜੇ (doffset < 0)
		ਜਾਓ skip_device_disable;

	/* Count current domain device DT nodes to be disabled */
	dcount = 0;
	fdt_iterate_each_memregion(fdt, doffset, &dcount,
				   __fixup_count_disable_devices);
	ਜੇ (!dcount)
		ਜਾਓ skip_device_disable;

	/* Expand FDT based on device DT nodes to be disabled */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + dcount * 32);
	ਜੇ (err < 0)
		ਵਾਪਸ;

	/* Again find current domain DT node */
	doffset = -1;
	fdo.name = dom->name;
	fdo.doffset = &doffset;
	fdt_iterate_each_domain(fdt, &fdo, __fixup_find_domain_offset);
	ਜੇ (doffset < 0)
		ਜਾਓ skip_device_disable;

	/* Disable device DT nodes for current domain */
	fdt_iterate_each_memregion(fdt, doffset, ਨਿਰਾ,
				   __fixup_disable_devices);
skip_device_disable:

	/* Remove the OpenSBI domain config DT node */
	poffset = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/chosen");
	ਜੇ (poffset < 0)
		ਵਾਪਸ;
	poffset = fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(fdt, poffset,
						"opensbi,domain,config");
	ਜੇ (poffset < 0)
		ਵਾਪਸ;
	fdt_nop_ਗੰਢ(fdt, poffset);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_DOMAIN_REGION_MAX_ਗਿਣਤੀ		16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_ਜੜ੍ਹ_REGION_INHERIT_M_ONLY		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_ਜੜ੍ਹ_REGION_INHERIT_ALL		1

ਬਣਤਰ parse_region_ਅੰਕੜੇ {
	ਬਣਤਰ sbi_domain *dom;
	u32 region_ਗਿਣਤੀ;
	u32 max_regions;
};

ਸਥਿਰ ਪੂਰਨ __fdt_parse_region(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ domain_ਵਿੱਥ,
			      ਪੂਰਨ region_ਵਿੱਥ, u32 region_access,
			      ਖਾਲੀ *opaque)
{
	ਪੂਰਨ len;
	u32 val32;
	u64 val64;
	ਅਚਲ u32 *val;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2, ਕ੍ਰਮ, flags;
	ਬਣਤਰ parse_region_ਅੰਕੜੇ *preg = opaque;

	/*
	 * Non-root domains cannot add a region with only M-mode
	 * access permissions. M-mode regions can only be part of
	 * root domain.
	 *
	 * SU permission bits can't be all zeroes when M-mode permission
	 * bits have at least one bit set.
	 */
	ਜੇ (!(region_access & SBI_DOMAIN_MEMREGION_SU_ACCESS_ਨਕਾਬ)
	    && (region_access & SBI_DOMAIN_MEMREGION_M_ACCESS_ਨਕਾਬ))
		ਵਾਪਸ SBI_EINVAL;

	/* Find next region of the domain */
	ਜੇ (preg->max_regions <= preg->region_ਗਿਣਤੀ)
		ਵਾਪਸ SBI_ENOSPC;

	/* Read "base" DT property */
	val = fdt_getprop(fdt, region_ਵਿੱਥ, "base", &len);
	ਜੇ (!val || len != 8)
		ਵਾਪਸ SBI_EINVAL;
	val64 = fdt32_ਮੰਜ਼ਿਲ_cpu(val[0]);
	val64 = (val64 << 32) | fdt32_ਮੰਜ਼ਿਲ_cpu(val[1]);
	ਅਧਾਰ_2 = val64;

	/* Read "order" DT property */
	val = fdt_getprop(fdt, region_ਵਿੱਥ, "order", &len);
	ਜੇ (!val || len != 4)
		ਵਾਪਸ SBI_EINVAL;
	val32 = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਜੇ (val32 < 3 || __riscv_xlen < val32)
		ਵਾਪਸ SBI_EINVAL;
	ਕ੍ਰਮ = val32;

	flags = region_access & (SBI_DOMAIN_MEMREGION_ACCESS_ਨਕਾਬ
				| SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS);

	/* Read "mmio" DT property */
	ਜੇ (fdt_get_property(fdt, region_ਵਿੱਥ, "mmio", ਨਿਰਾ))
		flags |= SBI_DOMAIN_MEMREGION_MMIO;

	sbi_domain_memregion_init(ਅਧਾਰ_2, (ਕ੍ਰਮ == __riscv_xlen) ? ~0UL : ਦੋਆਧਾਰੀ_ਅੰਕ(ਕ੍ਰਮ),
				  flags, &preg->dom->regions[preg->region_ਗਿਣਤੀ]);

	preg->region_ਗਿਣਤੀ++;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ __fdt_parse_domain(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ domain_ਵਿੱਥ, ਖਾਲੀ *opaque)
{
	u32 val32;
	u64 val64;
	ਅਚਲ u32 *val;
	ਅਚਲ ਅੱਖਰ *inherit;
	ਬਣਤਰ sbi_domain *dom;
	ਬਣਤਰ sbi_hartmask *ਨਕਾਬ;
	ਬਣਤਰ parse_region_ਅੰਕੜੇ preg;
	ਬਣਤਰ sbi_domain_memregion *reg;
	ਪੂਰਨ inheritance_ਬਹੁਲਕ = FDT_ਜੜ੍ਹ_REGION_INHERIT_M_ONLY;
	ਪੂਰਨ i, err = 0, len, cpu_ਵਿੱਥ;

	dom = sbi_zalloc(ਆਕਾਰ(*dom));
	ਜੇ (!dom)
		ਵਾਪਸ SBI_ENOMEM;

	dom->regions = sbi_calloc(ਆਕਾਰ(*dom->regions),
				  FDT_DOMAIN_REGION_MAX_ਗਿਣਤੀ + 1);
	ਜੇ (!dom->regions) {
		err = SBI_ENOMEM;
		ਜਾਓ fail_free_domain;
	}
	preg.dom = dom;
	preg.region_ਗਿਣਤੀ = 0;
	preg.max_regions = FDT_DOMAIN_REGION_MAX_ਗਿਣਤੀ;

	ਨਕਾਬ = sbi_zalloc(ਆਕਾਰ(*ਨਕਾਬ));
	ਜੇ (!ਨਕਾਬ) {
		err = SBI_ENOMEM;
		ਜਾਓ fail_free_regions;
	}

	/* Read DT node name */
	strncpy(dom->name, fdt_get_name(fdt, domain_ਵਿੱਥ, ਨਿਰਾ),
		    ਆਕਾਰ(dom->name));
	dom->name[ਆਕਾਰ(dom->name) - 1] = '\0';

	/* Read initalization order */
	val = fdt_getprop(fdt, domain_ਵਿੱਥ, "init-order", &len);
	len = len / ਆਕਾਰ(u32);
	ਜੇ (val && len)
		dom->init_ਕ੍ਰਮ = fdt32_ਮੰਜ਼ਿਲ_cpu(val[0]);
	ਨਹੀਂਤਾਂ
		dom->init_ਕ੍ਰਮ = (u32)domain_ਵਿੱਥ;

	/* Setup possible HARTs mask */
	SBI_HARTMASK_INIT(ਨਕਾਬ);
	dom->possible_harts = ਨਕਾਬ;
	val = fdt_getprop(fdt, domain_ਵਿੱਥ, "possible-harts", &len);
	len = len / ਆਕਾਰ(u32);
	ਜੇ (val && len) {
		ਲਈ (i = 0; i < len; i++) {
			cpu_ਵਿੱਥ = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt,
							fdt32_ਮੰਜ਼ਿਲ_cpu(val[i]));
			ਜੇ (cpu_ਵਿੱਥ < 0) {
				err = cpu_ਵਿੱਥ;
				ਜਾਓ fail_free_all;
			}

			err = fdt_parse_hart_id(fdt, cpu_ਵਿੱਥ, &val32);
			ਜੇ (err)
				ਜਾਓ fail_free_all;

			ਜੇ (!fdt_ਗੰਢ_is_enabled(fdt, cpu_ਵਿੱਥ))
				ਜਾਰੀ;

			sbi_hartmask_ਸੈੱਟ_hartid(val32, ਨਕਾਬ);
		}
	}

	/* Setup memregions from DT */
	err = fdt_iterate_each_memregion_ro(fdt, domain_ਵਿੱਥ, &preg,
					    __fdt_parse_region);
	ਜੇ (err)
		ਜਾਓ fail_free_all;

	/* Determine root domain regions inheritance behavior. */
	inherit = fdt_getprop(fdt, domain_ਵਿੱਥ,
			       "root-regions-inheritance", &len);
	ਜੇ (inherit && len > 0) {
		ਜੇ (!strcmp(inherit, "all"))
			inheritance_ਬਹੁਲਕ = FDT_ਜੜ੍ਹ_REGION_INHERIT_ALL;
		ਨਹੀਂਤਾਂ ਜੇ (!strcmp(inherit, "m-only"))
			inheritance_ਬਹੁਲਕ = FDT_ਜੜ੍ਹ_REGION_INHERIT_M_ONLY;
		ਨਹੀਂਤਾਂ {
			sbi_printf("%s: domain \"%s\" has unsupported "
				   "root-regions-inheritance=\"%s\"\n",
				   __func__, dom->name, inherit);
			err = SBI_EINVAL;
			ਜਾਓ fail_free_all;
		}
	}

	/* Copy over root domain memregions according to inheritance_mode. */
	sbi_domain_for_each_memregion(&ਜੜ੍ਹ, reg) {
		bool copy = false;

		ਚੋਣ (inheritance_ਬਹੁਲਕ) {
		ਹਾਲਤ FDT_ਜੜ੍ਹ_REGION_INHERIT_ALL:
			copy = true;
			ਰੋਕੋ;
		ਹਾਲਤ FDT_ਜੜ੍ਹ_REGION_INHERIT_M_ONLY:
			ਜੇ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags) ||
			    SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
				copy = true;
			}
			ਰੋਕੋ;
		}

		ਜੇ (!copy)
			ਜਾਰੀ;

		ਜੇ (preg.max_regions <= preg.region_ਗਿਣਤੀ) {
			err = SBI_EINVAL;
			ਜਾਓ fail_free_all;
		}
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(&dom->regions[preg.region_ਗਿਣਤੀ++], reg, ਆਕਾਰ(*reg));
	}
	dom->fw_region_inited = ਜੜ੍ਹ.fw_region_inited;

	/* Read "boot-hart" DT property */
	val32 = current_hartid();
	val = fdt_getprop(fdt, domain_ਵਿੱਥ, "boot-hart", &len);
	ਜੇ (val && len >= 4) {
		cpu_ਵਿੱਥ = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt,
							 fdt32_ਮੰਜ਼ਿਲ_cpu(*val));
		ਜੇ (cpu_ਵਿੱਥ >= 0 && fdt_ਗੰਢ_is_enabled(fdt, cpu_ਵਿੱਥ))
			fdt_parse_hart_id(fdt, cpu_ਵਿੱਥ, &val32);
	}
	dom->boot_hartid = val32;

	/* Read "next-arg1" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_ਵਿੱਥ, "next-arg1", &len);
	ਜੇ (val && len >= 8) {
		val64 = fdt32_ਮੰਜ਼ਿਲ_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_ਮੰਜ਼ਿਲ_cpu(val[1]);
	} ਨਹੀਂਤਾਂ {
		val64 = ਜੜ੍ਹ.ਅਗਲਾ_arg1;
	}
	dom->ਅਗਲਾ_arg1 = val64;

	/* Read "next-addr" DT property */
	val64 = 0;
	val = fdt_getprop(fdt, domain_ਵਿੱਥ, "next-addr", &len);
	ਜੇ (val && len >= 8) {
		val64 = fdt32_ਮੰਜ਼ਿਲ_cpu(val[0]);
		val64 = (val64 << 32) | fdt32_ਮੰਜ਼ਿਲ_cpu(val[1]);
	} ਨਹੀਂਤਾਂ {
		val64 = ਜੜ੍ਹ.ਅਗਲਾ_addr;
	}
	dom->ਅਗਲਾ_addr = val64;

	/* Read "next-mode" DT property */
	val32 = 0x1;
	val = fdt_getprop(fdt, domain_ਵਿੱਥ, "next-mode", &len);
	ਜੇ (val && len >= 4) {
		val32 = fdt32_ਮੰਜ਼ਿਲ_cpu(val[0]);
		ਜੇ (val32 != 0x0 && val32 != 0x1)
			val32 = 0x1;
	} ਨਹੀਂਤਾਂ {
		val32 = ਜੜ੍ਹ.ਅਗਲਾ_ਬਹੁਲਕ;
	}
	dom->ਅਗਲਾ_ਬਹੁਲਕ = val32;

	/* Read "system-reset-allowed" DT property */
	ਜੇ (fdt_get_property(fdt, domain_ਵਿੱਥ,
			     "system-reset-allowed", ਨਿਰਾ))
		dom->system_reset_allowed = true;
	ਨਹੀਂਤਾਂ
		dom->system_reset_allowed = false;

	/* Read "system-suspend-allowed" DT property */
	ਜੇ (fdt_get_property(fdt, domain_ਵਿੱਥ,
			     "system-suspend-allowed", ਨਿਰਾ))
		dom->system_suspend_allowed = true;
	ਨਹੀਂਤਾਂ
		dom->system_suspend_allowed = false;

	/* Register the domain */
	err = sbi_domain_ਰਜਿਸਟਰ(dom);
	ਜੇ (err)
		ਜਾਓ fail_free_all;

	ਵਾਪਸ 0;

fail_free_all:
	sbi_free(ਨਕਾਬ);
fail_free_regions:
	sbi_free(dom->regions);
fail_free_domain:
	sbi_free(dom);
	ਵਾਪਸ err;
}

ਪੂਰਨ fdt_domains_populate(ਅਚਲ ਖਾਲੀ *fdt)
{
	/* Sanity checks */
	ਜੇ (!fdt)
		ਵਾਪਸ SBI_EINVAL;

	/* Iterate over each domain in FDT and populate details */
	ਵਾਪਸ fdt_iterate_each_domain_ro(fdt, ਨਿਰਾ, __fdt_parse_domain);
}
