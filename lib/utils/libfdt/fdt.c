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
			إذا (fdt_نسخہ(fdt) < FDT_پہلا_SUPPORTED_نسخہ)
				أرجع -FDT_ERR_BADVERSION;
			إذا (fdt_آخری_comp_نسخہ(fdt) >
					FDT_آخری_SUPPORTED_نسخہ)
				أرجع -FDT_ERR_BADVERSION;
		}
	} وإلا إذا (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		إذا (!can_assume(VALID_داخلہ) && fdt_ناپ_dt_struct(fdt) == 0)
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
			uint32_t ادھار_2, uint32_t ناپ)
{
	إذا (!check_off_(hdrsize, totalsize, ادھار_2))
		أرجع 0; /* block start out of bounds */
	إذا ((ادھار_2 + ناپ) < ادھار_2)
		أرجع 0; /*حد_توں_ودھ*/
	إذا (!check_off_(hdrsize, totalsize, ادھار_2 + ناپ))
		أرجع 0; /* block end out of bounds */
	أرجع 1;
}

size_t fdt_سرخی_ناپ(uint32_t نسخہ)
{
	إذا (نسخہ <= 1)
		أرجع FDT_V1_ناپ;
	وإلا إذا (نسخہ <= 2)
		أرجع FDT_V2_ناپ;
	وإلا إذا (نسخہ <= 3)
		أرجع FDT_V3_ناپ;
	وإلا إذا (نسخہ <= 16)
		أرجع FDT_V16_ناپ;
	وإلا
		أرجع FDT_V17_ناپ;
}

size_t fdt_سرخی_ناپ_2(ثابت فراغ *fdt)
{
	أرجع can_assume(LATEST) ? FDT_V17_ناپ :
		fdt_سرخی_ناپ(fdt_نسخہ(fdt));
}

