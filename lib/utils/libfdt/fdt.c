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

	אם (can_assume(VALID_DTB))
		החזר totalsize;

	/* The device tree must be at an 8-byte aligned address */
	אם ((uintptr_t)fdt & 7)
		החזר -FDT_ERR_ALIGNMENT;

	אם (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		אם (!can_assume(LATEST)) {
			אם (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				החזר -FDT_ERR_BADVERSION;
			אם (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				החזר -FDT_ERR_BADVERSION;
		}
	} אחרת אם (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		אם (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			החזר -FDT_ERR_BADSTATE;
	} אחרת {
		החזר -FDT_ERR_BADMAGIC;
	}

	אם (totalsize < INT32_MAX)
		החזר totalsize;
	אחרת
		החזר -FDT_ERR_TRUNCATED;
}

ساكن صحيح check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	החזר (off >= hdrsize) && (off <= totalsize);
}

ساكن صحيح check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	אם (!check_off_(hdrsize, totalsize, base))
		החזר 0; /* block start out of bounds */
	אם ((base + size) < base)
		החזר 0; /* overflow */
	אם (!check_off_(hdrsize, totalsize, base + size))
		החזר 0; /* block end out of bounds */
	החזר 1;
}

size_t fdt_header_size_(uint32_t version)
{
	אם (version <= 1)
		החזר FDT_V1_SIZE;
	אחרת אם (version <= 2)
		החזר FDT_V2_SIZE;
	אחרת אם (version <= 3)
		החזר FDT_V3_SIZE;
	אחרת אם (version <= 16)
		החזר FDT_V16_SIZE;
	אחרת
		החזר FDT_V17_SIZE;
}

size_t fdt_header_size(ثابت فراغ *fdt)
{
	החזר can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

صحيح fdt_check_header(ثابت فراغ *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	אם ((uintptr_t)fdt & 7)
		החזר -FDT_ERR_ALIGNMENT;

	אם (fdt_magic(fdt) != FDT_MAGIC)
		החזר -FDT_ERR_BADMAGIC;
	אם (!can_assume(LATEST)) {
		אם ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			החזר -FDT_ERR_BADVERSION;
		אם (fdt_version(fdt) < fdt_last_comp_version(fdt))
			החזר -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	אם (!can_assume(VALID_DTB)) {

		אם ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			החזר -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		אם (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			החזר -FDT_ERR_TRUNCATED;
	}

	אם (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		אם (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			אם (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				החזר -FDT_ERR_TRUNCATED;
		} אחרת {
			אם (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				החזר -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		אם (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			החזר -FDT_ERR_TRUNCATED;
	}

	החזר 0;
}

ثابت فراغ *fdt_offset_ptr(ثابت فراغ *fdt, صحيح offset, unsigned صحيح len)
{
	unsigned صحيح uoffset = offset;
	unsigned صحيح absoffset = offset + fdt_off_dt_struct(fdt);

	אם (offset < 0)
		החזר عدم;

	אם (!can_assume(VALID_INPUT))
		אם ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			החזר عدم;

	אם (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		אם (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			החזר عدم;

	החזר fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(ثابت فراغ *fdt, صحيح startoffset, صحيح *nextoffset)
{
	ثابت fdt32_t *tagp, *lenp;
	uint32_t tag;
	صحيح offset = startoffset;
	ثابت حرف *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	אם (!can_assume(VALID_DTB) && !tagp)
		החזר FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	بدل (tag) {
	حالة FDT_BEGIN_NODE:
		/* skip name */
		עשה {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} כלעוד (p && (*p != '\0'));
		אם (!can_assume(VALID_DTB) && !p)
			החזר FDT_END; /* premature end */
		اكسر;

	حالة FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, حجم(*lenp));
		אם (!can_assume(VALID_DTB) && !lenp)
			החזר FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += حجم(بنية fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		אם (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		اكسر;

	حالة FDT_END:
	حالة FDT_END_NODE:
	حالة FDT_NOP:
		اكسر;

	افتراضي:
		החזר FDT_END;
	}

	אם (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		החזר FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	החזר tag;
}

صحيح fdt_check_node_offset_(ثابت فراغ *fdt, صحيح offset)
{
	אם (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		החזר -FDT_ERR_BADOFFSET;

	אם (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		החזר -FDT_ERR_BADOFFSET;

	החזר offset;
}

صحيح fdt_check_prop_offset_(ثابت فراغ *fdt, صحيح offset)
{
	אם (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		החזר -FDT_ERR_BADOFFSET;

	אם (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		החזר -FDT_ERR_BADOFFSET;

	החזר offset;
}

صحيح fdt_next_node(ثابت فراغ *fdt, صحيح offset, صحيح *depth)
{
	صحيح nextoffset = 0;
	uint32_t tag;

	אם (offset >= 0)
		אם ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			החזר nextoffset;

	עשה {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		بدل (tag) {
		حالة FDT_PROP:
		حالة FDT_NOP:
			اكسر;

		حالة FDT_BEGIN_NODE:
			אם (depth)
				(*depth)++;
			اكسر;

		حالة FDT_END_NODE:
			אם (depth && ((--(*depth)) < 0))
				החזר nextoffset;
			اكسر;

		حالة FDT_END:
			אם ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				החזר -FDT_ERR_NOTFOUND;
			אחרת
				החזר nextoffset;
		}
	} כלעוד (tag != FDT_BEGIN_NODE);

	החזר offset;
}

صحيح fdt_first_subnode(ثابت فراغ *fdt, صحيح offset)
{
	صحيح depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	אם (offset < 0 || depth != 1)
		החזר -FDT_ERR_NOTFOUND;

	החזר offset;
}

صحيح fdt_next_subnode(ثابت فراغ *fdt, صحيح offset)
{
	صحيح depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	עשה {
		offset = fdt_next_node(fdt, offset, &depth);
		אם (offset < 0 || depth < 1)
			החזר -FDT_ERR_NOTFOUND;
	} כלעוד (depth > 1);

	החזר offset;
}

ثابت حرف *fdt_find_string_(ثابت حرف *strtab, صحيح tabsize, ثابت حرف *s)
{
	صحيح len = strlen(s) + 1;
	ثابت حرف *last = strtab + tabsize - len;
	ثابت حرف *p;

	עבור (p = strtab; p <= last; p++)
		אם (memcmp(p, s, len) == 0)
			החזר p;
	החזר عدم;
}

صحيح fdt_move(ثابت فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	אם (!can_assume(VALID_INPUT) && bufsize < 0)
		החזר -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	אם (fdt_totalsize(fdt) > (unsigned صحيح)bufsize)
		החזר -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	החזר 0;
}
