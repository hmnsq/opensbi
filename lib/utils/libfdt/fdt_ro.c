// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#అన్తర్భావ "libfdt_env.h"

#అన్తర్భావ <fdt.h>
#అన్తర్భావ <libfdt.h>

#అన్తర్భావ "libfdt_internal.h"

స్థిర పూర్ణాంకం fdt_nodename_eq_(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం స్థానభ్రంశం,
			    స్థిరరాశి అక్షరం *s, పూర్ణాంకం len)
{
	పూర్ణాంకం olen;
	స్థిరరాశి అక్షరం *p = fdt_get_name(fdt, స్థానభ్రంశం, &olen);

	ఒకవేళ (!p || olen < len)
		/* short match */
		తిరిగి 0;

	ఒకవేళ (memcmp(p, s, len) != 0)
		తిరిగి 0;

	ఒకవేళ (p[len] == '\0')
		తిరిగి 1;
	లేకపోతే ఒకవేళ (!memchr(s, '@', len) && (p[len] == '@'))
		తిరిగి 1;
	లేకపోతే
		తిరిగి 0;
}

స్థిరరాశి అక్షరం *fdt_get_string(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం stroffset, పూర్ణాంకం *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	పూర్ణాంకం err;
	స్థిరరాశి అక్షరం *s, *n;

	ఒకవేళ (can_assume(VALID_ఆదానం)) {
		s = (స్థిరరాశి అక్షరం *)fdt + fdt_off_dt_అక్షరశ్రేణులు(fdt) + stroffset;

		ఒకవేళ (lenp)
			*lenp = అక్షరశ్రేణి_పొడవు(s);
		తిరిగి s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ఒకవేళ (totalsize < 0)
		వెళ్ళు fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_అక్షరశ్రేణులు(fdt);
	ఒకవేళ (absoffset >= (గుర్తులేని)totalsize)
		వెళ్ళు fail;
	len = totalsize - absoffset;

	ఒకవేళ (fdt_magic(fdt) == FDT_MAGIC) {
		ఒకవేళ (stroffset < 0)
			వెళ్ళు fail;
		ఒకవేళ (can_assume(LATEST) || fdt_సంస్కరణ(fdt) >= 17) {
			ఒకవేళ ((గుర్తులేని)stroffset >= fdt_కొలత_dt_అక్షరశ్రేణులు(fdt))
				వెళ్ళు fail;
			ఒకవేళ ((fdt_కొలత_dt_అక్షరశ్రేణులు(fdt) - stroffset) < len)
				len = fdt_కొలత_dt_అక్షరశ్రేణులు(fdt) - stroffset;
		}
	} లేకపోతే ఒకవేళ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		గుర్తులేని పూర్ణాంకం sw_stroffset = -stroffset;

		ఒకవేళ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_కొలత_dt_అక్షరశ్రేణులు(fdt)))
			వెళ్ళు fail;
		ఒకవేళ (sw_stroffset < len)
			len = sw_stroffset;
	} లేకపోతే {
		err = -FDT_ERR_అంతర్గత;
		వెళ్ళు fail;
	}

	s = (స్థిరరాశి అక్షరం *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ఒకవేళ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		వెళ్ళు fail;
	}

	ఒకవేళ (lenp)
		*lenp = n - s;
	తిరిగి s;

fail:
	ఒకవేళ (lenp)
		*lenp = err;
	తిరిగి ఖాళీ;
}

స్థిరరాశి అక్షరం *fdt_string(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం stroffset)
{
	తిరిగి fdt_get_string(fdt, stroffset, ఖాళీ);
}

స్థిర పూర్ణాంకం fdt_string_eq_(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం stroffset,
			  స్థిరరాశి అక్షరం *s, పూర్ణాంకం len)
{
	పూర్ణాంకం slen;
	స్థిరరాశి అక్షరం *p = fdt_get_string(fdt, stroffset, &slen);

	తిరిగి p && (slen == len) && (memcmp(p, s, len) == 0);
}

