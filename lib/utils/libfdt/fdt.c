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

	ܐܢ (can_assume(VALID_DTB))
		ܦܢܐ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ܐܢ ((uintptr_t)fdt & 7)
		ܦܢܐ -FDT_ERR_ALIGNMENT;

	ܐܢ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ܐܢ (!can_assume(LATEST)) {
			ܐܢ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ܦܢܐ -FDT_ERR_BADVERSION;
			ܐܢ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ܦܢܐ -FDT_ERR_BADVERSION;
		}
	} ܐܚܪܢܐ ܐܢ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ܐܢ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ܦܢܐ -FDT_ERR_BADSTATE;
	} ܐܚܪܢܐ {
		ܦܢܐ -FDT_ERR_BADMAGIC;
	}

	ܐܢ (totalsize < INT32_MAX)
		ܦܢܐ totalsize;
	ܐܚܪܢܐ
		ܦܢܐ -FDT_ERR_TRUNCATED;
}

ساكن صحيح check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ܦܢܐ (off >= hdrsize) && (off <= totalsize);
}

ساكن صحيح check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ܐܢ (!check_off_(hdrsize, totalsize, base))
		ܦܢܐ 0; /* block start out of bounds */
	ܐܢ ((base + size) < base)
		ܦܢܐ 0; /* overflow */
	ܐܢ (!check_off_(hdrsize, totalsize, base + size))
		ܦܢܐ 0; /* block end out of bounds */
	ܦܢܐ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ܐܢ (version <= 1)
		ܦܢܐ FDT_V1_SIZE;
	ܐܚܪܢܐ ܐܢ (version <= 2)
		ܦܢܐ FDT_V2_SIZE;
	ܐܚܪܢܐ ܐܢ (version <= 3)
		ܦܢܐ FDT_V3_SIZE;
	ܐܚܪܢܐ ܐܢ (version <= 16)
		ܦܢܐ FDT_V16_SIZE;
	ܐܚܪܢܐ
		ܦܢܐ FDT_V17_SIZE;
}

