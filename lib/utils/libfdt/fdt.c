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
			إذا (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				أرجع -FDT_ERR_BADVERSION;
			إذا (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				أرجع -FDT_ERR_BADVERSION;
		}
	} وإلا إذا (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		إذا (!can_assume(VALID_داخلہ) && fdt_size_dt_struct(fdt) == 0)
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

ساكن صحيح check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	إذا (!check_off_(hdrsize, totalsize, base))
		أرجع 0; /* block start out of bounds */
	إذا ((base + size) < base)
		أرجع 0; /* overflow */
	إذا (!check_off_(hdrsize, totalsize, base + size))
		أرجع 0; /* block end out of bounds */
	أرجع 1;
}

size_t fdt_header_size_(uint32_t version)
{
	إذا (version <= 1)
		أرجع FDT_V1_SIZE;
	وإلا إذا (version <= 2)
		أرجع FDT_V2_SIZE;
	وإلا إذا (version <= 3)
		أرجع FDT_V3_SIZE;
	وإلا إذا (version <= 16)
		أرجع FDT_V16_SIZE;
	وإلا
		أرجع FDT_V17_SIZE;
}

size_t fdt_header_size(ثابت فراغ *fdt)
{
	أرجع can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

صحيح fdt_check_header(ثابت فراغ *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	إذا ((uintptr_t)fdt & 7)
		أرجع -FDT_ERR_ALIGNMENT;

	إذا (fdt_magic(fdt) != FDT_MAGIC)
		أرجع -FDT_ERR_BADMAGIC;
	إذا (!can_assume(LATEST)) {
		إذا ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			أرجع -FDT_ERR_BADVERSION;
		إذا (fdt_version(fdt) < fdt_last_comp_version(fdt))
			أرجع -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	إذا (!can_assume(VALID_DTB)) {

		إذا ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			أرجع -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		إذا (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			أرجع -FDT_ERR_TRUNCATED;
	}

	إذا (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		إذا (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			إذا (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				أرجع -FDT_ERR_TRUNCATED;
		} وإلا {
			إذا (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				أرجع -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		إذا (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			أرجع -FDT_ERR_TRUNCATED;
	}

	أرجع 0;
}

ثابت فراغ *fdt_offset_ptr(ثابت فراغ *fdt, صحيح offset, unsigned صحيح len)
{
	unsigned صحيح uoffset = offset;
	unsigned صحيح absoffset = offset + fdt_off_dt_struct(fdt);

	إذا (offset < 0)
		أرجع عدم;

	إذا (!can_assume(VALID_داخلہ))
		إذا ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			أرجع عدم;

	إذا (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		إذا (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			أرجع عدم;

	أرجع fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(ثابت فراغ *fdt, صحيح startoffset, صحيح *nextoffset)
{
	ثابت fdt32_t *tagp, *lenp;
	uint32_t tag;
	صحيح offset = startoffset;
	ثابت حرف *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	إذا (!can_assume(VALID_DTB) && !tagp)
		أرجع FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	بدل (tag) {
	حالة FDT_BEGIN_گرہ:
		/* skip name */
		افعل {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} طالما (p && (*p != '\0'));
		إذا (!can_assume(VALID_DTB) && !p)
			أرجع FDT_END; /* premature end */
		اكسر;

	حالة FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, حجم(*lenp));
		إذا (!can_assume(VALID_DTB) && !lenp)
			أرجع FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += حجم(بنية fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		إذا (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		اكسر;

	حالة FDT_END:
	حالة FDT_END_گرہ:
	حالة FDT_NOP:
		اكسر;

	افتراضي:
		أرجع FDT_END;
	}

	إذا (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		أرجع FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	أرجع tag;
}

صحيح fdt_check_گرہ_offset(ثابت فراغ *fdt, صحيح offset)
{
	إذا (!can_assume(VALID_داخلہ)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		أرجع -FDT_ERR_BADOFFSET;

	إذا (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_گرہ)
		أرجع -FDT_ERR_BADOFFSET;

	أرجع offset;
}

صحيح fdt_check_prop_offset_(ثابت فراغ *fdt, صحيح offset)
{
	إذا (!can_assume(VALID_داخلہ)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		أرجع -FDT_ERR_BADOFFSET;

	إذا (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		أرجع -FDT_ERR_BADOFFSET;

	أرجع offset;
}

صحيح fdt_next_گرہ(ثابت فراغ *fdt, صحيح offset, صحيح *depth)
{
	صحيح nextoffset = 0;
	uint32_t tag;

	إذا (offset >= 0)
		إذا ((nextoffset = fdt_check_گرہ_offset(fdt, offset)) < 0)
			أرجع nextoffset;

	افعل {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		بدل (tag) {
		حالة FDT_PROP:
		حالة FDT_NOP:
			اكسر;

		حالة FDT_BEGIN_گرہ:
			إذا (depth)
				(*depth)++;
			اكسر;

		حالة FDT_END_گرہ:
			إذا (depth && ((--(*depth)) < 0))
				أرجع nextoffset;
			اكسر;

		حالة FDT_END:
			إذا ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				أرجع -FDT_ERR_NOTFOUND;
			وإلا
				أرجع nextoffset;
		}
	} طالما (tag != FDT_BEGIN_گرہ);

	أرجع offset;
}

صحيح fdt_first_subnode(ثابت فراغ *fdt, صحيح offset)
{
	صحيح depth = 0;

	offset = fdt_next_گرہ(fdt, offset, &depth);
	إذا (offset < 0 || depth != 1)
		أرجع -FDT_ERR_NOTFOUND;

	أرجع offset;
}

صحيح fdt_next_subnode(ثابت فراغ *fdt, صحيح offset)
{
	صحيح depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	افعل {
		offset = fdt_next_گرہ(fdt, offset, &depth);
		إذا (offset < 0 || depth < 1)
			أرجع -FDT_ERR_NOTFOUND;
	} طالما (depth > 1);

	أرجع offset;
}

ثابت حرف *fdt_find_string_(ثابت حرف *strtab, صحيح tabsize, ثابت حرف *s)
{
	صحيح len = strlen(s) + 1;
	ثابت حرف *last = strtab + tabsize - len;
	ثابت حرف *p;

	لكل (p = strtab; p <= last; p++)
		إذا (memcmp(p, s, len) == 0)
			أرجع p;
	أرجع عدم;
}

صحيح fdt_move(ثابت فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	إذا (!can_assume(VALID_داخلہ) && bufsize < 0)
		أرجع -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	إذا (fdt_totalsize(fdt) > (unsigned صحيح)bufsize)
		أرجع -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	أرجع 0;
}
