// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#అన్తర్భావ "libfdt_env.h"

#అన్తర్భావ <fdt.h>
#అన్తర్భావ <libfdt.h>

#అన్తర్భావ "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(స్థిరరాశి శూన్య *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	ఒకవేళ (can_assume(VALID_DTB))
		తిరిగి totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ఒకవేళ ((uintptr_t)fdt & 7)
		తిరిగి -FDT_ERR_ALIGNMENT;

	ఒకవేళ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ఒకవేళ (!can_assume(LATEST)) {
			ఒకవేళ (fdt_సంస్కరణ(fdt) < FDT_మొదటి_SUPPORTED_సంస్కరణ)
				తిరిగి -FDT_ERR_BADVERSION;
			ఒకవేళ (fdt_చివరి_comp_సంస్కరణ(fdt) >
					FDT_చివరి_SUPPORTED_సంస్కరణ)
				తిరిగి -FDT_ERR_BADVERSION;
		}
	} లేకపోతే ఒకవేళ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ఒకవేళ (!can_assume(VALID_ఆదానం) && fdt_కొలత_dt_struct(fdt) == 0)
			తిరిగి -FDT_ERR_BADSTATE;
	} లేకపోతే {
		తిరిగి -FDT_ERR_BADMAGIC;
	}

	ఒకవేళ (totalsize < INT32_MAX)
		తిరిగి totalsize;
	లేకపోతే
		తిరిగి -FDT_ERR_TRUNCATED;
}

స్థిర పూర్ణాంకం check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	తిరిగి (off >= hdrsize) && (off <= totalsize);
}

స్థిర పూర్ణాంకం check_ఖండం(uint32_t hdrsize, uint32_t totalsize,
			uint32_t పునాది_2, uint32_t కొలత)
{
	ఒకవేళ (!check_off_(hdrsize, totalsize, పునాది_2))
		తిరిగి 0; /* block start out of bounds */
	ఒకవేళ ((పునాది_2 + కొలత) < పునాది_2)
		తిరిగి 0; /*అధికప్రవాహం*/
	ఒకవేళ (!check_off_(hdrsize, totalsize, పునాది_2 + కొలత))
		తిరిగి 0; /* block end out of bounds */
	తిరిగి 1;
}

size_t fdt_శీర్షిక_కొలత(uint32_t సంస్కరణ)
{
	ఒకవేళ (సంస్కరణ <= 1)
		తిరిగి FDT_V1_కొలత;
	లేకపోతే ఒకవేళ (సంస్కరణ <= 2)
		తిరిగి FDT_V2_కొలత;
	లేకపోతే ఒకవేళ (సంస్కరణ <= 3)
		తిరిగి FDT_V3_కొలత;
	లేకపోతే ఒకవేళ (సంస్కరణ <= 16)
		తిరిగి FDT_V16_కొలత;
	లేకపోతే
		తిరిగి FDT_V17_కొలత;
}

size_t fdt_శీర్షిక_కొలత_2(స్థిరరాశి శూన్య *fdt)
{
	తిరిగి can_assume(LATEST) ? FDT_V17_కొలత :
		fdt_శీర్షిక_కొలత(fdt_సంస్కరణ(fdt));
}

పూర్ణాంకం fdt_check_శీర్షిక(స్థిరరాశి శూన్య *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ఒకవేళ ((uintptr_t)fdt & 7)
		తిరిగి -FDT_ERR_ALIGNMENT;

	ఒకవేళ (fdt_magic(fdt) != FDT_MAGIC)
		తిరిగి -FDT_ERR_BADMAGIC;
	ఒకవేళ (!can_assume(LATEST)) {
		ఒకవేళ ((fdt_సంస్కరణ(fdt) < FDT_మొదటి_SUPPORTED_సంస్కరణ)
		    || (fdt_చివరి_comp_సంస్కరణ(fdt) >
			FDT_చివరి_SUPPORTED_సంస్కరణ))
			తిరిగి -FDT_ERR_BADVERSION;
		ఒకవేళ (fdt_సంస్కరణ(fdt) < fdt_చివరి_comp_సంస్కరణ(fdt))
			తిరిగి -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_శీర్షిక_కొలత_2(fdt);
	ఒకవేళ (!can_assume(VALID_DTB)) {

		ఒకవేళ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > గరిష్ఠ_పూర్ణాంకం))
			తిరిగి -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ఒకవేళ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			తిరిగి -FDT_ERR_TRUNCATED;
	}

	ఒకవేళ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ఒకవేళ (!can_assume(LATEST) && fdt_సంస్కరణ(fdt) < 17) {
			ఒకవేళ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				తిరిగి -FDT_ERR_TRUNCATED;
		} లేకపోతే {
			ఒకవేళ (!check_ఖండం(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_కొలత_dt_struct(fdt)))
				తిరిగి -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ఒకవేళ (!check_ఖండం(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_అక్షరశ్రేణులు(fdt),
				  fdt_కొలత_dt_అక్షరశ్రేణులు(fdt)))
			తిరిగి -FDT_ERR_TRUNCATED;
	}

	తిరిగి 0;
}