صحيح fdt_check_سرخی(ثابت فراغ *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	إذا ((uintptr_t)fdt & 7)
		أرجع -FDT_ERR_ALIGNMENT;

	إذا (fdt_magic(fdt) != FDT_MAGIC)
		أرجع -FDT_ERR_BADMAGIC;
	إذا (!can_assume(LATEST)) {
		إذا ((fdt_نسخہ(fdt) < FDT_پہلا_SUPPORTED_نسخہ)
		    || (fdt_آخری_comp_نسخہ(fdt) >
			FDT_آخری_SUPPORTED_نسخہ))
			أرجع -FDT_ERR_BADVERSION;
		إذا (fdt_نسخہ(fdt) < fdt_آخری_comp_نسخہ(fdt))
			أرجع -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_سرخی_ناپ_2(fdt);
	إذا (!can_assume(VALID_DTB)) {

		إذا ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > ودھ_توں_ودھ_عدد))
			أرجع -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		إذا (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			أرجع -FDT_ERR_TRUNCATED;
	}

	إذا (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		إذا (!can_assume(LATEST) && fdt_نسخہ(fdt) < 17) {
			إذا (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				أرجع -FDT_ERR_TRUNCATED;
		} وإلا {
			إذا (!check_بلاک(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_ناپ_dt_struct(fdt)))
				أرجع -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		إذا (!check_بلاک(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_لڑیاں(fdt),
				  fdt_ناپ_dt_لڑیاں(fdt)))
			أرجع -FDT_ERR_TRUNCATED;
	}

	أرجع 0;
}

ثابت فراغ *fdt_وتھ_ptr(ثابت فراغ *fdt, صحيح وتھ, بنانشانی صحيح len)
{
	بنانشانی صحيح uoffset = وتھ;
	بنانشانی صحيح absoffset = وتھ + fdt_off_dt_struct(fdt);

	إذا (وتھ < 0)
		أرجع عدم;

	إذا (!can_assume(VALID_داخلہ))
		إذا ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			أرجع عدم;

	إذا (can_assume(LATEST) || fdt_نسخہ(fdt) >= 0x11)
		إذا (((uoffset + len) < uoffset)
		    || ((وتھ + len) > fdt_ناپ_dt_struct(fdt)))
			أرجع عدم;

	أرجع fdt_وتھ_ptr_2(fdt, وتھ);
}

uint32_t fdt_اگلا_ٹیگ(ثابت فراغ *fdt, صحيح startoffset, صحيح *nextoffset)
{
	ثابت fdt32_t *tagp, *lenp;
	uint32_t ٹیگ;
	صحيح وتھ = startoffset;
	ثابت حرف *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_وتھ_ptr(fdt, وتھ, FDT_TAGSIZE);
	إذا (!can_assume(VALID_DTB) && !tagp)
		أرجع FDT_سرا; /* premature end */
	ٹیگ = fdt32_منزل_cpu(*tagp);
	وتھ += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	بدل (ٹیگ) {
	حالة FDT_BEGIN_گرہ:
		/* skip name */
		افعل {
			p = fdt_وتھ_ptr(fdt, وتھ++, 1);
		} طالما (p && (*p != '\0'));
		إذا (!can_assume(VALID_DTB) && !p)
			أرجع FDT_سرا; /* premature end */
		اكسر;

	حالة FDT_PROP:
		lenp = fdt_وتھ_ptr(fdt, وتھ, حجم(*lenp));
		إذا (!can_assume(VALID_DTB) && !lenp)
			أرجع FDT_سرا; /* premature end */
		/* skip-name offset, length and value */
		وتھ += حجم(بنية fdt_property) - FDT_TAGSIZE
			+ fdt32_منزل_cpu(*lenp);
		إذا (!can_assume(LATEST) &&
		    fdt_نسخہ(fdt) < 0x10 && fdt32_منزل_cpu(*lenp) >= 8 &&
		    ((وتھ - fdt32_منزل_cpu(*lenp)) % 8) != 0)
			وتھ += 4;
		اكسر;

	حالة FDT_سرا:
	حالة FDT_سرا_گرہ:
	حالة FDT_NOP:
		اكسر;

	افتراضي:
		أرجع FDT_سرا;
	}

	إذا (!fdt_وتھ_ptr(fdt, startoffset, وتھ - startoffset))
		أرجع FDT_سرا; /* premature end */

	*nextoffset = FDT_TAGALIGN(وتھ);
	أرجع ٹیگ;
}

صحيح fdt_check_گرہ_وتھ(ثابت فراغ *fdt, صحيح وتھ)
{
	إذا (!can_assume(VALID_داخلہ)
	    && ((وتھ < 0) || (وتھ % FDT_TAGSIZE)))
		أرجع -FDT_ERR_BADOFFSET;

	إذا (fdt_اگلا_ٹیگ(fdt, وتھ, &وتھ) != FDT_BEGIN_گرہ)
		أرجع -FDT_ERR_BADOFFSET;

	أرجع وتھ;
}

صحيح fdt_check_prop_وتھ(ثابت فراغ *fdt, صحيح وتھ)
{
	إذا (!can_assume(VALID_داخلہ)
	    && ((وتھ < 0) || (وتھ % FDT_TAGSIZE)))
		أرجع -FDT_ERR_BADOFFSET;

	إذا (fdt_اگلا_ٹیگ(fdt, وتھ, &وتھ) != FDT_PROP)
		أرجع -FDT_ERR_BADOFFSET;

	أرجع وتھ;
}

صحيح fdt_اگلا_گرہ(ثابت فراغ *fdt, صحيح وتھ, صحيح *ڈونگھائی)
{
	صحيح nextoffset = 0;
	uint32_t ٹیگ;

	إذا (وتھ >= 0)
		إذا ((nextoffset = fdt_check_گرہ_وتھ(fdt, وتھ)) < 0)
			أرجع nextoffset;

	افعل {
		وتھ = nextoffset;
		ٹیگ = fdt_اگلا_ٹیگ(fdt, وتھ, &nextoffset);

		بدل (ٹیگ) {
		حالة FDT_PROP:
		حالة FDT_NOP:
			اكسر;

		حالة FDT_BEGIN_گرہ:
			إذا (ڈونگھائی)
				(*ڈونگھائی)++;
			اكسر;

		حالة FDT_سرا_گرہ:
			إذا (ڈونگھائی && ((--(*ڈونگھائی)) < 0))
				أرجع nextoffset;
			اكسر;

		حالة FDT_سرا:
			إذا ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !ڈونگھائی))
				أرجع -FDT_ERR_NOTFOUND;
			وإلا
				أرجع nextoffset;
		}
	} طالما (ٹیگ != FDT_BEGIN_گرہ);

	أرجع وتھ;
}

صحيح fdt_پہلا_subnode(ثابت فراغ *fdt, صحيح وتھ)
{
	صحيح ڈونگھائی = 0;

	وتھ = fdt_اگلا_گرہ(fdt, وتھ, &ڈونگھائی);
	إذا (وتھ < 0 || ڈونگھائی != 1)
		أرجع -FDT_ERR_NOTFOUND;

	أرجع وتھ;
}

صحيح fdt_اگلا_subnode(ثابت فراغ *fdt, صحيح وتھ)
{
	صحيح ڈونگھائی = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	افعل {
		وتھ = fdt_اگلا_گرہ(fdt, وتھ, &ڈونگھائی);
		إذا (وتھ < 0 || ڈونگھائی < 1)
			أرجع -FDT_ERR_NOTFOUND;
	} طالما (ڈونگھائی > 1);

	أرجع وتھ;
}

ثابت حرف *fdt_لبھو_string(ثابت حرف *strtab, صحيح tabsize, ثابت حرف *s)
{
	صحيح len = لڑی_لمبائی(s) + 1;
	ثابت حرف *آخری = strtab + tabsize - len;
	ثابت حرف *p;

	لكل (p = strtab; p <= آخری; p++)
		إذا (memcmp(p, s, len) == 0)
			أرجع p;
	أرجع عدم;
}

صحيح fdt_move(ثابت فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	إذا (!can_assume(VALID_داخلہ) && bufsize < 0)
		أرجع -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	إذا (fdt_totalsize(fdt) > (بنانشانی صحيح)bufsize)
		أرجع -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	أرجع 0;
}
