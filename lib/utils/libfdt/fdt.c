// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(ثابت فراغ *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	إذا (can_assume(VALID_DTB))
		أرجع totalsize;

	/* The device tree must be at an 8-byte aligned address */
	إذا ((uintptr_t)fdt & 7)
		أرجع -FDT_ERR_ALIGNMENT;

	إذا (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		إذا (!can_assume(LATEST)) {
			إذا (fdt_نسخه(fdt) < FDT_لومړی_SUPPORTED_نسخه)
				أرجع -FDT_ERR_BADVERSION;
			إذا (fdt_وروستی_comp_نسخه(fdt) >
					FDT_وروستی_SUPPORTED_نسخه)
				أرجع -FDT_ERR_BADVERSION;
		}
	} وإلا إذا (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		إذا (!can_assume(VALID_ننوت) && fdt_اندازه_dt_struct(fdt) == 0)
			أرجع -FDT_ERR_BADSTATE;
	} وإلا {
		أرجع -FDT_ERR_BADMAGIC;
	}

	إذا (totalsize < INT32_MAX)
		أرجع totalsize;
	وإلا
		أرجع -FDT_ERR_TRUNCATED;
}

ساكن صحيح check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	أرجع (off >= hdrsize) && (off <= totalsize);
}

ساكن صحيح check_بلاک(uint32_t hdrsize, uint32_t totalsize,
			uint32_t بنسټ_2, uint32_t اندازه)
{
	إذا (!check_off_(hdrsize, totalsize, بنسټ_2))
		أرجع 0; /* block start out of bounds */
	إذا ((بنسټ_2 + اندازه) < بنسټ_2)
		أرجع 0; /*ډکېدنه*/
	إذا (!check_off_(hdrsize, totalsize, بنسټ_2 + اندازه))
		أرجع 0; /* block end out of bounds */
	أرجع 1;
}

size_t fdt_سرلیک_اندازه(uint32_t نسخه)
{
	إذا (نسخه <= 1)
		أرجع FDT_V1_اندازه;
	وإلا إذا (نسخه <= 2)
		أرجع FDT_V2_اندازه;
	وإلا إذا (نسخه <= 3)
		أرجع FDT_V3_اندازه;
	وإلا إذا (نسخه <= 16)
		أرجع FDT_V16_اندازه;
	وإلا
		أرجع FDT_V17_اندازه;
}

size_t fdt_سرلیک_اندازه_2(ثابت فراغ *fdt)
{
	أرجع can_assume(LATEST) ? FDT_V17_اندازه :
		fdt_سرلیک_اندازه(fdt_نسخه(fdt));
}

