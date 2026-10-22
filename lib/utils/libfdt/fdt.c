// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(ثابت خالی *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	اگر (can_assume(VALID_DTB))
		واپس totalsize;

	/* The device tree must be at an 8-byte aligned address */
	اگر ((uintptr_t)fdt & 7)
		واپس -FDT_ERR_ALIGNMENT;

	اگر (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		اگر (!can_assume(LATEST)) {
			اگر (fdt_نسخہ(fdt) < FDT_پہلا_SUPPORTED_نسخہ)
				واپس -FDT_ERR_BADVERSION;
			اگر (fdt_آخری_comp_نسخہ(fdt) >
					FDT_آخری_SUPPORTED_نسخہ)
				واپس -FDT_ERR_BADVERSION;
		}
	} ورنہ اگر (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		اگر (!can_assume(VALID_داخلہ) && fdt_سائز_dt_struct(fdt) == 0)
			واپس -FDT_ERR_BADSTATE;
	} ورنہ {
		واپس -FDT_ERR_BADMAGIC;
	}

	اگر (totalsize < INT32_MAX)
		واپس totalsize;
	ورنہ
		واپس -FDT_ERR_TRUNCATED;
}

ساکن صحیح check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	واپس (off >= hdrsize) && (off <= totalsize);
}

ساکن صحیح check_بلاک(uint32_t hdrsize, uint32_t totalsize,
			uint32_t بنیاد_2, uint32_t سائز)
{
	اگر (!check_off_(hdrsize, totalsize, بنیاد_2))
		واپس 0; /* block start out of bounds */
	اگر ((بنیاد_2 + سائز) < بنیاد_2)
		واپس 0; /*حد_سے_تجاوز*/
	اگر (!check_off_(hdrsize, totalsize, بنیاد_2 + سائز))
		واپس 0; /* block end out of bounds */
	واپس 1;
}

size_t fdt_سرخی_سائز(uint32_t نسخہ)
{
	اگر (نسخہ <= 1)
		واپس FDT_V1_سائز;
	ورنہ اگر (نسخہ <= 2)
		واپس FDT_V2_سائز;
	ورنہ اگر (نسخہ <= 3)
		واپس FDT_V3_سائز;
	ورنہ اگر (نسخہ <= 16)
		واپس FDT_V16_سائز;
	ورنہ
		واپس FDT_V17_سائز;
}

size_t fdt_سرخی_سائز_2(ثابت خالی *fdt)
{
	واپس can_assume(LATEST) ? FDT_V17_سائز :
		fdt_سرخی_سائز(fdt_نسخہ(fdt));
}

