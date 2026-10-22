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
			إذا (fdt_نسخه(fdt) < FDT_اولی_SUPPORTED_نسخه)
				أرجع -FDT_ERR_BADVERSION;
			إذا (fdt_آخرین_comp_نسخه(fdt) >
					FDT_آخرین_SUPPORTED_نسخه)
				أرجع -FDT_ERR_BADVERSION;
		}
	} وإلا إذا (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		إذا (!can_assume(VALID_درون_داد) && fdt_اندازه_dt_struct(fdt) == 0)
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

ساكن صحيح check_بلوک(uint32_t hdrsize, uint32_t totalsize,
			uint32_t پایه_2, uint32_t اندازه)
{
	إذا (!check_off_(hdrsize, totalsize, پایه_2))
		أرجع 0; /* block start out of bounds */
	إذا ((پایه_2 + اندازه) < پایه_2)
		أرجع 0; /*سرریز*/
	إذا (!check_off_(hdrsize, totalsize, پایه_2 + اندازه))
		أرجع 0; /* block end out of bounds */
	أرجع 1;
}

size_t fdt_سرآیند_اندازه(uint32_t نسخه)
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

size_t fdt_سرآیند_اندازه_2(ثابت فراغ *fdt)
{
	أرجع can_assume(LATEST) ? FDT_V17_اندازه :
		fdt_سرآیند_اندازه(fdt_نسخه(fdt));
}

صحيح fdt_check_سرآیند(ثابت فراغ *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	إذا ((uintptr_t)fdt & 7)
		أرجع -FDT_ERR_ALIGNMENT;

	إذا (fdt_magic(fdt) != FDT_MAGIC)
		أرجع -FDT_ERR_BADMAGIC;
	إذا (!can_assume(LATEST)) {
		إذا ((fdt_نسخه(fdt) < FDT_اولی_SUPPORTED_نسخه)
		    || (fdt_آخرین_comp_نسخه(fdt) >
			FDT_آخرین_SUPPORTED_نسخه))
			أرجع -FDT_ERR_BADVERSION;
		إذا (fdt_نسخه(fdt) < fdt_آخرین_comp_نسخه(fdt))
			أرجع -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_سرآیند_اندازه_2(fdt);
	إذا (!can_assume(VALID_DTB)) {

		إذا ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > بیشترین_صحیح))
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
			إذا (!check_بلوک(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_اندازه_dt_struct(fdt)))
				أرجع -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		إذا (!check_بلوک(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_رشتهها(fdt),
				  fdt_اندازه_dt_رشتهها(fdt)))
			أرجع -FDT_ERR_TRUNCATED;
	}

	أرجع 0;
}

ثابت فراغ *fdt_جابه_جایی_ptr(ثابت فراغ *fdt, صحيح جابه_جایی, بیعلامت صحيح len)
{
	بیعلامت صحيح uoffset = جابه_جایی;
	بیعلامت صحيح absoffset = جابه_جایی + fdt_off_dt_struct(fdt);

	إذا (جابه_جایی < 0)
		أرجع عدم;

	إذا (!can_assume(VALID_درون_داد))
		إذا ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			أرجع عدم;

	إذا (can_assume(LATEST) || fdt_نسخه(fdt) >= 0x11)
		إذا (((uoffset + len) < uoffset)
		    || ((جابه_جایی + len) > fdt_اندازه_dt_struct(fdt)))
			أرجع عدم;

	أرجع fdt_جابه_جایی_ptr_2(fdt, جابه_جایی);
}

uint32_t fdt_بعدی_برچسب(ثابت فراغ *fdt, صحيح startoffset, صحيح *nextoffset)
{
	ثابت fdt32_t *tagp, *lenp;
	uint32_t برچسب;
	صحيح جابه_جایی = startoffset;
	ثابت حرف *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_جابه_جایی_ptr(fdt, جابه_جایی, FDT_TAGSIZE);
	إذا (!can_assume(VALID_DTB) && !tagp)
		أرجع FDT_انتها; /* premature end */
	برچسب = fdt32_مقصد_cpu(*tagp);
	جابه_جایی += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	بدل (برچسب) {
	حالة FDT_BEGIN_گره:
		/* skip name */
		افعل {
			p = fdt_جابه_جایی_ptr(fdt, جابه_جایی++, 1);
		} طالما (p && (*p != '\0'));
		إذا (!can_assume(VALID_DTB) && !p)
			أرجع FDT_انتها; /* premature end */
		اكسر;

	حالة FDT_PROP:
		lenp = fdt_جابه_جایی_ptr(fdt, جابه_جایی, حجم(*lenp));
		إذا (!can_assume(VALID_DTB) && !lenp)
			أرجع FDT_انتها; /* premature end */
		/* skip-name offset, length and value */
		جابه_جایی += حجم(بنية fdt_property) - FDT_TAGSIZE
			+ fdt32_مقصد_cpu(*lenp);
		إذا (!can_assume(LATEST) &&
		    fdt_نسخه(fdt) < 0x10 && fdt32_مقصد_cpu(*lenp) >= 8 &&
		    ((جابه_جایی - fdt32_مقصد_cpu(*lenp)) % 8) != 0)
			جابه_جایی += 4;
		اكسر;

	حالة FDT_انتها:
	حالة FDT_انتها_گره:
	حالة FDT_NOP:
		اكسر;

	افتراضي:
		أرجع FDT_انتها;
	}

	إذا (!fdt_جابه_جایی_ptr(fdt, startoffset, جابه_جایی - startoffset))
		أرجع FDT_انتها; /* premature end */

	*nextoffset = FDT_TAGALIGN(جابه_جایی);
	أرجع برچسب;
}