صحيح fdt_check_سرلیک(ثابت فراغ *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	إذا ((uintptr_t)fdt & 7)
		أرجع -FDT_ERR_ALIGNMENT;

	إذا (fdt_magic(fdt) != FDT_MAGIC)
		أرجع -FDT_ERR_BADMAGIC;
	إذا (!can_assume(LATEST)) {
		إذا ((fdt_نسخه(fdt) < FDT_لومړی_SUPPORTED_نسخه)
		    || (fdt_وروستی_comp_نسخه(fdt) >
			FDT_وروستی_SUPPORTED_نسخه))
			أرجع -FDT_ERR_BADVERSION;
		إذا (fdt_نسخه(fdt) < fdt_وروستی_comp_نسخه(fdt))
			أرجع -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_سرلیک_اندازه_2(fdt);
	إذا (!can_assume(VALID_DTB)) {

		إذا ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > لوی_عدد))
			أرجع -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		إذا (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			أرجع -FDT_ERR_TRUNCATED;
	}

	إذا (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		إذا (!can_assume(LATEST) && fdt_نسخه(fdt) < 17) {
			إذا (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				أرجع -FDT_ERR_TRUNCATED;
		} وإلا {
			إذا (!check_بلاک(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_اندازه_dt_struct(fdt)))
				أرجع -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		إذا (!check_بلاک(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_مزي_ګانې(fdt),
				  fdt_اندازه_dt_مزي_ګانې(fdt)))
			أرجع -FDT_ERR_TRUNCATED;
	}

	أرجع 0;
}

ثابت فراغ *fdt_دننه_واټن_ptr(ثابت فراغ *fdt, صحيح دننه_واټن, بینښه صحيح len)
{
	بینښه صحيح uoffset = دننه_واټن;
	بینښه صحيح absoffset = دننه_واټن + fdt_off_dt_struct(fdt);

	إذا (دننه_واټن < 0)
		أرجع عدم;

	إذا (!can_assume(VALID_ننوت))
		إذا ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			أرجع عدم;

	إذا (can_assume(LATEST) || fdt_نسخه(fdt) >= 0x11)
		إذا (((uoffset + len) < uoffset)
		    || ((دننه_واټن + len) > fdt_اندازه_dt_struct(fdt)))
			أرجع عدم;

	أرجع fdt_دننه_واټن_ptr_2(fdt, دننه_واټن);
}

uint32_t fdt_بل_نښان(ثابت فراغ *fdt, صحيح startoffset, صحيح *nextoffset)
{
	ثابت fdt32_t *tagp, *lenp;
	uint32_t نښان;
	صحيح دننه_واټن = startoffset;
	ثابت حرف *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_دننه_واټن_ptr(fdt, دننه_واټن, FDT_TAGSIZE);
	إذا (!can_assume(VALID_DTB) && !tagp)
		أرجع FDT_پای_ټکی; /* premature end */
	نښان = fdt32_منزل_cpu(*tagp);
	دننه_واټن += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	بدل (نښان) {
	حالة FDT_BEGIN_غوټه:
		/* skip name */
		افعل {
			p = fdt_دننه_واټن_ptr(fdt, دننه_واټن++, 1);
		} طالما (p && (*p != '\0'));
		إذا (!can_assume(VALID_DTB) && !p)
			أرجع FDT_پای_ټکی; /* premature end */
		اكسر;

	حالة FDT_PROP:
		lenp = fdt_دننه_واټن_ptr(fdt, دننه_واټن, حجم(*lenp));
		إذا (!can_assume(VALID_DTB) && !lenp)
			أرجع FDT_پای_ټکی; /* premature end */
		/* skip-name offset, length and value */
		دننه_واټن += حجم(بنية fdt_property) - FDT_TAGSIZE
			+ fdt32_منزل_cpu(*lenp);
		إذا (!can_assume(LATEST) &&
		    fdt_نسخه(fdt) < 0x10 && fdt32_منزل_cpu(*lenp) >= 8 &&
		    ((دننه_واټن - fdt32_منزل_cpu(*lenp)) % 8) != 0)
			دننه_واټن += 4;
		اكسر;

	حالة FDT_پای_ټکی:
	حالة FDT_پای_ټکی_غوټه:
	حالة FDT_NOP:
		اكسر;

	افتراضي:
		أرجع FDT_پای_ټکی;
	}

	إذا (!fdt_دننه_واټن_ptr(fdt, startoffset, دننه_واټن - startoffset))
		أرجع FDT_پای_ټکی; /* premature end */

	*nextoffset = FDT_TAGALIGN(دننه_واټن);
	أرجع نښان;
}

صحيح fdt_check_غوټه_دننه_واټن(ثابت فراغ *fdt, صحيح دننه_واټن)
{
	إذا (!can_assume(VALID_ننوت)
	    && ((دننه_واټن < 0) || (دننه_واټن % FDT_TAGSIZE)))
		أرجع -FDT_ERR_BADOFFSET;

	إذا (fdt_بل_نښان(fdt, دننه_واټن, &دننه_واټن) != FDT_BEGIN_غوټه)
		أرجع -FDT_ERR_BADOFFSET;

	أرجع دننه_واټن;
}

صحيح fdt_check_prop_دننه_واټن(ثابت فراغ *fdt, صحيح دننه_واټن)
{
	إذا (!can_assume(VALID_ننوت)
	    && ((دننه_واټن < 0) || (دننه_واټن % FDT_TAGSIZE)))
		أرجع -FDT_ERR_BADOFFSET;

	إذا (fdt_بل_نښان(fdt, دننه_واټن, &دننه_واټن) != FDT_PROP)
		أرجع -FDT_ERR_BADOFFSET;

	أرجع دننه_واټن;
}

صحيح fdt_بل_غوټه(ثابت فراغ *fdt, صحيح دننه_واټن, صحيح *ژوروالی)
{
	صحيح nextoffset = 0;
	uint32_t نښان;

	إذا (دننه_واټن >= 0)
		إذا ((nextoffset = fdt_check_غوټه_دننه_واټن(fdt, دننه_واټن)) < 0)
			أرجع nextoffset;

	افعل {
		دننه_واټن = nextoffset;
		نښان = fdt_بل_نښان(fdt, دننه_واټن, &nextoffset);

		بدل (نښان) {
		حالة FDT_PROP:
		حالة FDT_NOP:
			اكسر;

		حالة FDT_BEGIN_غوټه:
			إذا (ژوروالی)
				(*ژوروالی)++;
			اكسر;

		حالة FDT_پای_ټکی_غوټه:
			إذا (ژوروالی && ((--(*ژوروالی)) < 0))
				أرجع nextoffset;
			اكسر;

		حالة FDT_پای_ټکی:
			إذا ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !ژوروالی))
				أرجع -FDT_ERR_NOTFOUND;
			وإلا
				أرجع nextoffset;
		}
	} طالما (نښان != FDT_BEGIN_غوټه);

	أرجع دننه_واټن;
}

صحيح fdt_لومړی_subnode(ثابت فراغ *fdt, صحيح دننه_واټن)
{
	صحيح ژوروالی = 0;

	دننه_واټن = fdt_بل_غوټه(fdt, دننه_واټن, &ژوروالی);
	إذا (دننه_واټن < 0 || ژوروالی != 1)
		أرجع -FDT_ERR_NOTFOUND;

	أرجع دننه_واټن;
}

صحيح fdt_بل_subnode(ثابت فراغ *fdt, صحيح دننه_واټن)
{
	صحيح ژوروالی = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	افعل {
		دننه_واټن = fdt_بل_غوټه(fdt, دننه_واټن, &ژوروالی);
		إذا (دننه_واټن < 0 || ژوروالی < 1)
			أرجع -FDT_ERR_NOTFOUND;
	} طالما (ژوروالی > 1);

	أرجع دننه_واټن;
}

ثابت حرف *fdt_پیدا_کړه_string(ثابت حرف *strtab, صحيح tabsize, ثابت حرف *s)
{
	صحيح len = مزي_اوږدوالی(s) + 1;
	ثابت حرف *وروستی = strtab + tabsize - len;
	ثابت حرف *p;

	لكل (p = strtab; p <= وروستی; p++)
		إذا (memcmp(p, s, len) == 0)
			أرجع p;
	أرجع عدم;
}

صحيح fdt_move(ثابت فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	إذا (!can_assume(VALID_ننوت) && bufsize < 0)
		أرجع -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	إذا (fdt_totalsize(fdt) > (بینښه صحيح)bufsize)
		أرجع -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	أرجع 0;
}
