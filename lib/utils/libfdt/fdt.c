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
			إذا (fdt_نسخو(fdt) < FDT_پهريون_SUPPORTED_نسخو)
				أرجع -FDT_ERR_BADVERSION;
			إذا (fdt_آخري_comp_نسخو(fdt) >
					FDT_آخري_SUPPORTED_نسخو)
				أرجع -FDT_ERR_BADVERSION;
		}
	} وإلا إذا (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		إذا (!can_assume(VALID_داخلا) && fdt_سائيز_dt_struct(fdt) == 0)
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

ساكن صحيح check_بلاڪ(uint32_t hdrsize, uint32_t totalsize,
			uint32_t بنياد_2, uint32_t سائيز)
{
	إذا (!check_off_(hdrsize, totalsize, بنياد_2))
		أرجع 0; /* block start out of bounds */
	إذا ((بنياد_2 + سائيز) < بنياد_2)
		أرجع 0; /*حد_کان_مٿي*/
	إذا (!check_off_(hdrsize, totalsize, بنياد_2 + سائيز))
		أرجع 0; /* block end out of bounds */
	أرجع 1;
}

size_t fdt_سرو_سائيز(uint32_t نسخو)
{
	إذا (نسخو <= 1)
		أرجع FDT_V1_سائيز;
	وإلا إذا (نسخو <= 2)
		أرجع FDT_V2_سائيز;
	وإلا إذا (نسخو <= 3)
		أرجع FDT_V3_سائيز;
	وإلا إذا (نسخو <= 16)
		أرجع FDT_V16_سائيز;
	وإلا
		أرجع FDT_V17_سائيز;
}

size_t fdt_سرو_سائيز_2(ثابت فراغ *fdt)
{
	أرجع can_assume(LATEST) ? FDT_V17_سائيز :
		fdt_سرو_سائيز(fdt_نسخو(fdt));
}

صحيح fdt_check_سرو(ثابت فراغ *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	إذا ((uintptr_t)fdt & 7)
		أرجع -FDT_ERR_ALIGNMENT;

	إذا (fdt_magic(fdt) != FDT_MAGIC)
		أرجع -FDT_ERR_BADMAGIC;
	إذا (!can_assume(LATEST)) {
		إذا ((fdt_نسخو(fdt) < FDT_پهريون_SUPPORTED_نسخو)
		    || (fdt_آخري_comp_نسخو(fdt) >
			FDT_آخري_SUPPORTED_نسخو))
			أرجع -FDT_ERR_BADVERSION;
		إذا (fdt_نسخو(fdt) < fdt_آخري_comp_نسخو(fdt))
			أرجع -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_سرو_سائيز_2(fdt);
	إذا (!can_assume(VALID_DTB)) {

		إذا ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > وڌ_کان_وڌ_عدد))
			أرجع -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		إذا (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			أرجع -FDT_ERR_TRUNCATED;
	}

	إذا (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		إذا (!can_assume(LATEST) && fdt_نسخو(fdt) < 17) {
			إذا (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				أرجع -FDT_ERR_TRUNCATED;
		} وإلا {
			إذا (!check_بلاڪ(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_سائيز_dt_struct(fdt)))
				أرجع -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		إذا (!check_بلاڪ(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_لڙيون(fdt),
				  fdt_سائيز_dt_لڙيون(fdt)))
			أرجع -FDT_ERR_TRUNCATED;
	}

	أرجع 0;
}

ثابت فراغ *fdt_وٿي_ptr(ثابت فراغ *fdt, صحيح وٿي, بنانشان صحيح len)
{
	بنانشان صحيح uoffset = وٿي;
	بنانشان صحيح absoffset = وٿي + fdt_off_dt_struct(fdt);

	إذا (وٿي < 0)
		أرجع عدم;

	إذا (!can_assume(VALID_داخلا))
		إذا ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			أرجع عدم;

	إذا (can_assume(LATEST) || fdt_نسخو(fdt) >= 0x11)
		إذا (((uoffset + len) < uoffset)
		    || ((وٿي + len) > fdt_سائيز_dt_struct(fdt)))
			أرجع عدم;

	أرجع fdt_وٿي_ptr_2(fdt, وٿي);
}

uint32_t fdt_ايندڙ_ٽيگ(ثابت فراغ *fdt, صحيح startoffset, صحيح *nextoffset)
{
	ثابت fdt32_t *tagp, *lenp;
	uint32_t ٽيگ;
	صحيح وٿي = startoffset;
	ثابت حرف *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_وٿي_ptr(fdt, وٿي, FDT_TAGSIZE);
	إذا (!can_assume(VALID_DTB) && !tagp)
		أرجع FDT_پڇاڙي; /* premature end */
	ٽيگ = fdt32_منزل_cpu(*tagp);
	وٿي += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	بدل (ٽيگ) {
	حالة FDT_BEGIN_ڳنڍ:
		/* skip name */
		افعل {
			p = fdt_وٿي_ptr(fdt, وٿي++, 1);
		} طالما (p && (*p != '\0'));
		إذا (!can_assume(VALID_DTB) && !p)
			أرجع FDT_پڇاڙي; /* premature end */
		اكسر;

	حالة FDT_PROP:
		lenp = fdt_وٿي_ptr(fdt, وٿي, حجم(*lenp));
		إذا (!can_assume(VALID_DTB) && !lenp)
			أرجع FDT_پڇاڙي; /* premature end */
		/* skip-name offset, length and value */
		وٿي += حجم(بنية fdt_property) - FDT_TAGSIZE
			+ fdt32_منزل_cpu(*lenp);
		إذا (!can_assume(LATEST) &&
		    fdt_نسخو(fdt) < 0x10 && fdt32_منزل_cpu(*lenp) >= 8 &&
		    ((وٿي - fdt32_منزل_cpu(*lenp)) % 8) != 0)
			وٿي += 4;
		اكسر;

	حالة FDT_پڇاڙي:
	حالة FDT_پڇاڙي_ڳنڍ:
	حالة FDT_NOP:
		اكسر;

	افتراضي:
		أرجع FDT_پڇاڙي;
	}

	إذا (!fdt_وٿي_ptr(fdt, startoffset, وٿي - startoffset))
		أرجع FDT_پڇاڙي; /* premature end */

	*nextoffset = FDT_TAGALIGN(وٿي);
	أرجع ٽيگ;
}

صحيح fdt_check_ڳنڍ_وٿي(ثابت فراغ *fdt, صحيح وٿي)
{
	إذا (!can_assume(VALID_داخلا)
	    && ((وٿي < 0) || (وٿي % FDT_TAGSIZE)))
		أرجع -FDT_ERR_BADOFFSET;

	إذا (fdt_ايندڙ_ٽيگ(fdt, وٿي, &وٿي) != FDT_BEGIN_ڳنڍ)
		أرجع -FDT_ERR_BADOFFSET;

	أرجع وٿي;
}

صحيح fdt_check_prop_وٿي(ثابت فراغ *fdt, صحيح وٿي)
{
	إذا (!can_assume(VALID_داخلا)
	    && ((وٿي < 0) || (وٿي % FDT_TAGSIZE)))
		أرجع -FDT_ERR_BADOFFSET;

	إذا (fdt_ايندڙ_ٽيگ(fdt, وٿي, &وٿي) != FDT_PROP)
		أرجع -FDT_ERR_BADOFFSET;

	أرجع وٿي;
}

صحيح fdt_ايندڙ_ڳنڍ(ثابت فراغ *fdt, صحيح وٿي, صحيح *اونهائي)
{
	صحيح nextoffset = 0;
	uint32_t ٽيگ;

	إذا (وٿي >= 0)
		إذا ((nextoffset = fdt_check_ڳنڍ_وٿي(fdt, وٿي)) < 0)
			أرجع nextoffset;

	افعل {
		وٿي = nextoffset;
		ٽيگ = fdt_ايندڙ_ٽيگ(fdt, وٿي, &nextoffset);

		بدل (ٽيگ) {
		حالة FDT_PROP:
		حالة FDT_NOP:
			اكسر;

		حالة FDT_BEGIN_ڳنڍ:
			إذا (اونهائي)
				(*اونهائي)++;
			اكسر;

		حالة FDT_پڇاڙي_ڳنڍ:
			إذا (اونهائي && ((--(*اونهائي)) < 0))
				أرجع nextoffset;
			اكسر;

		حالة FDT_پڇاڙي:
			إذا ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !اونهائي))
				أرجع -FDT_ERR_NOTFOUND;
			وإلا
				أرجع nextoffset;
		}
	} طالما (ٽيگ != FDT_BEGIN_ڳنڍ);

	أرجع وٿي;
}

صحيح fdt_پهريون_subnode(ثابت فراغ *fdt, صحيح وٿي)
{
	صحيح اونهائي = 0;

	وٿي = fdt_ايندڙ_ڳنڍ(fdt, وٿي, &اونهائي);
	إذا (وٿي < 0 || اونهائي != 1)
		أرجع -FDT_ERR_NOTFOUND;

	أرجع وٿي;
}

صحيح fdt_ايندڙ_subnode(ثابت فراغ *fdt, صحيح وٿي)
{
	صحيح اونهائي = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	افعل {
		وٿي = fdt_ايندڙ_ڳنڍ(fdt, وٿي, &اونهائي);
		إذا (وٿي < 0 || اونهائي < 1)
			أرجع -FDT_ERR_NOTFOUND;
	} طالما (اونهائي > 1);

	أرجع وٿي;
}

ثابت حرف *fdt_ڳوليو_string(ثابت حرف *strtab, صحيح tabsize, ثابت حرف *s)
{
	صحيح len = لڙي_ڊيگهه(s) + 1;
	ثابت حرف *آخري = strtab + tabsize - len;
	ثابت حرف *p;

	لكل (p = strtab; p <= آخري; p++)
		إذا (memcmp(p, s, len) == 0)
			أرجع p;
	أرجع عدم;
}

صحيح fdt_move(ثابت فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	إذا (!can_assume(VALID_داخلا) && bufsize < 0)
		أرجع -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	إذا (fdt_totalsize(fdt) > (بنانشان صحيح)bufsize)
		أرجع -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	أرجع 0;
}