స్థిరరాశి శూన్య *fdt_స్థానభ్రంశం_ptr(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం స్థానభ్రంశం, గుర్తులేని పూర్ణాంకం len)
{
	గుర్తులేని పూర్ణాంకం uoffset = స్థానభ్రంశం;
	గుర్తులేని పూర్ణాంకం absoffset = స్థానభ్రంశం + fdt_off_dt_struct(fdt);

	ఒకవేళ (స్థానభ్రంశం < 0)
		తిరిగి ఖాళీ;

	ఒకవేళ (!can_assume(VALID_ఆదానం))
		ఒకవేళ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			తిరిగి ఖాళీ;

	ఒకవేళ (can_assume(LATEST) || fdt_సంస్కరణ(fdt) >= 0x11)
		ఒకవేళ (((uoffset + len) < uoffset)
		    || ((స్థానభ్రంశం + len) > fdt_కొలత_dt_struct(fdt)))
			తిరిగి ఖాళీ;

	తిరిగి fdt_స్థానభ్రంశం_ptr_2(fdt, స్థానభ్రంశం);
}

uint32_t fdt_తదుపరి_ట్యాగ్(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం startoffset, పూర్ణాంకం *nextoffset)
{
	స్థిరరాశి fdt32_t *tagp, *lenp;
	uint32_t ట్యాగ్;
	పూర్ణాంకం స్థానభ్రంశం = startoffset;
	స్థిరరాశి అక్షరం *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_స్థానభ్రంశం_ptr(fdt, స్థానభ్రంశం, FDT_TAGSIZE);
	ఒకవేళ (!can_assume(VALID_DTB) && !tagp)
		తిరిగి FDT_చివర; /* premature end */
	ట్యాగ్ = fdt32_గమ్యం_cpu(*tagp);
	స్థానభ్రంశం += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	ఎంపిక (ట్యాగ్) {
	స్థితి FDT_BEGIN_కణుపు:
		/* skip name */
		చేయి {
			p = fdt_స్థానభ్రంశం_ptr(fdt, స్థానభ్రంశం++, 1);
		} అయితే (p && (*p != '\0'));
		ఒకవేళ (!can_assume(VALID_DTB) && !p)
			తిరిగి FDT_చివర; /* premature end */
		ఆపు;

	స్థితి FDT_PROP:
		lenp = fdt_స్థానభ్రంశం_ptr(fdt, స్థానభ్రంశం, పరిమాణం(*lenp));
		ఒకవేళ (!can_assume(VALID_DTB) && !lenp)
			తిరిగి FDT_చివర; /* premature end */
		/* skip-name offset, length and value */
		స్థానభ్రంశం += పరిమాణం(నిర్మాణం fdt_property) - FDT_TAGSIZE
			+ fdt32_గమ్యం_cpu(*lenp);
		ఒకవేళ (!can_assume(LATEST) &&
		    fdt_సంస్కరణ(fdt) < 0x10 && fdt32_గమ్యం_cpu(*lenp) >= 8 &&
		    ((స్థానభ్రంశం - fdt32_గమ్యం_cpu(*lenp)) % 8) != 0)
			స్థానభ్రంశం += 4;
		ఆపు;

	స్థితి FDT_చివర:
	స్థితి FDT_చివర_కణుపు:
	స్థితి FDT_NOP:
		ఆపు;

	అప్రమేయ:
		తిరిగి FDT_చివర;
	}

	ఒకవేళ (!fdt_స్థానభ్రంశం_ptr(fdt, startoffset, స్థానభ్రంశం - startoffset))
		తిరిగి FDT_చివర; /* premature end */

	*nextoffset = FDT_TAGALIGN(స్థానభ్రంశం);
	తిరిగి ట్యాగ్;
}