పూర్ణాంకం fdt_కనుగొను_max_phandle(స్థిరరాశి శూన్య *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	పూర్ణాంకం స్థానభ్రంశం = -1;

	అయితే (true) {
		uint32_t విలువ;

		స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, స్థానభ్రంశం, ఖాళీ);
		ఒకవేళ (స్థానభ్రంశం < 0) {
			ఒకవేళ (స్థానభ్రంశం == -FDT_ERR_NOTFOUND)
				ఆపు;

			తిరిగి స్థానభ్రంశం;
		}

		విలువ = fdt_get_phandle(fdt, స్థానభ్రంశం);

		ఒకవేళ (విలువ > max)
			max = విలువ;
	}

	ఒకవేళ (phandle)
		*phandle = max;

	తిరిగి 0;
}

పూర్ణాంకం fdt_generate_phandle(స్థిరరాశి శూన్య *fdt, uint32_t *phandle)
{
	uint32_t max;
	పూర్ణాంకం err;

	err = fdt_కనుగొను_max_phandle(fdt, &max);
	ఒకవేళ (err < 0)
		తిరిగి err;

	ఒకవేళ (max == FDT_MAX_PHANDLE)
		తిరిగి -FDT_ERR_NOPHANDLES;

	ఒకవేళ (phandle)
		*phandle = max + 1;

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_reserve_నమోదు *fdt_mem_rsv(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం n)
{
	గుర్తులేని పూర్ణాంకం స్థానభ్రంశం = n * పరిమాణం(నిర్మాణం fdt_reserve_నమోదు);
	గుర్తులేని పూర్ణాంకం absoffset = fdt_off_mem_rsvmap(fdt) + స్థానభ్రంశం;

	ఒకవేళ (!can_assume(VALID_ఆదానం)) {
		ఒకవేళ (absoffset < fdt_off_mem_rsvmap(fdt))
			తిరిగి ఖాళీ;
		ఒకవేళ (absoffset > fdt_totalsize(fdt) -
		    పరిమాణం(నిర్మాణం fdt_reserve_నమోదు))
			తిరిగి ఖాళీ;
	}
	తిరిగి fdt_mem_rsv_(fdt, n);
}

పూర్ణాంకం fdt_get_mem_rsv(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం n, uint64_t *చిరునామా, uint64_t *కొలత)
{
	స్థిరరాశి నిర్మాణం fdt_reserve_నమోదు *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ఒకవేళ (!can_assume(VALID_ఆదానం) && !re)
		తిరిగి -FDT_ERR_BADOFFSET;

	*చిరునామా = fdt64_ld_(&re->చిరునామా);
	*కొలత = fdt64_ld_(&re->కొలత);
	తిరిగి 0;
}

పూర్ణాంకం fdt_అంకె_mem_rsv(స్థిరరాశి శూన్య *fdt)
{
	పూర్ణాంకం i;
	స్థిరరాశి నిర్మాణం fdt_reserve_నమోదు *re;

	కొరకు (i = 0; (re = fdt_mem_rsv(fdt, i)) != ఖాళీ; i++) {
		ఒకవేళ (fdt64_ld_(&re->కొలత) == 0)
			తిరిగి i;
	}
	తిరిగి -FDT_ERR_TRUNCATED;
}

స్థిర పూర్ణాంకం nextprop_(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం స్థానభ్రంశం)
{
	uint32_t ట్యాగ్;
	పూర్ణాంకం nextoffset;

	చేయి {
		ట్యాగ్ = fdt_తదుపరి_ట్యాగ్(fdt, స్థానభ్రంశం, &nextoffset);

		ఎంపిక (ట్యాగ్) {
		స్థితి FDT_చివర:
			ఒకవేళ (nextoffset >= 0)
				తిరిగి -FDT_ERR_BADSTRUCTURE;
			లేకపోతే
				తిరిగి nextoffset;

		స్థితి FDT_PROP:
			తిరిగి స్థానభ్రంశం;
		}
		స్థానభ్రంశం = nextoffset;
	} అయితే (ట్యాగ్ == FDT_NOP);

	తిరిగి -FDT_ERR_NOTFOUND;
}

పూర్ణాంకం fdt_subnode_స్థానభ్రంశం_namelen(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం స్థానభ్రంశం,
			       స్థిరరాశి అక్షరం *name, పూర్ణాంకం namelen)
{
	పూర్ణాంకం లోతు;

	FDT_RO_PROBE(fdt);

	కొరకు (లోతు = 0;
	     (స్థానభ్రంశం >= 0) && (లోతు >= 0);
	     స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, స్థానభ్రంశం, &లోతు))
		ఒకవేళ ((లోతు == 1)
		    && fdt_nodename_eq_(fdt, స్థానభ్రంశం, name, namelen))
			తిరిగి స్థానభ్రంశం;

	ఒకవేళ (లోతు < 0)
		తిరిగి -FDT_ERR_NOTFOUND;
	తిరిగి స్థానభ్రంశం; /* error */
}