صحیح fdt_check_سرخی(ثابت خالی *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	اگر ((uintptr_t)fdt & 7)
		واپس -FDT_ERR_ALIGNMENT;

	اگر (fdt_magic(fdt) != FDT_MAGIC)
		واپس -FDT_ERR_BADMAGIC;
	اگر (!can_assume(LATEST)) {
		اگر ((fdt_نسخہ(fdt) < FDT_پہلا_SUPPORTED_نسخہ)
		    || (fdt_آخری_comp_نسخہ(fdt) >
			FDT_آخری_SUPPORTED_نسخہ))
			واپس -FDT_ERR_BADVERSION;
		اگر (fdt_نسخہ(fdt) < fdt_آخری_comp_نسخہ(fdt))
			واپس -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_سرخی_سائز_2(fdt);
	اگر (!can_assume(VALID_DTB)) {

		اگر ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > زیادہ_سے_زیادہ_عدد))
			واپس -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		اگر (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			واپس -FDT_ERR_TRUNCATED;
	}

	اگر (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		اگر (!can_assume(LATEST) && fdt_نسخہ(fdt) < 17) {
			اگر (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				واپس -FDT_ERR_TRUNCATED;
		} ورنہ {
			اگر (!check_بلاک(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_سائز_dt_struct(fdt)))
				واپس -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		اگر (!check_بلاک(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_لڑیاں(fdt),
				  fdt_سائز_dt_لڑیاں(fdt)))
			واپس -FDT_ERR_TRUNCATED;
	}

	واپس 0;
}

ثابت خالی *fdt_انحراف_ptr(ثابت خالی *fdt, صحیح انحراف, غیرعلامتی صحیح len)
{
	غیرعلامتی صحیح uoffset = انحراف;
	غیرعلامتی صحیح absoffset = انحراف + fdt_off_dt_struct(fdt);

	اگر (انحراف < 0)
		واپس تہی;

	اگر (!can_assume(VALID_داخلہ))
		اگر ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			واپس تہی;

	اگر (can_assume(LATEST) || fdt_نسخہ(fdt) >= 0x11)
		اگر (((uoffset + len) < uoffset)
		    || ((انحراف + len) > fdt_سائز_dt_struct(fdt)))
			واپس تہی;

	واپس fdt_انحراف_ptr_2(fdt, انحراف);
}

uint32_t fdt_اگلا_لیبل(ثابت خالی *fdt, صحیح startoffset, صحیح *nextoffset)
{
	ثابت fdt32_t *tagp, *lenp;
	uint32_t لیبل;
	صحیح انحراف = startoffset;
	ثابت حرف *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_انحراف_ptr(fdt, انحراف, FDT_TAGSIZE);
	اگر (!can_assume(VALID_DTB) && !tagp)
		واپس FDT_انتہا; /* premature end */
	لیبل = fdt32_مقصد_cpu(*tagp);
	انحراف += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	انتخاب (لیبل) {
	حالت FDT_BEGIN_گرہ:
		/* skip name */
		کرو {
			p = fdt_انحراف_ptr(fdt, انحراف++, 1);
		} جب (p && (*p != '\0'));
		اگر (!can_assume(VALID_DTB) && !p)
			واپس FDT_انتہا; /* premature end */
		توڑو;

	حالت FDT_PROP:
		lenp = fdt_انحراف_ptr(fdt, انحراف, حجم(*lenp));
		اگر (!can_assume(VALID_DTB) && !lenp)
			واپس FDT_انتہا; /* premature end */
		/* skip-name offset, length and value */
		انحراف += حجم(ساخت fdt_property) - FDT_TAGSIZE
			+ fdt32_مقصد_cpu(*lenp);
		اگر (!can_assume(LATEST) &&
		    fdt_نسخہ(fdt) < 0x10 && fdt32_مقصد_cpu(*lenp) >= 8 &&
		    ((انحراف - fdt32_مقصد_cpu(*lenp)) % 8) != 0)
			انحراف += 4;
		توڑو;

	حالت FDT_انتہا:
	حالت FDT_انتہا_گرہ:
	حالت FDT_NOP:
		توڑو;

	طےشدہ:
		واپس FDT_انتہا;
	}

	اگر (!fdt_انحراف_ptr(fdt, startoffset, انحراف - startoffset))
		واپس FDT_انتہا; /* premature end */

	*nextoffset = FDT_TAGALIGN(انحراف);
	واپس لیبل;
}

صحیح fdt_check_گرہ_انحراف(ثابت خالی *fdt, صحیح انحراف)
{
	اگر (!can_assume(VALID_داخلہ)
	    && ((انحراف < 0) || (انحراف % FDT_TAGSIZE)))
		واپس -FDT_ERR_BADOFFSET;

	اگر (fdt_اگلا_لیبل(fdt, انحراف, &انحراف) != FDT_BEGIN_گرہ)
		واپس -FDT_ERR_BADOFFSET;

	واپس انحراف;
}

صحیح fdt_check_prop_انحراف(ثابت خالی *fdt, صحیح انحراف)
{
	اگر (!can_assume(VALID_داخلہ)
	    && ((انحراف < 0) || (انحراف % FDT_TAGSIZE)))
		واپس -FDT_ERR_BADOFFSET;

	اگر (fdt_اگلا_لیبل(fdt, انحراف, &انحراف) != FDT_PROP)
		واپس -FDT_ERR_BADOFFSET;

	واپس انحراف;
}

صحیح fdt_اگلا_گرہ(ثابت خالی *fdt, صحیح انحراف, صحیح *گہرائی)
{
	صحیح nextoffset = 0;
	uint32_t لیبل;

	اگر (انحراف >= 0)
		اگر ((nextoffset = fdt_check_گرہ_انحراف(fdt, انحراف)) < 0)
			واپس nextoffset;

	کرو {
		انحراف = nextoffset;
		لیبل = fdt_اگلا_لیبل(fdt, انحراف, &nextoffset);

		انتخاب (لیبل) {
		حالت FDT_PROP:
		حالت FDT_NOP:
			توڑو;

		حالت FDT_BEGIN_گرہ:
			اگر (گہرائی)
				(*گہرائی)++;
			توڑو;

		حالت FDT_انتہا_گرہ:
			اگر (گہرائی && ((--(*گہرائی)) < 0))
				واپس nextoffset;
			توڑو;

		حالت FDT_انتہا:
			اگر ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !گہرائی))
				واپس -FDT_ERR_NOTFOUND;
			ورنہ
				واپس nextoffset;
		}
	} جب (لیبل != FDT_BEGIN_گرہ);

	واپس انحراف;
}

صحیح fdt_پہلا_subnode(ثابت خالی *fdt, صحیح انحراف)
{
	صحیح گہرائی = 0;

	انحراف = fdt_اگلا_گرہ(fdt, انحراف, &گہرائی);
	اگر (انحراف < 0 || گہرائی != 1)
		واپس -FDT_ERR_NOTFOUND;

	واپس انحراف;
}

صحیح fdt_اگلا_subnode(ثابت خالی *fdt, صحیح انحراف)
{
	صحیح گہرائی = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	کرو {
		انحراف = fdt_اگلا_گرہ(fdt, انحراف, &گہرائی);
		اگر (انحراف < 0 || گہرائی < 1)
			واپس -FDT_ERR_NOTFOUND;
	} جب (گہرائی > 1);

	واپس انحراف;
}

ثابت حرف *fdt_ڈھونڈو_string(ثابت حرف *strtab, صحیح tabsize, ثابت حرف *s)
{
	صحیح len = لڑی_لمبائی(s) + 1;
	ثابت حرف *آخری = strtab + tabsize - len;
	ثابت حرف *p;

	برائے (p = strtab; p <= آخری; p++)
		اگر (memcmp(p, s, len) == 0)
			واپس p;
	واپس تہی;
}

صحیح fdt_move(ثابت خالی *fdt, خالی *buf, صحیح bufsize)
{
	اگر (!can_assume(VALID_داخلہ) && bufsize < 0)
		واپس -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	اگر (fdt_totalsize(fdt) > (غیرعلامتی صحیح)bufsize)
		واپس -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	واپس 0;
}
