// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#అన్తర్భావ "libfdt_env.h"

#అన్తర్భావ <fdt.h>
#అన్తర్భావ <libfdt.h>

#అన్తర్భావ "libfdt_internal.h"

స్థిర పూర్ణాంకం fdt_sw_probe_(శూన్య *fdt)
{
	ఒకవేళ (!can_assume(VALID_ఆదానం)) {
		ఒకవేళ (fdt_magic(fdt) == FDT_MAGIC)
			తిరిగి -FDT_ERR_BADSTATE;
		లేకపోతే ఒకవేళ (fdt_magic(fdt) != FDT_SW_MAGIC)
			తిరిగి -FDT_ERR_BADMAGIC;
	}

	తిరిగి 0;
}

#నిర్వచించు FDT_SW_PROBE(fdt) \
	{ \
		పూర్ణాంకం err; \
		ఒకవేళ ((err = fdt_sw_probe_(fdt)) != 0) \
			తిరిగి err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
స్థిర పూర్ణాంకం fdt_sw_probe_memrsv_(శూన్య *fdt)
{
	పూర్ణాంకం err = fdt_sw_probe_(fdt);
	ఒకవేళ (err)
		తిరిగి err;

	ఒకవేళ (!can_assume(VALID_ఆదానం) && fdt_off_dt_అక్షరశ్రేణులు(fdt) != 0)
		తిరిగి -FDT_ERR_BADSTATE;
	తిరిగి 0;
}

#నిర్వచించు FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		పూర్ణాంకం err; \
		ఒకవేళ ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			తిరిగి err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
స్థిర పూర్ణాంకం fdt_sw_probe_struct_(శూన్య *fdt)
{
	పూర్ణాంకం err = fdt_sw_probe_(fdt);
	ఒకవేళ (err)
		తిరిగి err;

	ఒకవేళ (!can_assume(VALID_ఆదానం) &&
	    fdt_off_dt_అక్షరశ్రేణులు(fdt) != fdt_totalsize(fdt))
		తిరిగి -FDT_ERR_BADSTATE;
	తిరిగి 0;
}

#నిర్వచించు FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		పూర్ణాంకం err; \
		ఒకవేళ ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			తిరిగి err; \
	}