పూర్ణాంకం fdt_subnode_స్థానభ్రంశం(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం parentoffset,
		       స్థిరరాశి అక్షరం *name)
{
	తిరిగి fdt_subnode_స్థానభ్రంశం_namelen(fdt, parentoffset, name, అక్షరశ్రేణి_పొడవు(name));
}

పూర్ణాంకం fdt_మార్గం_స్థానభ్రంశం_namelen(స్థిరరాశి శూన్య *fdt, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం namelen)
{
	స్థిరరాశి అక్షరం *చివర = మార్గం + namelen;
	స్థిరరాశి అక్షరం *p = మార్గం;
	పూర్ణాంకం స్థానభ్రంశం = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ఒకవేళ (*మార్గం != '/') {
		స్థిరరాశి అక్షరం *q = memchr(మార్గం, '/', చివర - p);

		ఒకవేళ (!q)
			q = చివర;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ఒకవేళ (!p)
			తిరిగి -FDT_ERR_BADPATH;
		స్థానభ్రంశం = fdt_మార్గం_స్థానభ్రంశం(fdt, p);

		p = q;
	}

	అయితే (p < చివర) {
		స్థిరరాశి అక్షరం *q;

		అయితే (*p == '/') {
			p++;
			ఒకవేళ (p == చివర)
				తిరిగి స్థానభ్రంశం;
		}
		q = memchr(p, '/', చివర - p);
		ఒకవేళ (! q)
			q = చివర;

		స్థానభ్రంశం = fdt_subnode_స్థానభ్రంశం_namelen(fdt, స్థానభ్రంశం, p, q-p);
		ఒకవేళ (స్థానభ్రంశం < 0)
			తిరిగి స్థానభ్రంశం;

		p = q;
	}

	తిరిగి స్థానభ్రంశం;
}

పూర్ణాంకం fdt_మార్గం_స్థానభ్రంశం(స్థిరరాశి శూన్య *fdt, స్థిరరాశి అక్షరం *మార్గం)
{
	తిరిగి fdt_మార్గం_స్థానభ్రంశం_namelen(fdt, మార్గం, అక్షరశ్రేణి_పొడవు(మార్గం));
}

