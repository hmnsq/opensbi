// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#అన్తర్భావ "libfdt_env.h"

#అన్తర్భావ <fdt.h>
#అన్తర్భావ <libfdt.h>

#అన్తర్భావ "libfdt_internal.h"

స్థిర పూర్ణాంకం fdt_ఖండాలు_misordered(స్థిరరాశి శూన్య *fdt,
				  పూర్ణాంకం mem_rsv_కొలత, పూర్ణాంకం struct_కొలత)
{
	తిరిగి (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(పరిమాణం(నిర్మాణం fdt_శీర్షిక), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_కొలత))
		|| (fdt_off_dt_అక్షరశ్రేణులు(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_కొలత))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_అక్షరశ్రేణులు(fdt) + fdt_కొలత_dt_అక్షరశ్రేణులు(fdt)));
}

స్థిర పూర్ణాంకం fdt_rw_probe_(శూన్య *fdt)
{
	ఒకవేళ (can_assume(VALID_DTB))
		తిరిగి 0;
	FDT_RO_PROBE(fdt);

	ఒకవేళ (!can_assume(LATEST) && fdt_సంస్కరణ(fdt) < 17)
		తిరిగి -FDT_ERR_BADVERSION;
	ఒకవేళ (fdt_ఖండాలు_misordered(fdt, పరిమాణం(నిర్మాణం fdt_reserve_నమోదు),
				   fdt_కొలత_dt_struct(fdt)))
		తిరిగి -FDT_ERR_BADLAYOUT;
	ఒకవేళ (!can_assume(LATEST) && fdt_సంస్కరణ(fdt) > 17)
		fdt_సమితి_సంస్కరణ(fdt, 17);

	తిరిగి 0;
}

#నిర్వచించు FDT_RW_PROBE(fdt) \
	{ \
		పూర్ణాంకం err_; \
		ఒకవేళ ((err_ = fdt_rw_probe_(fdt)) != 0) \
			తిరిగి err_; \
	}

స్థిర అంతఃవరుస గుర్తులేని పూర్ణాంకం fdt_దత్తాంశం_కొలత(శూన్య *fdt)
{
	తిరిగి fdt_off_dt_అక్షరశ్రేణులు(fdt) + fdt_కొలత_dt_అక్షరశ్రేణులు(fdt);
}

స్థిర పూర్ణాంకం fdt_splice_(శూన్య *fdt, శూన్య *splicepoint, పూర్ణాంకం oldlen, పూర్ణాంకం newlen)
{
	అక్షరం *p = splicepoint;
	గుర్తులేని పూర్ణాంకం dsize = fdt_దత్తాంశం_కొలత(fdt);
	size_t soff = p - (అక్షరం *)fdt;

	ఒకవేళ ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		తిరిగి -FDT_ERR_BADOFFSET;
	ఒకవేళ ((p < (అక్షరం *)fdt) || (dsize + newlen < (గుర్తులేని)oldlen))
		తిరిగి -FDT_ERR_BADOFFSET;
	ఒకవేళ (dsize - oldlen + newlen > fdt_totalsize(fdt))
		తిరిగి -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((అక్షరం *)fdt + dsize) - (p + oldlen));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం fdt_splice_mem_rsv_(శూన్య *fdt, నిర్మాణం fdt_reserve_నమోదు *p,
			       పూర్ణాంకం oldn, పూర్ణాంకం newn)
{
	పూర్ణాంకం delta = (newn - oldn) * పరిమాణం(*p);
	పూర్ణాంకం err;
	err = fdt_splice_(fdt, p, oldn * పరిమాణం(*p), newn * పరిమాణం(*p));
	ఒకవేళ (err)
		తిరిగి err;
	fdt_సమితి_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_సమితి_off_dt_అక్షరశ్రేణులు(fdt, fdt_off_dt_అక్షరశ్రేణులు(fdt) + delta);
	తిరిగి 0;
}

స్థిర పూర్ణాంకం fdt_splice_struct_(శూన్య *fdt, శూన్య *p,
			      పూర్ణాంకం oldlen, పూర్ణాంకం newlen)
{
	పూర్ణాంకం delta = newlen - oldlen;
	పూర్ణాంకం err;

	ఒకవేళ ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		తిరిగి err;

	fdt_సమితి_కొలత_dt_struct(fdt, fdt_కొలత_dt_struct(fdt) + delta);
	fdt_సమితి_off_dt_అక్షరశ్రేణులు(fdt, fdt_off_dt_అక్షరశ్రేణులు(fdt) + delta);
	తిరిగి 0;
}