పూర్ణాంకం fdt_check_కణుపు_స్థానభ్రంశం(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం స్థానభ్రంశం)
{
	ఒకవేళ (!can_assume(VALID_ఆదానం)
	    && ((స్థానభ్రంశం < 0) || (స్థానభ్రంశం % FDT_TAGSIZE)))
		తిరిగి -FDT_ERR_BADOFFSET;

	ఒకవేళ (fdt_తదుపరి_ట్యాగ్(fdt, స్థానభ్రంశం, &స్థానభ్రంశం) != FDT_BEGIN_కణుపు)
		తిరిగి -FDT_ERR_BADOFFSET;

	తిరిగి స్థానభ్రంశం;
}

పూర్ణాంకం fdt_check_prop_స్థానభ్రంశం(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం స్థానభ్రంశం)
{
	ఒకవేళ (!can_assume(VALID_ఆదానం)
	    && ((స్థానభ్రంశం < 0) || (స్థానభ్రంశం % FDT_TAGSIZE)))
		తిరిగి -FDT_ERR_BADOFFSET;

	ఒకవేళ (fdt_తదుపరి_ట్యాగ్(fdt, స్థానభ్రంశం, &స్థానభ్రంశం) != FDT_PROP)
		తిరిగి -FDT_ERR_BADOFFSET;

	తిరిగి స్థానభ్రంశం;
}

పూర్ణాంకం fdt_తదుపరి_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం *లోతు)
{
	పూర్ణాంకం nextoffset = 0;
	uint32_t ట్యాగ్;

	ఒకవేళ (స్థానభ్రంశం >= 0)
		ఒకవేళ ((nextoffset = fdt_check_కణుపు_స్థానభ్రంశం(fdt, స్థానభ్రంశం)) < 0)
			తిరిగి nextoffset;

	చేయి {
		స్థానభ్రంశం = nextoffset;
		ట్యాగ్ = fdt_తదుపరి_ట్యాగ్(fdt, స్థానభ్రంశం, &nextoffset);

		ఎంపిక (ట్యాగ్) {
		స్థితి FDT_PROP:
		స్థితి FDT_NOP:
			ఆపు;

		స్థితి FDT_BEGIN_కణుపు:
			ఒకవేళ (లోతు)
				(*లోతు)++;
			ఆపు;

		స్థితి FDT_చివర_కణుపు:
			ఒకవేళ (లోతు && ((--(*లోతు)) < 0))
				తిరిగి nextoffset;
			ఆపు;

		స్థితి FDT_చివర:
			ఒకవేళ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !లోతు))
				తిరిగి -FDT_ERR_NOTFOUND;
			లేకపోతే
				తిరిగి nextoffset;
		}
	} అయితే (ట్యాగ్ != FDT_BEGIN_కణుపు);

	తిరిగి స్థానభ్రంశం;
}

పూర్ణాంకం fdt_మొదటి_subnode(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం స్థానభ్రంశం)
{
	పూర్ణాంకం లోతు = 0;

	స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, స్థానభ్రంశం, &లోతు);
	ఒకవేళ (స్థానభ్రంశం < 0 || లోతు != 1)
		తిరిగి -FDT_ERR_NOTFOUND;

	తిరిగి స్థానభ్రంశం;
}

పూర్ణాంకం fdt_తదుపరి_subnode(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం స్థానభ్రంశం)
{
	పూర్ణాంకం లోతు = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	చేయి {
		స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, స్థానభ్రంశం, &లోతు);
		ఒకవేళ (స్థానభ్రంశం < 0 || లోతు < 1)
			తిరిగి -FDT_ERR_NOTFOUND;
	} అయితే (లోతు > 1);

	తిరిగి స్థానభ్రంశం;
}

స్థిరరాశి అక్షరం *fdt_కనుగొను_string(స్థిరరాశి అక్షరం *strtab, పూర్ణాంకం tabsize, స్థిరరాశి అక్షరం *s)
{
	పూర్ణాంకం len = అక్షరశ్రేణి_పొడవు(s) + 1;
	స్థిరరాశి అక్షరం *చివరి = strtab + tabsize - len;
	స్థిరరాశి అక్షరం *p;

	కొరకు (p = strtab; p <= చివరి; p++)
		ఒకవేళ (memcmp(p, s, len) == 0)
			తిరిగి p;
	తిరిగి ఖాళీ;
}

పూర్ణాంకం fdt_move(స్థిరరాశి శూన్య *fdt, శూన్య *buf, పూర్ణాంకం bufsize)
{
	ఒకవేళ (!can_assume(VALID_ఆదానం) && bufsize < 0)
		తిరిగి -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ఒకవేళ (fdt_totalsize(fdt) > (గుర్తులేని పూర్ణాంకం)bufsize)
		తిరిగి -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	తిరిగి 0;
}