స్థిరరాశి అక్షరం *fdt_get_name(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset, పూర్ణాంకం *len)
{
	స్థిరరాశి నిర్మాణం fdt_కణుపు_శీర్షిక *nh = fdt_స్థానభ్రంశం_ptr_2(fdt, nodeoffset);
	స్థిరరాశి అక్షరం *nameptr;
	పూర్ణాంకం err;

	ఒకవేళ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_కణుపు_స్థానభ్రంశం(fdt, nodeoffset)) < 0))
			వెళ్ళు fail;

	nameptr = nh->name;

	ఒకవేళ (!can_assume(LATEST) && fdt_సంస్కరణ(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		స్థిరరాశి అక్షరం *ఆకు;
		ఆకు = strrchr(nameptr, '/');
		ఒకవేళ (ఆకు == ఖాళీ) {
			err = -FDT_ERR_BADSTRUCTURE;
			వెళ్ళు fail;
		}
		nameptr = ఆకు+1;
	}

	ఒకవేళ (len)
		*len = అక్షరశ్రేణి_పొడవు(nameptr);

	తిరిగి nameptr;

 fail:
	ఒకవేళ (len)
		*len = err;
	తిరిగి ఖాళీ;
}

పూర్ణాంకం fdt_మొదటి_property_స్థానభ్రంశం(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset)
{
	పూర్ణాంకం స్థానభ్రంశం;

	ఒకవేళ ((స్థానభ్రంశం = fdt_check_కణుపు_స్థానభ్రంశం(fdt, nodeoffset)) < 0)
		తిరిగి స్థానభ్రంశం;

	తిరిగి nextprop_(fdt, స్థానభ్రంశం);
}

పూర్ణాంకం fdt_తదుపరి_property_స్థానభ్రంశం(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం స్థానభ్రంశం)
{
	ఒకవేళ ((స్థానభ్రంశం = fdt_check_prop_స్థానభ్రంశం(fdt, స్థానభ్రంశం)) < 0)
		తిరిగి స్థానభ్రంశం;

	తిరిగి nextprop_(fdt, స్థానభ్రంశం);
}

స్థిర స్థిరరాశి నిర్మాణం fdt_property *fdt_get_property_by_స్థానభ్రంశం(స్థిరరాశి శూన్య *fdt,
						              పూర్ణాంకం స్థానభ్రంశం,
						              పూర్ణాంకం *lenp)
{
	పూర్ణాంకం err;
	స్థిరరాశి నిర్మాణం fdt_property *prop;

	ఒకవేళ (!can_assume(VALID_ఆదానం) &&
	    (err = fdt_check_prop_స్థానభ్రంశం(fdt, స్థానభ్రంశం)) < 0) {
		ఒకవేళ (lenp)
			*lenp = err;
		తిరిగి ఖాళీ;
	}

	prop = fdt_స్థానభ్రంశం_ptr_2(fdt, స్థానభ్రంశం);

	ఒకవేళ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	తిరిగి prop;
}

స్థిరరాశి నిర్మాణం fdt_property *fdt_get_property_by_స్థానభ్రంశం_2(స్థిరరాశి శూన్య *fdt,
						      పూర్ణాంకం స్థానభ్రంశం,
						      పూర్ణాంకం *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ఒకవేళ (!can_assume(LATEST) && fdt_సంస్కరణ(fdt) < 0x10) {
		ఒకవేళ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		తిరిగి ఖాళీ;
	}

	తిరిగి fdt_get_property_by_స్థానభ్రంశం(fdt, స్థానభ్రంశం, lenp);
}

స్థిర స్థిరరాశి నిర్మాణం fdt_property *fdt_get_property_namelen_(స్థిరరాశి శూన్య *fdt,
						            పూర్ణాంకం స్థానభ్రంశం,
						            స్థిరరాశి అక్షరం *name,
						            పూర్ణాంకం namelen,
							    పూర్ణాంకం *lenp,
							    పూర్ణాంకం *poffset)
{
	కొరకు (స్థానభ్రంశం = fdt_మొదటి_property_స్థానభ్రంశం(fdt, స్థానభ్రంశం);
	     (స్థానభ్రంశం >= 0);
	     (స్థానభ్రంశం = fdt_తదుపరి_property_స్థానభ్రంశం(fdt, స్థానభ్రంశం))) {
		స్థిరరాశి నిర్మాణం fdt_property *prop;

		prop = fdt_get_property_by_స్థానభ్రంశం(fdt, స్థానభ్రంశం, lenp);
		ఒకవేళ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			స్థానభ్రంశం = -FDT_ERR_అంతర్గత;
			ఆపు;
		}
		ఒకవేళ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ఒకవేళ (poffset)
				*poffset = స్థానభ్రంశం;
			తిరిగి prop;
		}
	}

	ఒకవేళ (lenp)
		*lenp = స్థానభ్రంశం;
	తిరిగి ఖాళీ;
}