స్థిర అంతఃవరుస uint32_t sw_flags(శూన్య *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	తిరిగి fdt_చివరి_comp_సంస్కరణ(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

స్థిర శూన్య *fdt_grab_space_(శూన్య *fdt, size_t len)
{
	గుర్తులేని పూర్ణాంకం స్థానభ్రంశం = fdt_కొలత_dt_struct(fdt);
	గుర్తులేని పూర్ణాంకం spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_కొలత_dt_అక్షరశ్రేణులు(fdt);

	ఒకవేళ ((స్థానభ్రంశం + len < స్థానభ్రంశం) || (స్థానభ్రంశం + len > spaceleft))
		తిరిగి ఖాళీ;

	fdt_సమితి_కొలత_dt_struct(fdt, స్థానభ్రంశం + len);
	తిరిగి fdt_స్థానభ్రంశం_ptr_w(fdt, స్థానభ్రంశం);
}

పూర్ణాంకం fdt_create_with_flags(శూన్య *buf, పూర్ణాంకం bufsize, uint32_t flags)
{
	స్థిరరాశి పూర్ణాంకం hdrsize = FDT_ALIGN(పరిమాణం(నిర్మాణం fdt_శీర్షిక),
				      పరిమాణం(నిర్మాణం fdt_reserve_నమోదు));
	శూన్య *fdt = buf;

	ఒకవేళ (bufsize < hdrsize)
		తిరిగి -FDT_ERR_NOSPACE;

	ఒకవేళ (flags & ~FDT_CREATE_FLAGS_ALL)
		తిరిగి -FDT_ERR_BADFLAGS;

	memset(buf, 0, bufsize);

	/*
	 * magic and last_comp_version keep intermediate state during the fdt
	 * creation process, which is replaced with the proper FDT format by
	 * fdt_finish().
	 *
	 * flags should be accessed with sw_flags().
	 */
	fdt_సమితి_magic(fdt, FDT_SW_MAGIC);
	fdt_సమితి_సంస్కరణ(fdt, FDT_చివరి_SUPPORTED_సంస్కరణ);
	fdt_సమితి_చివరి_comp_సంస్కరణ(fdt, flags);

	fdt_సమితి_totalsize(fdt,  bufsize);

	fdt_సమితి_off_mem_rsvmap(fdt, hdrsize);
	fdt_సమితి_off_dt_struct(fdt, fdt_off_mem_rsvmap(fdt));
	fdt_సమితి_off_dt_అక్షరశ్రేణులు(fdt, 0);

	తిరిగి 0;
}

పూర్ణాంకం fdt_create(శూన్య *buf, పూర్ణాంకం bufsize)
{
	తిరిగి fdt_create_with_flags(buf, bufsize, 0);
}

పూర్ణాంకం fdt_resize(శూన్య *fdt, శూన్య *buf, పూర్ణాంకం bufsize)
{
	size_t headsize, tailsize;
	అక్షరం *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	ఒకవేళ (bufsize < 0)
		తిరిగి -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_కొలత_dt_struct(fdt);
	tailsize = fdt_కొలత_dt_అక్షరశ్రేణులు(fdt);

	ఒకవేళ (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		తిరిగి -FDT_ERR_అంతర్గత;

	ఒకవేళ ((headsize + tailsize) > (గుర్తులేని)bufsize)
		తిరిగి -FDT_ERR_NOSPACE;

	oldtail = (అక్షరం *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (అక్షరం *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	ఒకవేళ (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} లేకపోతే {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_సమితి_totalsize(buf, bufsize);
	ఒకవేళ (fdt_off_dt_అక్షరశ్రేణులు(buf))
		fdt_సమితి_off_dt_అక్షరశ్రేణులు(buf, bufsize);

	తిరిగి 0;
}

పూర్ణాంకం fdt_కూడు_reservemap_నమోదు(శూన్య *fdt, uint64_t addr, uint64_t కొలత)
{
	నిర్మాణం fdt_reserve_నమోదు *re;
	పూర్ణాంకం స్థానభ్రంశం;

	FDT_SW_PROBE_MEMRSV(fdt);

	స్థానభ్రంశం = fdt_off_dt_struct(fdt);
	ఒకవేళ ((స్థానభ్రంశం + పరిమాణం(*re)) > fdt_totalsize(fdt))
		తిరిగి -FDT_ERR_NOSPACE;

	re = (నిర్మాణం fdt_reserve_నమోదు *)((అక్షరం *)fdt + స్థానభ్రంశం);
	re->చిరునామా = cpu_గమ్యం_fdt64(addr);
	re->కొలత = cpu_గమ్యం_fdt64(కొలత);

	fdt_సమితి_off_dt_struct(fdt, స్థానభ్రంశం + పరిమాణం(*re));

	తిరిగి 0;
}

పూర్ణాంకం fdt_ముగింపు_reservemap(శూన్య *fdt)
{
	పూర్ణాంకం err = fdt_కూడు_reservemap_నమోదు(fdt, 0, 0);

	ఒకవేళ (err)
		తిరిగి err;

	fdt_సమితి_off_dt_అక్షరశ్రేణులు(fdt, fdt_totalsize(fdt));
	తిరిగి 0;
}

పూర్ణాంకం fdt_begin_కణుపు(శూన్య *fdt, స్థిరరాశి అక్షరం *name)
{
	నిర్మాణం fdt_కణుపు_శీర్షిక *nh;
	పూర్ణాంకం namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = అక్షరశ్రేణి_పొడవు(name) + 1;
	nh = fdt_grab_space_(fdt, పరిమాణం(*nh) + FDT_TAGALIGN(namelen));
	ఒకవేళ (! nh)
		తిరిగి -FDT_ERR_NOSPACE;

	nh->ట్యాగ్ = cpu_గమ్యం_fdt32(FDT_BEGIN_కణుపు);
	జ్ఞాపకం_నకలు(nh->name, name, namelen);
	తిరిగి 0;
}

పూర్ణాంకం fdt_చివర_కణుపు(శూన్య *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	ఒకవేళ (! en)
		తిరిగి -FDT_ERR_NOSPACE;

	*en = cpu_గమ్యం_fdt32(FDT_చివర_కణుపు);
	తిరిగి 0;
}

స్థిర పూర్ణాంకం fdt_కూడు_string(శూన్య *fdt, స్థిరరాశి అక్షరం *s)
{
	అక్షరం *strtab = (అక్షరం *)fdt + fdt_totalsize(fdt);
	గుర్తులేని పూర్ణాంకం strtabsize = fdt_కొలత_dt_అక్షరశ్రేణులు(fdt);
	గుర్తులేని పూర్ణాంకం len = అక్షరశ్రేణి_పొడవు(s) + 1;
	గుర్తులేని పూర్ణాంకం struct_శిఖరం, స్థానభ్రంశం;

	స్థానభ్రంశం = strtabsize + len;
	struct_శిఖరం = fdt_off_dt_struct(fdt) + fdt_కొలత_dt_struct(fdt);
	ఒకవేళ (fdt_totalsize(fdt) - స్థానభ్రంశం < struct_శిఖరం)
		తిరిగి 0; /* no more room :( */

	జ్ఞాపకం_నకలు(strtab - స్థానభ్రంశం, s, len);
	fdt_సమితి_కొలత_dt_అక్షరశ్రేణులు(fdt, strtabsize + len);
	తిరిగి -స్థానభ్రంశం;
}

/* Must only be used to roll back in case of error */
స్థిర శూన్య fdt_del_చివరి_string(శూన్య *fdt, స్థిరరాశి అక్షరం *s)
{
	పూర్ణాంకం strtabsize = fdt_కొలత_dt_అక్షరశ్రేణులు(fdt);
	పూర్ణాంకం len = అక్షరశ్రేణి_పొడవు(s) + 1;

	fdt_సమితి_కొలత_dt_అక్షరశ్రేణులు(fdt, strtabsize - len);
}

స్థిర పూర్ణాంకం fdt_కనుగొను_కూడు_string(శూన్య *fdt, స్థిరరాశి అక్షరం *s, పూర్ణాంకం *allocated)
{
	అక్షరం *strtab = (అక్షరం *)fdt + fdt_totalsize(fdt);
	పూర్ణాంకం strtabsize = fdt_కొలత_dt_అక్షరశ్రేణులు(fdt);
	స్థిరరాశి అక్షరం *p;

	*allocated = 0;

	p = fdt_కనుగొను_string(strtab - strtabsize, strtabsize, s);
	ఒకవేళ (p)
		తిరిగి p - strtab;

	*allocated = 1;

	తిరిగి fdt_కూడు_string(fdt, s);
}

పూర్ణాంకం fdt_property_placeholder(శూన్య *fdt, స్థిరరాశి అక్షరం *name, పూర్ణాంకం len, శూన్య **valp)
{
	నిర్మాణం fdt_property *prop;
	పూర్ణాంకం nameoff;
	పూర్ణాంకం allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	ఒకవేళ (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_కూడు_string(fdt, name);
	} లేకపోతే {
		nameoff = fdt_కనుగొను_కూడు_string(fdt, name, &allocated);
	}
	ఒకవేళ (nameoff == 0)
		తిరిగి -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, పరిమాణం(*prop) + FDT_TAGALIGN(len));
	ఒకవేళ (! prop) {
		ఒకవేళ (allocated)
			fdt_del_చివరి_string(fdt, name);
		తిరిగి -FDT_ERR_NOSPACE;
	}

	prop->ట్యాగ్ = cpu_గమ్యం_fdt32(FDT_PROP);
	prop->nameoff = cpu_గమ్యం_fdt32(nameoff);
	prop->len = cpu_గమ్యం_fdt32(len);
	*valp = prop->దత్తాంశం;
	తిరిగి 0;
}

పూర్ణాంకం fdt_property(శూన్య *fdt, స్థిరరాశి అక్షరం *name, స్థిరరాశి శూన్య *val, పూర్ణాంకం len)
{
	శూన్య *ptr;
	పూర్ణాంకం ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	ఒకవేళ (ret)
		తిరిగి ret;
	జ్ఞాపకం_నకలు(ptr, val, len);
	తిరిగి 0;
}

పూర్ణాంకం fdt_ముగింపు(శూన్య *fdt)
{
	అక్షరం *p = (అక్షరం *)fdt;
	fdt32_t *చివర;
	పూర్ణాంకం oldstroffset, newstroffset;
	uint32_t ట్యాగ్;
	పూర్ణాంకం స్థానభ్రంశం, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	చివర = fdt_grab_space_(fdt, పరిమాణం(*చివర));
	ఒకవేళ (! చివర)
		తిరిగి -FDT_ERR_NOSPACE;
	*చివర = cpu_గమ్యం_fdt32(FDT_చివర);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_కొలత_dt_అక్షరశ్రేణులు(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_కొలత_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_కొలత_dt_అక్షరశ్రేణులు(fdt));
	fdt_సమితి_off_dt_అక్షరశ్రేణులు(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	స్థానభ్రంశం = 0;
	అయితే ((ట్యాగ్ = fdt_తదుపరి_ట్యాగ్(fdt, స్థానభ్రంశం, &nextoffset)) != FDT_చివర) {
		ఒకవేళ (ట్యాగ్ == FDT_PROP) {
			నిర్మాణం fdt_property *prop =
				fdt_స్థానభ్రంశం_ptr_w(fdt, స్థానభ్రంశం);
			పూర్ణాంకం nameoff;

			nameoff = fdt32_గమ్యం_cpu(prop->nameoff);
			nameoff += fdt_కొలత_dt_అక్షరశ్రేణులు(fdt);
			prop->nameoff = cpu_గమ్యం_fdt32(nameoff);
		}
		స్థానభ్రంశం = nextoffset;
	}
	ఒకవేళ (nextoffset < 0)
		తిరిగి nextoffset;

	/* Finally, adjust the header */
	fdt_సమితి_totalsize(fdt, newstroffset + fdt_కొలత_dt_అక్షరశ్రేణులు(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_సమితి_చివరి_comp_సంస్కరణ(fdt, FDT_చివరి_COMPATIBLE_సంస్కరణ);
	fdt_సమితి_magic(fdt, FDT_MAGIC);

	తిరిగి 0;
}