size_t fdt_header_size(ثابت فراغ *fdt)
{
	ܦܢܐ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

صحيح fdt_check_header(ثابت فراغ *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ܐܢ ((uintptr_t)fdt & 7)
		ܦܢܐ -FDT_ERR_ALIGNMENT;

	ܐܢ (fdt_magic(fdt) != FDT_MAGIC)
		ܦܢܐ -FDT_ERR_BADMAGIC;
	ܐܢ (!can_assume(LATEST)) {
		ܐܢ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ܦܢܐ -FDT_ERR_BADVERSION;
		ܐܢ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ܦܢܐ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ܐܢ (!can_assume(VALID_DTB)) {

		ܐܢ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ܦܢܐ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ܐܢ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ܦܢܐ -FDT_ERR_TRUNCATED;
	}

	ܐܢ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ܐܢ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ܐܢ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ܦܢܐ -FDT_ERR_TRUNCATED;
		} ܐܚܪܢܐ {
			ܐܢ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ܦܢܐ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ܐܢ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ܦܢܐ -FDT_ERR_TRUNCATED;
	}

	ܦܢܐ 0;
}

ثابت فراغ *fdt_offset_ptr(ثابت فراغ *fdt, صحيح offset, unsigned صحيح len)
{
	unsigned صحيح uoffset = offset;
	unsigned صحيح absoffset = offset + fdt_off_dt_struct(fdt);

	ܐܢ (offset < 0)
		ܦܢܐ عدم;

	ܐܢ (!can_assume(VALID_INPUT))
		ܐܢ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ܦܢܐ عدم;

	ܐܢ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ܐܢ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ܦܢܐ عدم;

	ܦܢܐ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(ثابت فراغ *fdt, صحيح startoffset, صحيح *nextoffset)
{
	ثابت fdt32_t *tagp, *lenp;
	uint32_t tag;
	صحيح offset = startoffset;
	ثابت حرف *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ܐܢ (!can_assume(VALID_DTB) && !tagp)
		ܦܢܐ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	بدل (tag) {
	حالة FDT_BEGIN_NODE:
		/* skip name */
		ܥܒܕ {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ܟܡܐ (p && (*p != '\0'));
		ܐܢ (!can_assume(VALID_DTB) && !p)
			ܦܢܐ FDT_END; /* premature end */
		اكسر;

	حالة FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, حجم(*lenp));
		ܐܢ (!can_assume(VALID_DTB) && !lenp)
			ܦܢܐ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += حجم(بنية fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ܐܢ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		اكسر;

	حالة FDT_END:
	حالة FDT_END_NODE:
	حالة FDT_NOP:
		اكسر;

	افتراضي:
		ܦܢܐ FDT_END;
	}

	ܐܢ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ܦܢܐ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ܦܢܐ tag;
}

صحيح fdt_check_node_offset_(ثابت فراغ *fdt, صحيح offset)
{
	ܐܢ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ܦܢܐ -FDT_ERR_BADOFFSET;

	ܐܢ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ܦܢܐ -FDT_ERR_BADOFFSET;

	ܦܢܐ offset;
}

صحيح fdt_check_prop_offset_(ثابت فراغ *fdt, صحيح offset)
{
	ܐܢ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ܦܢܐ -FDT_ERR_BADOFFSET;

	ܐܢ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ܦܢܐ -FDT_ERR_BADOFFSET;

	ܦܢܐ offset;
}

صحيح fdt_next_node(ثابت فراغ *fdt, صحيح offset, صحيح *depth)
{
	صحيح nextoffset = 0;
	uint32_t tag;

	ܐܢ (offset >= 0)
		ܐܢ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ܦܢܐ nextoffset;

	ܥܒܕ {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		بدل (tag) {
		حالة FDT_PROP:
		حالة FDT_NOP:
			اكسر;

		حالة FDT_BEGIN_NODE:
			ܐܢ (depth)
				(*depth)++;
			اكسر;

		حالة FDT_END_NODE:
			ܐܢ (depth && ((--(*depth)) < 0))
				ܦܢܐ nextoffset;
			اكسر;

		حالة FDT_END:
			ܐܢ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ܦܢܐ -FDT_ERR_NOTFOUND;
			ܐܚܪܢܐ
				ܦܢܐ nextoffset;
		}
	} ܟܡܐ (tag != FDT_BEGIN_NODE);

	ܦܢܐ offset;
}

صحيح fdt_first_subnode(ثابت فراغ *fdt, صحيح offset)
{
	صحيح depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ܐܢ (offset < 0 || depth != 1)
		ܦܢܐ -FDT_ERR_NOTFOUND;

	ܦܢܐ offset;
}

صحيح fdt_next_subnode(ثابت فراغ *fdt, صحيح offset)
{
	صحيح depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	ܥܒܕ {
		offset = fdt_next_node(fdt, offset, &depth);
		ܐܢ (offset < 0 || depth < 1)
			ܦܢܐ -FDT_ERR_NOTFOUND;
	} ܟܡܐ (depth > 1);

	ܦܢܐ offset;
}

ثابت حرف *fdt_find_string_(ثابت حرف *strtab, صحيح tabsize, ثابت حرف *s)
{
	صحيح len = strlen(s) + 1;
	ثابت حرف *last = strtab + tabsize - len;
	ثابت حرف *p;

	لكل (p = strtab; p <= last; p++)
		ܐܢ (memcmp(p, s, len) == 0)
			ܦܢܐ p;
	ܦܢܐ عدم;
}

صحيح fdt_move(ثابت فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	ܐܢ (!can_assume(VALID_INPUT) && bufsize < 0)
		ܦܢܐ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ܐܢ (fdt_totalsize(fdt) > (unsigned صحيح)bufsize)
		ܦܢܐ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ܦܢܐ 0;
}