/* Must only be used to roll back in case of error */
స్థిర శూన్య fdt_del_చివరి_string(శూన్య *fdt, స్థిరరాశి అక్షరం *s)
{
	పూర్ణాంకం newlen = అక్షరశ్రేణి_పొడవు(s) + 1;

	fdt_సమితి_కొలత_dt_అక్షరశ్రేణులు(fdt, fdt_కొలత_dt_అక్షరశ్రేణులు(fdt) - newlen);
}

స్థిర పూర్ణాంకం fdt_splice_string_(శూన్య *fdt, పూర్ణాంకం newlen)
{
	శూన్య *p = (అక్షరం *)fdt
		+ fdt_off_dt_అక్షరశ్రేణులు(fdt) + fdt_కొలత_dt_అక్షరశ్రేణులు(fdt);
	పూర్ణాంకం err;

	ఒకవేళ ((err = fdt_splice_(fdt, p, 0, newlen)))
		తిరిగి err;

	fdt_సమితి_కొలత_dt_అక్షరశ్రేణులు(fdt, fdt_కొలత_dt_అక్షరశ్రేణులు(fdt) + newlen);
	తిరిగి 0;
}

/**
 * fdt_find_add_string_() - Find or allocate a string
 *
 * @fdt: pointer to the device tree to check/adjust
 * @s: string to find/add
 * @allocated: Set to 0 if the string was found, 1 if not found and so
 *	allocated. Ignored if can_assume(NO_ROLLBACK)
 * @return offset of string in the string table (whether found or added)
 */