صحيح fdt_check_گره_جابه_جایی(ثابت فراغ *fdt, صحيح جابه_جایی)
{
	إذا (!can_assume(VALID_درون_داد)
	    && ((جابه_جایی < 0) || (جابه_جایی % FDT_TAGSIZE)))
		أرجع -FDT_ERR_BADOFFSET;

	إذا (fdt_بعدی_برچسب(fdt, جابه_جایی, &جابه_جایی) != FDT_BEGIN_گره)
		أرجع -FDT_ERR_BADOFFSET;

	أرجع جابه_جایی;
}

صحيح fdt_check_prop_جابه_جایی(ثابت فراغ *fdt, صحيح جابه_جایی)
{
	إذا (!can_assume(VALID_درون_داد)
	    && ((جابه_جایی < 0) || (جابه_جایی % FDT_TAGSIZE)))
		أرجع -FDT_ERR_BADOFFSET;

	إذا (fdt_بعدی_برچسب(fdt, جابه_جایی, &جابه_جایی) != FDT_PROP)
		أرجع -FDT_ERR_BADOFFSET;

	أرجع جابه_جایی;
}

صحيح fdt_بعدی_گره(ثابت فراغ *fdt, صحيح جابه_جایی, صحيح *عمق)
{
	صحيح nextoffset = 0;
	uint32_t برچسب;

	إذا (جابه_جایی >= 0)
		إذا ((nextoffset = fdt_check_گره_جابه_جایی(fdt, جابه_جایی)) < 0)
			أرجع nextoffset;

	افعل {
		جابه_جایی = nextoffset;
		برچسب = fdt_بعدی_برچسب(fdt, جابه_جایی, &nextoffset);

		بدل (برچسب) {
		حالة FDT_PROP:
		حالة FDT_NOP:
			اكسر;

		حالة FDT_BEGIN_گره:
			إذا (عمق)
				(*عمق)++;
			اكسر;

		حالة FDT_انتها_گره:
			إذا (عمق && ((--(*عمق)) < 0))
				أرجع nextoffset;
			اكسر;

		حالة FDT_انتها:
			إذا ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !عمق))
				أرجع -FDT_ERR_NOTFOUND;
			وإلا
				أرجع nextoffset;
		}
	} طالما (برچسب != FDT_BEGIN_گره);

	أرجع جابه_جایی;
}

صحيح fdt_اولی_subnode(ثابت فراغ *fdt, صحيح جابه_جایی)
{
	صحيح عمق = 0;

	جابه_جایی = fdt_بعدی_گره(fdt, جابه_جایی, &عمق);
	إذا (جابه_جایی < 0 || عمق != 1)
		أرجع -FDT_ERR_NOTFOUND;

	أرجع جابه_جایی;
}

صحيح fdt_بعدی_subnode(ثابت فراغ *fdt, صحيح جابه_جایی)
{
	صحيح عمق = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	افعل {
		جابه_جایی = fdt_بعدی_گره(fdt, جابه_جایی, &عمق);
		إذا (جابه_جایی < 0 || عمق < 1)
			أرجع -FDT_ERR_NOTFOUND;
	} طالما (عمق > 1);

	أرجع جابه_جایی;
}

ثابت حرف *fdt_پیدا_کن_string(ثابت حرف *strtab, صحيح tabsize, ثابت حرف *s)
{
	صحيح len = طول_رشته(s) + 1;
	ثابت حرف *آخرین = strtab + tabsize - len;
	ثابت حرف *p;

	لكل (p = strtab; p <= آخرین; p++)
		إذا (memcmp(p, s, len) == 0)
			أرجع p;
	أرجع عدم;
}

صحيح fdt_move(ثابت فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	إذا (!can_assume(VALID_درون_داد) && bufsize < 0)
		أرجع -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	إذا (fdt_totalsize(fdt) > (بیعلامت صحيح)bufsize)
		أرجع -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	أرجع 0;
}