స్థిరరాశి నిర్మాణం fdt_property *fdt_get_property_namelen(స్థిరరాశి శూన్య *fdt,
						    పూర్ణాంకం స్థానభ్రంశం,
						    స్థిరరాశి అక్షరం *name,
						    పూర్ణాంకం namelen, పూర్ణాంకం *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ఒకవేళ (!can_assume(LATEST) && fdt_సంస్కరణ(fdt) < 0x10) {
		ఒకవేళ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		తిరిగి ఖాళీ;
	}

	తిరిగి fdt_get_property_namelen_(fdt, స్థానభ్రంశం, name, namelen, lenp,
					 ఖాళీ);
}


స్థిరరాశి నిర్మాణం fdt_property *fdt_get_property(స్థిరరాశి శూన్య *fdt,
					    పూర్ణాంకం nodeoffset,
					    స్థిరరాశి అక్షరం *name, పూర్ణాంకం *lenp)
{
	తిరిగి fdt_get_property_namelen(fdt, nodeoffset, name,
					అక్షరశ్రేణి_పొడవు(name), lenp);
}

స్థిరరాశి శూన్య *fdt_getprop_namelen(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
				స్థిరరాశి అక్షరం *name, పూర్ణాంకం namelen, పూర్ణాంకం *lenp)
{
	పూర్ణాంకం poffset;
	స్థిరరాశి నిర్మాణం fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ఒకవేళ (!prop)
		తిరిగి ఖాళీ;

	/* Handle realignment */
	ఒకవేళ (!can_assume(LATEST) && fdt_సంస్కరణ(fdt) < 0x10 &&
	    (poffset + పరిమాణం(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		తిరిగి prop->దత్తాంశం + 4;
	తిరిగి prop->దత్తాంశం;
}

స్థిరరాశి శూన్య *fdt_getprop_by_స్థానభ్రంశం(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం స్థానభ్రంశం,
				  స్థిరరాశి అక్షరం **namep, పూర్ణాంకం *lenp)
{
	స్థిరరాశి నిర్మాణం fdt_property *prop;

	prop = fdt_get_property_by_స్థానభ్రంశం(fdt, స్థానభ్రంశం, lenp);
	ఒకవేళ (!prop)
		తిరిగి ఖాళీ;
	ఒకవేళ (namep) {
		స్థిరరాశి అక్షరం *name;
		పూర్ణాంకం namelen;

		ఒకవేళ (!can_assume(VALID_ఆదానం)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ఒకవేళ (!name) {
				ఒకవేళ (lenp)
					*lenp = namelen;
				తిరిగి ఖాళీ;
			}
			*namep = name;
		} లేకపోతే {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ఒకవేళ (!can_assume(LATEST) && fdt_సంస్కరణ(fdt) < 0x10 &&
	    (స్థానభ్రంశం + పరిమాణం(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		తిరిగి prop->దత్తాంశం + 4;
	తిరిగి prop->దత్తాంశం;
}

స్థిరరాశి శూన్య *fdt_getprop(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
			స్థిరరాశి అక్షరం *name, పూర్ణాంకం *lenp)
{
	తిరిగి fdt_getprop_namelen(fdt, nodeoffset, name, అక్షరశ్రేణి_పొడవు(name), lenp);
}

uint32_t fdt_get_phandle(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset)
{
	స్థిరరాశి fdt32_t *php;
	పూర్ణాంకం len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ఒకవేళ (!php || (len != పరిమాణం(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ఒకవేళ (!php || (len != పరిమాణం(*php)))
			తిరిగి 0;
	}

	తిరిగి fdt32_ld_(php);
}

స్థిరరాశి అక్షరం *fdt_get_alias_namelen(స్థిరరాశి శూన్య *fdt,
				  స్థిరరాశి అక్షరం *name, పూర్ణాంకం namelen)
{
	పూర్ణాంకం aliasoffset;

	aliasoffset = fdt_మార్గం_స్థానభ్రంశం(fdt, "/aliases");
	ఒకవేళ (aliasoffset < 0)
		తిరిగి ఖాళీ;

	తిరిగి fdt_getprop_namelen(fdt, aliasoffset, name, namelen, ఖాళీ);
}

స్థిరరాశి అక్షరం *fdt_get_alias(స్థిరరాశి శూన్య *fdt, స్థిరరాశి అక్షరం *name)
{
	తిరిగి fdt_get_alias_namelen(fdt, name, అక్షరశ్రేణి_పొడవు(name));
}

పూర్ణాంకం fdt_get_మార్గం(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset, అక్షరం *buf, పూర్ణాంకం buflen)
{
	పూర్ణాంకం pdepth = 0, p = 0;
	పూర్ణాంకం స్థానభ్రంశం, లోతు, namelen;
	స్థిరరాశి అక్షరం *name;

	FDT_RO_PROBE(fdt);

	ఒకవేళ (buflen < 2)
		తిరిగి -FDT_ERR_NOSPACE;

	కొరకు (స్థానభ్రంశం = 0, లోతు = 0;
	     (స్థానభ్రంశం >= 0) && (స్థానభ్రంశం <= nodeoffset);
	     స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, స్థానభ్రంశం, &లోతు)) {
		అయితే (pdepth > లోతు) {
			చేయి {
				p--;
			} అయితే (buf[p-1] != '/');
			pdepth--;
		}

		ఒకవేళ (pdepth >= లోతు) {
			name = fdt_get_name(fdt, స్థానభ్రంశం, &namelen);
			ఒకవేళ (!name)
				తిరిగి namelen;
			ఒకవేళ ((p + namelen + 1) <= buflen) {
				జ్ఞాపకం_నకలు(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ఒకవేళ (స్థానభ్రంశం == nodeoffset) {
			ఒకవేళ (pdepth < (లోతు + 1))
				తిరిగి -FDT_ERR_NOSPACE;

			ఒకవేళ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			తిరిగి 0;
		}
	}

	ఒకవేళ ((స్థానభ్రంశం == -FDT_ERR_NOTFOUND) || (స్థానభ్రంశం >= 0))
		తిరిగి -FDT_ERR_BADOFFSET;
	లేకపోతే ఒకవేళ (స్థానభ్రంశం == -FDT_ERR_BADOFFSET)
		తిరిగి -FDT_ERR_BADSTRUCTURE;

	తిరిగి స్థానభ్రంశం; /* error from fdt_next_node() */
}

పూర్ణాంకం fdt_supernode_atdepth_స్థానభ్రంశం(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
				 పూర్ణాంకం supernodedepth, పూర్ణాంకం *nodedepth)
{
	పూర్ణాంకం స్థానభ్రంశం, లోతు;
	పూర్ణాంకం supernodeoffset = -FDT_ERR_అంతర్గత;

	FDT_RO_PROBE(fdt);

	ఒకవేళ (supernodedepth < 0)
		తిరిగి -FDT_ERR_NOTFOUND;

	కొరకు (స్థానభ్రంశం = 0, లోతు = 0;
	     (స్థానభ్రంశం >= 0) && (స్థానభ్రంశం <= nodeoffset);
	     స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, స్థానభ్రంశం, &లోతు)) {
		ఒకవేళ (లోతు == supernodedepth)
			supernodeoffset = స్థానభ్రంశం;

		ఒకవేళ (స్థానభ్రంశం == nodeoffset) {
			ఒకవేళ (nodedepth)
				*nodedepth = లోతు;

			ఒకవేళ (supernodedepth > లోతు)
				తిరిగి -FDT_ERR_NOTFOUND;
			లేకపోతే
				తిరిగి supernodeoffset;
		}
	}

	ఒకవేళ (!can_assume(VALID_ఆదానం)) {
		ఒకవేళ ((స్థానభ్రంశం == -FDT_ERR_NOTFOUND) || (స్థానభ్రంశం >= 0))
			తిరిగి -FDT_ERR_BADOFFSET;
		లేకపోతే ఒకవేళ (స్థానభ్రంశం == -FDT_ERR_BADOFFSET)
			తిరిగి -FDT_ERR_BADSTRUCTURE;
	}

	తిరిగి స్థానభ్రంశం; /* error from fdt_next_node() */
}

పూర్ణాంకం fdt_కణుపు_లోతు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset)
{
	పూర్ణాంకం nodedepth;
	పూర్ణాంకం err;

	err = fdt_supernode_atdepth_స్థానభ్రంశం(fdt, nodeoffset, 0, &nodedepth);
	ఒకవేళ (err)
		తిరిగి (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_అంతర్గత;
	తిరిగి nodedepth;
}

పూర్ణాంకం fdt_మాతృ_స్థానభ్రంశం(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset)
{
	పూర్ణాంకం nodedepth = fdt_కణుపు_లోతు(fdt, nodeoffset);

	ఒకవేళ (nodedepth < 0)
		తిరిగి nodedepth;
	తిరిగి fdt_supernode_atdepth_స్థానభ్రంశం(fdt, nodeoffset,
					    nodedepth - 1, ఖాళీ);
}

పూర్ణాంకం fdt_కణుపు_స్థానభ్రంశం_by_prop_విలువ(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం startoffset,
				  స్థిరరాశి అక్షరం *propname,
				  స్థిరరాశి శూన్య *propval, పూర్ణాంకం proplen)
{
	పూర్ణాంకం స్థానభ్రంశం;
	స్థిరరాశి శూన్య *val;
	పూర్ణాంకం len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	కొరకు (స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, startoffset, ఖాళీ);
	     స్థానభ్రంశం >= 0;
	     స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, స్థానభ్రంశం, ఖాళీ)) {
		val = fdt_getprop(fdt, స్థానభ్రంశం, propname, &len);
		ఒకవేళ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			తిరిగి స్థానభ్రంశం;
	}

	తిరిగి స్థానభ్రంశం; /* error from fdt_next_node() */
}

పూర్ణాంకం fdt_కణుపు_స్థానభ్రంశం_by_phandle(స్థిరరాశి శూన్య *fdt, uint32_t phandle)
{
	పూర్ణాంకం స్థానభ్రంశం;

	ఒకవేళ ((phandle == 0) || (phandle == ~0U))
		తిరిగి -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	కొరకు (స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, -1, ఖాళీ);
	     స్థానభ్రంశం >= 0;
	     స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, స్థానభ్రంశం, ఖాళీ)) {
		ఒకవేళ (fdt_get_phandle(fdt, స్థానభ్రంశం) == phandle)
			తిరిగి స్థానభ్రంశం;
	}

	తిరిగి స్థానభ్రంశం; /* error from fdt_next_node() */
}

పూర్ణాంకం fdt_stringlist_కలిగియుంది(స్థిరరాశి అక్షరం *strlist, పూర్ణాంకం listlen, స్థిరరాశి అక్షరం *str)
{
	పూర్ణాంకం len = అక్షరశ్రేణి_పొడవు(str);
	స్థిరరాశి అక్షరం *p;

	అయితే (listlen >= len) {
		ఒకవేళ (memcmp(str, strlist, len+1) == 0)
			తిరిగి 1;
		p = memchr(strlist, '\0', listlen);
		ఒకవేళ (!p)
			తిరిగి 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	తిరిగి 0;
}

పూర్ణాంకం fdt_stringlist_లెక్క(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset, స్థిరరాశి అక్షరం *property)
{
	స్థిరరాశి అక్షరం *list, *చివర;
	పూర్ణాంకం నిడివి, లెక్క = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &నిడివి);
	ఒకవేళ (!list)
		తిరిగి నిడివి;

	చివర = list + నిడివి;

	అయితే (list < చివర) {
		నిడివి = strnlen(list, చివర - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ఒకవేళ (list + నిడివి > చివర)
			తిరిగి -FDT_ERR_BADVALUE;

		list += నిడివి;
		లెక్క++;
	}

	తిరిగి లెక్క;
}

పూర్ణాంకం fdt_stringlist_శోధన(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset, స్థిరరాశి అక్షరం *property,
			  స్థిరరాశి అక్షరం *string)
{
	పూర్ణాంకం నిడివి, len, idx = 0;
	స్థిరరాశి అక్షరం *list, *చివర;

	list = fdt_getprop(fdt, nodeoffset, property, &నిడివి);
	ఒకవేళ (!list)
		తిరిగి నిడివి;

	len = అక్షరశ్రేణి_పొడవు(string) + 1;
	చివర = list + నిడివి;

	అయితే (list < చివర) {
		నిడివి = strnlen(list, చివర - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ఒకవేళ (list + నిడివి > చివర)
			తిరిగి -FDT_ERR_BADVALUE;

		ఒకవేళ (నిడివి == len && memcmp(list, string, నిడివి) == 0)
			తిరిగి idx;

		list += నిడివి;
		idx++;
	}

	తిరిగి -FDT_ERR_NOTFOUND;
}

స్థిరరాశి అక్షరం *fdt_stringlist_get(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
			       స్థిరరాశి అక్షరం *property, పూర్ణాంకం idx,
			       పూర్ణాంకం *lenp)
{
	స్థిరరాశి అక్షరం *list, *చివర;
	పూర్ణాంకం నిడివి;

	list = fdt_getprop(fdt, nodeoffset, property, &నిడివి);
	ఒకవేళ (!list) {
		ఒకవేళ (lenp)
			*lenp = నిడివి;

		తిరిగి ఖాళీ;
	}

	చివర = list + నిడివి;

	అయితే (list < చివర) {
		నిడివి = strnlen(list, చివర - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ఒకవేళ (list + నిడివి > చివర) {
			ఒకవేళ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			తిరిగి ఖాళీ;
		}

		ఒకవేళ (idx == 0) {
			ఒకవేళ (lenp)
				*lenp = నిడివి - 1;

			తిరిగి list;
		}

		list += నిడివి;
		idx--;
	}

	ఒకవేళ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	తిరిగి ఖాళీ;
}

పూర్ణాంకం fdt_కణుపు_check_compatible(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
			      స్థిరరాశి అక్షరం *compatible)
{
	స్థిరరాశి శూన్య *prop;
	పూర్ణాంకం len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ఒకవేళ (!prop)
		తిరిగి len;

	తిరిగి !fdt_stringlist_కలిగియుంది(prop, len, compatible);
}

పూర్ణాంకం fdt_కణుపు_స్థానభ్రంశం_by_compatible(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం startoffset,
				  స్థిరరాశి అక్షరం *compatible)
{
	పూర్ణాంకం స్థానభ్రంశం, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	కొరకు (స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, startoffset, ఖాళీ);
	     స్థానభ్రంశం >= 0;
	     స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, స్థానభ్రంశం, ఖాళీ)) {
		err = fdt_కణుపు_check_compatible(fdt, స్థానభ్రంశం, compatible);
		ఒకవేళ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			తిరిగి err;
		లేకపోతే ఒకవేళ (err == 0)
			తిరిగి స్థానభ్రంశం;
	}

	తిరిగి స్థానభ్రంశం; /* error from fdt_next_node() */
}