స్థిర పూర్ణాంకం fdt_కనుగొను_కూడు_string(శూన్య *fdt, స్థిరరాశి అక్షరం *s, పూర్ణాంకం *allocated)
{
	అక్షరం *strtab = (అక్షరం *)fdt + fdt_off_dt_అక్షరశ్రేణులు(fdt);
	స్థిరరాశి అక్షరం *p;
	అక్షరం *new;
	పూర్ణాంకం len = అక్షరశ్రేణి_పొడవు(s) + 1;
	పూర్ణాంకం err;

	ఒకవేళ (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_కనుగొను_string(strtab, fdt_కొలత_dt_అక్షరశ్రేణులు(fdt), s);
	ఒకవేళ (p)
		/* found it */
		తిరిగి (p - strtab);

	new = strtab + fdt_కొలత_dt_అక్షరశ్రేణులు(fdt);
	err = fdt_splice_string_(fdt, len);
	ఒకవేళ (err)
		తిరిగి err;

	ఒకవేళ (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	జ్ఞాపకం_నకలు(new, s, len);
	తిరిగి (new - strtab);
}

పూర్ణాంకం fdt_కూడు_mem_rsv(శూన్య *fdt, uint64_t చిరునామా, uint64_t కొలత)
{
	నిర్మాణం fdt_reserve_నమోదు *re;
	పూర్ణాంకం err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_అంకె_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	ఒకవేళ (err)
		తిరిగి err;

	re->చిరునామా = cpu_గమ్యం_fdt64(చిరునామా);
	re->కొలత = cpu_గమ్యం_fdt64(కొలత);
	తిరిగి 0;
}

పూర్ణాంకం fdt_del_mem_rsv(శూన్య *fdt, పూర్ణాంకం n)
{
	నిర్మాణం fdt_reserve_నమోదు *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	ఒకవేళ (n >= fdt_అంకె_mem_rsv(fdt))
		తిరిగి -FDT_ERR_NOTFOUND;

	తిరిగి fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

స్థిర పూర్ణాంకం fdt_resize_property_(శూన్య *fdt, పూర్ణాంకం nodeoffset, స్థిరరాశి అక్షరం *name,
				పూర్ణాంకం len, నిర్మాణం fdt_property **prop)
{
	పూర్ణాంకం oldlen;
	పూర్ణాంకం err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ఒకవేళ (!*prop)
		తిరిగి oldlen;

	ఒకవేళ ((err = fdt_splice_struct_(fdt, (*prop)->దత్తాంశం, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		తిరిగి err;

	(*prop)->len = cpu_గమ్యం_fdt32(len);
	తిరిగి 0;
}

స్థిర పూర్ణాంకం fdt_కూడు_property(శూన్య *fdt, పూర్ణాంకం nodeoffset, స్థిరరాశి అక్షరం *name,
			     పూర్ణాంకం len, నిర్మాణం fdt_property **prop)
{
	పూర్ణాంకం proplen;
	పూర్ణాంకం nextoffset;
	పూర్ణాంకం namestroff;
	పూర్ణాంకం err;
	పూర్ణాంకం allocated;

	ఒకవేళ ((nextoffset = fdt_check_కణుపు_స్థానభ్రంశం(fdt, nodeoffset)) < 0)
		తిరిగి nextoffset;

	namestroff = fdt_కనుగొను_కూడు_string(fdt, name, &allocated);
	ఒకవేళ (namestroff < 0)
		తిరిగి namestroff;

	*prop = fdt_స్థానభ్రంశం_ptr_w(fdt, nextoffset);
	proplen = పరిమాణం(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	ఒకవేళ (err) {
		/* Delete the string if we failed to add it */
		ఒకవేళ (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_చివరి_string(fdt, name);
		తిరిగి err;
	}

	(*prop)->ట్యాగ్ = cpu_గమ్యం_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_గమ్యం_fdt32(namestroff);
	(*prop)->len = cpu_గమ్యం_fdt32(len);
	తిరిగి 0;
}

పూర్ణాంకం fdt_సమితి_name(శూన్య *fdt, పూర్ణాంకం nodeoffset, స్థిరరాశి అక్షరం *name)
{
	అక్షరం *namep;
	పూర్ణాంకం oldlen, newlen;
	పూర్ణాంకం err;

	FDT_RW_PROBE(fdt);

	namep = (అక్షరం *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	ఒకవేళ (!namep)
		తిరిగి oldlen;

	newlen = అక్షరశ్రేణి_పొడవు(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	ఒకవేళ (err)
		తిరిగి err;

	జ్ఞాపకం_నకలు(namep, name, newlen+1);
	తిరిగి 0;
}

పూర్ణాంకం fdt_setprop_placeholder(శూన్య *fdt, పూర్ణాంకం nodeoffset, స్థిరరాశి అక్షరం *name,
			    పూర్ణాంకం len, శూన్య **prop_దత్తాంశం)
{
	నిర్మాణం fdt_property *prop;
	పూర్ణాంకం err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	ఒకవేళ (err == -FDT_ERR_NOTFOUND)
		err = fdt_కూడు_property(fdt, nodeoffset, name, len, &prop);
	ఒకవేళ (err)
		తిరిగి err;

	*prop_దత్తాంశం = prop->దత్తాంశం;
	తిరిగి 0;
}

పూర్ణాంకం fdt_setprop(శూన్య *fdt, పూర్ణాంకం nodeoffset, స్థిరరాశి అక్షరం *name,
		స్థిరరాశి శూన్య *val, పూర్ణాంకం len)
{
	శూన్య *prop_దత్తాంశం;
	పూర్ణాంకం err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_దత్తాంశం);
	ఒకవేళ (err)
		తిరిగి err;

	ఒకవేళ (len)
		జ్ఞాపకం_నకలు(prop_దత్తాంశం, val, len);
	తిరిగి 0;
}

పూర్ణాంకం fdt_appendprop(శూన్య *fdt, పూర్ణాంకం nodeoffset, స్థిరరాశి అక్షరం *name,
		   స్థిరరాశి శూన్య *val, పూర్ణాంకం len)
{
	నిర్మాణం fdt_property *prop;
	పూర్ణాంకం err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ఒకవేళ (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->దత్తాంశం,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		ఒకవేళ (err)
			తిరిగి err;
		prop->len = cpu_గమ్యం_fdt32(newlen);
		జ్ఞాపకం_నకలు(prop->దత్తాంశం + oldlen, val, len);
	} లేకపోతే {
		err = fdt_కూడు_property(fdt, nodeoffset, name, len, &prop);
		ఒకవేళ (err)
			తిరిగి err;
		జ్ఞాపకం_నకలు(prop->దత్తాంశం, val, len);
	}
	తిరిగి 0;
}

పూర్ణాంకం fdt_delprop(శూన్య *fdt, పూర్ణాంకం nodeoffset, స్థిరరాశి అక్షరం *name)
{
	నిర్మాణం fdt_property *prop;
	పూర్ణాంకం len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ఒకవేళ (!prop)
		తిరిగి len;

	proplen = పరిమాణం(*prop) + FDT_TAGALIGN(len);
	తిరిగి fdt_splice_struct_(fdt, prop, proplen, 0);
}

పూర్ణాంకం fdt_కూడు_subnode_namelen(శూన్య *fdt, పూర్ణాంకం parentoffset,
			    స్థిరరాశి అక్షరం *name, పూర్ణాంకం namelen)
{
	నిర్మాణం fdt_కణుపు_శీర్షిక *nh;
	పూర్ణాంకం స్థానభ్రంశం, nextoffset;
	పూర్ణాంకం nodelen;
	పూర్ణాంకం err;
	uint32_t ట్యాగ్;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	స్థానభ్రంశం = fdt_subnode_స్థానభ్రంశం_namelen(fdt, parentoffset, name, namelen);
	ఒకవేళ (స్థానభ్రంశం >= 0)
		తిరిగి -FDT_ERR_EXISTS;
	లేకపోతే ఒకవేళ (స్థానభ్రంశం != -FDT_ERR_NOTFOUND)
		తిరిగి స్థానభ్రంశం;

	/* Try to place the new node after the parent's properties */
	ట్యాగ్ = fdt_తదుపరి_ట్యాగ్(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	ఒకవేళ (!can_assume(LIBFDT_FLAWLESS) && (ట్యాగ్ != FDT_BEGIN_కణుపు))
		తిరిగి -FDT_ERR_అంతర్గత;
	చేయి {
		స్థానభ్రంశం = nextoffset;
		ట్యాగ్ = fdt_తదుపరి_ట్యాగ్(fdt, స్థానభ్రంశం, &nextoffset);
	} అయితే ((ట్యాగ్ == FDT_PROP) || (ట్యాగ్ == FDT_NOP));

	nh = fdt_స్థానభ్రంశం_ptr_w(fdt, స్థానభ్రంశం);
	nodelen = పరిమాణం(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	ఒకవేళ (err)
		తిరిగి err;

	nh->ట్యాగ్ = cpu_గమ్యం_fdt32(FDT_BEGIN_కణుపు);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	జ్ఞాపకం_నకలు(nh->name, name, namelen);
	endtag = (fdt32_t *)((అక్షరం *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_గమ్యం_fdt32(FDT_చివర_కణుపు);

	తిరిగి స్థానభ్రంశం;
}

పూర్ణాంకం fdt_కూడు_subnode(శూన్య *fdt, పూర్ణాంకం parentoffset, స్థిరరాశి అక్షరం *name)
{
	తిరిగి fdt_కూడు_subnode_namelen(fdt, parentoffset, name, అక్షరశ్రేణి_పొడవు(name));
}

పూర్ణాంకం fdt_del_కణుపు(శూన్య *fdt, పూర్ణాంకం nodeoffset)
{
	పూర్ణాంకం endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_కణుపు_చివర_స్థానభ్రంశం(fdt, nodeoffset);
	ఒకవేళ (endoffset < 0)
		తిరిగి endoffset;

	తిరిగి fdt_splice_struct_(fdt, fdt_స్థానభ్రంశం_ptr_w(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

స్థిర శూన్య fdt_packblocks_(స్థిరరాశి అక్షరం *old, అక్షరం *new,
			    పూర్ణాంకం mem_rsv_కొలత,
			    పూర్ణాంకం struct_కొలత,
			    పూర్ణాంకం అక్షరశ్రేణులు_కొలత)
{
	పూర్ణాంకం mem_rsv_off, struct_off, అక్షరశ్రేణులు_off;

	mem_rsv_off = FDT_ALIGN(పరిమాణం(నిర్మాణం fdt_శీర్షిక), 8);
	struct_off = mem_rsv_off + mem_rsv_కొలత;
	అక్షరశ్రేణులు_off = struct_off + struct_కొలత;

	memmove(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_కొలత);
	fdt_సమితి_off_mem_rsvmap(new, mem_rsv_off);

	memmove(new + struct_off, old + fdt_off_dt_struct(old), struct_కొలత);
	fdt_సమితి_off_dt_struct(new, struct_off);
	fdt_సమితి_కొలత_dt_struct(new, struct_కొలత);

	memmove(new + అక్షరశ్రేణులు_off, old + fdt_off_dt_అక్షరశ్రేణులు(old), అక్షరశ్రేణులు_కొలత);
	fdt_సమితి_off_dt_అక్షరశ్రేణులు(new, అక్షరశ్రేణులు_off);
	fdt_సమితి_కొలత_dt_అక్షరశ్రేణులు(new, fdt_కొలత_dt_అక్షరశ్రేణులు(old));
}

పూర్ణాంకం fdt_open_into(స్థిరరాశి శూన్య *fdt, శూన్య *buf, పూర్ణాంకం bufsize)
{
	పూర్ణాంకం err;
	పూర్ణాంకం mem_rsv_కొలత, struct_కొలత;
	పూర్ణాంకం newsize;
	స్థిరరాశి అక్షరం *fdtstart = fdt;
	స్థిరరాశి అక్షరం *fdtend = fdtstart + fdt_totalsize(fdt);
	అక్షరం *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_కొలత = (fdt_అంకె_mem_rsv(fdt)+1)
		* పరిమాణం(నిర్మాణం fdt_reserve_నమోదు);

	ఒకవేళ (can_assume(LATEST) || fdt_సంస్కరణ(fdt) >= 17) {
		struct_కొలత = fdt_కొలత_dt_struct(fdt);
	} లేకపోతే ఒకవేళ (fdt_సంస్కరణ(fdt) == 16) {
		struct_కొలత = 0;
		అయితే (fdt_తదుపరి_ట్యాగ్(fdt, struct_కొలత, &struct_కొలత) != FDT_చివర)
			;
		ఒకవేళ (struct_కొలత < 0)
			తిరిగి struct_కొలత;
	} లేకపోతే {
		తిరిగి -FDT_ERR_BADVERSION;
	}

	ఒకవేళ (can_assume(LIBFDT_క్రమం) ||
	    !fdt_ఖండాలు_misordered(fdt, mem_rsv_కొలత, struct_కొలత)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		ఒకవేళ (err)
			తిరిగి err;
		fdt_సమితి_సంస్కరణ(buf, 17);
		fdt_సమితి_కొలత_dt_struct(buf, struct_కొలత);
		fdt_సమితి_totalsize(buf, bufsize);
		తిరిగి 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(పరిమాణం(నిర్మాణం fdt_శీర్షిక), 8) + mem_rsv_కొలత
		+ struct_కొలత + fdt_కొలత_dt_అక్షరశ్రేణులు(fdt);

	ఒకవేళ (bufsize < newsize)
		తిరిగి -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	ఒకవేళ (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (అక్షరం *)(uintptr_t)fdtend;
		ఒకవేళ ((tmp + newsize) > ((అక్షరం *)buf + bufsize))
			తిరిగి -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_కొలత, struct_కొలత,
			fdt_కొలత_dt_అక్షరశ్రేణులు(fdt));
	memmove(buf, tmp, newsize);

	fdt_సమితి_magic(buf, FDT_MAGIC);
	fdt_సమితి_totalsize(buf, bufsize);
	fdt_సమితి_సంస్కరణ(buf, 17);
	fdt_సమితి_చివరి_comp_సంస్కరణ(buf, 16);
	fdt_సమితి_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	తిరిగి 0;
}

పూర్ణాంకం fdt_pack(శూన్య *fdt)
{
	పూర్ణాంకం mem_rsv_కొలత;

	FDT_RW_PROBE(fdt);

	mem_rsv_కొలత = (fdt_అంకె_mem_rsv(fdt)+1)
		* పరిమాణం(నిర్మాణం fdt_reserve_నమోదు);
	fdt_packblocks_(fdt, fdt, mem_rsv_కొలత, fdt_కొలత_dt_struct(fdt),
			fdt_కొలత_dt_అక్షరశ్రేణులు(fdt));
	fdt_సమితి_totalsize(fdt, fdt_దత్తాంశం_కొలత(fdt));

	తిరిగి 0;
}
