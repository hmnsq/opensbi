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

	𐤀𐤌 (can_assume(VALID_DTB))
		أرجع totalsize;

	/* The device tree must be at an 8-byte aligned address */
	𐤀𐤌 ((uintptr_t)fdt & 7)
		أرجع -FDT_ERR_ALIGNMENT;

	𐤀𐤌 (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		𐤀𐤌 (!can_assume(LATEST)) {
			𐤀𐤌 (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				أرجع -FDT_ERR_BADVERSION;
			𐤀𐤌 (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				أرجع -FDT_ERR_BADVERSION;
		}
	} وإلا 𐤀𐤌 (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		𐤀𐤌 (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			أرجع -FDT_ERR_BADSTATE;
	} وإلا {
		أرجع -FDT_ERR_BADMAGIC;
	}

	𐤀𐤌 (totalsize < INT32_MAX)
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
	𐤀𐤌 (!check_off_(hdrsize, totalsize, base))
		أرجع 0; /* block start out of bounds */
	𐤀𐤌 ((base + size) < base)
		أرجع 0; /* overflow */
	𐤀𐤌 (!check_off_(hdrsize, totalsize, base + size))
		أرجع 0; /* block end out of bounds */
	أرجع 1;
}

size_t fdt_header_size_(uint32_t version)
{
	𐤀𐤌 (version <= 1)
		أرجع FDT_V1_SIZE;
	وإلا 𐤀𐤌 (version <= 2)
		أرجع FDT_V2_SIZE;
	وإلا 𐤀𐤌 (version <= 3)
		أرجع FDT_V3_SIZE;
	وإلا 𐤀𐤌 (version <= 16)
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
	𐤀𐤌 ((uintptr_t)fdt & 7)
		أرجع -FDT_ERR_ALIGNMENT;

	𐤀𐤌 (fdt_magic(fdt) != FDT_MAGIC)
		أرجع -FDT_ERR_BADMAGIC;
	𐤀𐤌 (!can_assume(LATEST)) {
		𐤀𐤌 ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			أرجع -FDT_ERR_BADVERSION;
		𐤀𐤌 (fdt_version(fdt) < fdt_last_comp_version(fdt))
			أرجع -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	𐤀𐤌 (!can_assume(VALID_DTB)) {

		𐤀𐤌 ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			أرجع -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		𐤀𐤌 (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			أرجع -FDT_ERR_TRUNCATED;
	}

	𐤀𐤌 (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		𐤀𐤌 (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			𐤀𐤌 (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				أرجع -FDT_ERR_TRUNCATED;
		} وإلا {
			𐤀𐤌 (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				أرجع -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		𐤀𐤌 (!check_block_(hdrsize, fdt_totalsize(fdt),
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

	𐤀𐤌 (offset < 0)
		أرجع عدم;

	𐤀𐤌 (!can_assume(VALID_INPUT))
		𐤀𐤌 ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			أرجع عدم;

	𐤀𐤌 (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		𐤀𐤌 (((uoffset + len) < uoffset)
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
	𐤀𐤌 (!can_assume(VALID_DTB) && !tagp)
		أرجع FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	بدل (tag) {
	حالة FDT_BEGIN_NODE:
		/* skip name */
		𐤐𐤏𐤋 {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} طالما (p && (*p != '\0'));
		𐤀𐤌 (!can_assume(VALID_DTB) && !p)
			أرجع FDT_END; /* premature end */
		اكسر;

	حالة FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, حجم(*lenp));
		𐤀𐤌 (!can_assume(VALID_DTB) && !lenp)
			أرجع FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += حجم(بنية fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		𐤀𐤌 (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		اكسر;

	حالة FDT_END:
	حالة FDT_END_NODE:
	حالة FDT_NOP:
		اكسر;

	افتراضي:
		أرجع FDT_END;
	}

	𐤀𐤌 (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		أرجع FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	أرجع tag;
}

صحيح fdt_check_node_offset_(ثابت فراغ *fdt, صحيح offset)
{
	𐤀𐤌 (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		أرجع -FDT_ERR_BADOFFSET;

	𐤀𐤌 (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		أرجع -FDT_ERR_BADOFFSET;

	أرجع offset;
}

صحيح fdt_check_prop_offset_(ثابت فراغ *fdt, صحيح offset)
{
	𐤀𐤌 (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		أرجع -FDT_ERR_BADOFFSET;

	𐤀𐤌 (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		أرجع -FDT_ERR_BADOFFSET;

	أرجع offset;
}

صحيح fdt_next_node(ثابت فراغ *fdt, صحيح offset, صحيح *depth)
{
	صحيح nextoffset = 0;
	uint32_t tag;

	𐤀𐤌 (offset >= 0)
		𐤀𐤌 ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			أرجع nextoffset;

	𐤐𐤏𐤋 {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		بدل (tag) {
		حالة FDT_PROP:
		حالة FDT_NOP:
			اكسر;

		حالة FDT_BEGIN_NODE:
			𐤀𐤌 (depth)
				(*depth)++;
			اكسر;

		حالة FDT_END_NODE:
			𐤀𐤌 (depth && ((--(*depth)) < 0))
				أرجع nextoffset;
			اكسر;

		حالة FDT_END:
			𐤀𐤌 ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				أرجع -FDT_ERR_NOTFOUND;
			وإلا
				أرجع nextoffset;
		}
	} طالما (tag != FDT_BEGIN_NODE);

	أرجع offset;
}

صحيح fdt_first_subnode(ثابت فراغ *fdt, صحيح offset)
{
	صحيح depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	𐤀𐤌 (offset < 0 || depth != 1)
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
	𐤐𐤏𐤋 {
		offset = fdt_next_node(fdt, offset, &depth);
		𐤀𐤌 (offset < 0 || depth < 1)
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
		𐤀𐤌 (memcmp(p, s, len) == 0)
			أرجع p;
	أرجع عدم;
}

صحيح fdt_move(ثابت فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	𐤀𐤌 (!can_assume(VALID_INPUT) && bufsize < 0)
		أرجع -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	𐤀𐤌 (fdt_totalsize(fdt) > (unsigned صحيح)bufsize)
		أرجع -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	أرجع 0;
}
