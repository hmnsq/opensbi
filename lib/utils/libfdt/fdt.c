// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(ধ্রুব শূন্য *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	যদি (can_assume(VALID_DTB))
		প্রত্যাগম totalsize;

	/* The device tree must be at an 8-byte aligned address */
	যদি ((uintptr_t)fdt & 7)
		প্রত্যাগম -FDT_ERR_ALIGNMENT;

	যদি (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		যদি (!can_assume(LATEST)) {
			যদি (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				প্রত্যাগম -FDT_ERR_BADVERSION;
			যদি (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				প্রত্যাগম -FDT_ERR_BADVERSION;
		}
	} অন্যথা যদি (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		যদি (!can_assume(VALID_প্রবেশ) && fdt_size_dt_struct(fdt) == 0)
			প্রত্যাগম -FDT_ERR_BADSTATE;
	} অন্যথা {
		প্রত্যাগম -FDT_ERR_BADMAGIC;
	}

	যদি (totalsize < INT32_MAX)
		প্রত্যাগম totalsize;
	অন্যথা
		প্রত্যাগম -FDT_ERR_TRUNCATED;
}

স্থির পূর্ণাঙ্ক check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	প্রত্যাগম (off >= hdrsize) && (off <= totalsize);
}

স্থির পূর্ণাঙ্ক check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	যদি (!check_off_(hdrsize, totalsize, base))
		প্রত্যাগম 0; /* block start out of bounds */
	যদি ((base + size) < base)
		প্রত্যাগম 0; /* overflow */
	যদি (!check_off_(hdrsize, totalsize, base + size))
		প্রত্যাগম 0; /* block end out of bounds */
	প্রত্যাগম 1;
}

size_t fdt_header_size_(uint32_t version)
{
	যদি (version <= 1)
		প্রত্যাগম FDT_V1_SIZE;
	অন্যথা যদি (version <= 2)
		প্রত্যাগম FDT_V2_SIZE;
	অন্যথা যদি (version <= 3)
		প্রত্যাগম FDT_V3_SIZE;
	অন্যথা যদি (version <= 16)
		প্রত্যাগম FDT_V16_SIZE;
	অন্যথা
		প্রত্যাগম FDT_V17_SIZE;
}

size_t fdt_header_size(ধ্রুব শূন্য *fdt)
{
	প্রত্যাগম can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

পূর্ণাঙ্ক fdt_check_header(ধ্রুব শূন্য *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	যদি ((uintptr_t)fdt & 7)
		প্রত্যাগম -FDT_ERR_ALIGNMENT;

	যদি (fdt_magic(fdt) != FDT_MAGIC)
		প্রত্যাগম -FDT_ERR_BADMAGIC;
	যদি (!can_assume(LATEST)) {
		যদি ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			প্রত্যাগম -FDT_ERR_BADVERSION;
		যদি (fdt_version(fdt) < fdt_last_comp_version(fdt))
			প্রত্যাগম -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	যদি (!can_assume(VALID_DTB)) {

		যদি ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			প্রত্যাগম -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		যদি (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			প্রত্যাগম -FDT_ERR_TRUNCATED;
	}

	যদি (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		যদি (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			যদি (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				প্রত্যাগম -FDT_ERR_TRUNCATED;
		} অন্যথা {
			যদি (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				প্রত্যাগম -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		যদি (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			প্রত্যাগম -FDT_ERR_TRUNCATED;
	}

	প্রত্যাগম 0;
}

ধ্রুব শূন্য *fdt_offset_ptr(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক offset, unsigned পূর্ণাঙ্ক len)
{
	unsigned পূর্ণাঙ্ক uoffset = offset;
	unsigned পূর্ণাঙ্ক absoffset = offset + fdt_off_dt_struct(fdt);

	যদি (offset < 0)
		প্রত্যাগম রিক্ত;

	যদি (!can_assume(VALID_প্রবেশ))
		যদি ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			প্রত্যাগম রিক্ত;

	যদি (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		যদি (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			প্রত্যাগম রিক্ত;

	প্রত্যাগম fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক startoffset, পূর্ণাঙ্ক *nextoffset)
{
	ধ্রুব fdt32_t *tagp, *lenp;
	uint32_t tag;
	পূর্ণাঙ্ক offset = startoffset;
	ধ্রুব বর্ণ *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	যদি (!can_assume(VALID_DTB) && !tagp)
		প্রত্যাগম FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	বিকল্প (tag) {
	পক্ষ FDT_BEGIN_গ্রন্থি:
		/* skip name */
		কুরু {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} যাবত্ (p && (*p != '\0'));
		যদি (!can_assume(VALID_DTB) && !p)
			প্রত্যাগম FDT_END; /* premature end */
		ভঙ্গ;

	পক্ষ FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, পরিমাণ(*lenp));
		যদি (!can_assume(VALID_DTB) && !lenp)
			প্রত্যাগম FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += পরিমাণ(সংরচনা fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		যদি (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		ভঙ্গ;

	পক্ষ FDT_END:
	পক্ষ FDT_END_গ্রন্থি:
	পক্ষ FDT_NOP:
		ভঙ্গ;

	স্বভাব:
		প্রত্যাগম FDT_END;
	}

	যদি (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		প্রত্যাগম FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	প্রত্যাগম tag;
}

পূর্ণাঙ্ক fdt_check_গ্রন্থি_offset(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	যদি (!can_assume(VALID_প্রবেশ)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		প্রত্যাগম -FDT_ERR_BADOFFSET;

	যদি (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_গ্রন্থি)
		প্রত্যাগম -FDT_ERR_BADOFFSET;

	প্রত্যাগম offset;
}

পূর্ণাঙ্ক fdt_check_prop_offset_(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	যদি (!can_assume(VALID_প্রবেশ)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		প্রত্যাগম -FDT_ERR_BADOFFSET;

	যদি (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		প্রত্যাগম -FDT_ERR_BADOFFSET;

	প্রত্যাগম offset;
}

পূর্ণাঙ্ক fdt_next_গ্রন্থি(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক offset, পূর্ণাঙ্ক *depth)
{
	পূর্ণাঙ্ক nextoffset = 0;
	uint32_t tag;

	যদি (offset >= 0)
		যদি ((nextoffset = fdt_check_গ্রন্থি_offset(fdt, offset)) < 0)
			প্রত্যাগম nextoffset;

	কুরু {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		বিকল্প (tag) {
		পক্ষ FDT_PROP:
		পক্ষ FDT_NOP:
			ভঙ্গ;

		পক্ষ FDT_BEGIN_গ্রন্থি:
			যদি (depth)
				(*depth)++;
			ভঙ্গ;

		পক্ষ FDT_END_গ্রন্থি:
			যদি (depth && ((--(*depth)) < 0))
				প্রত্যাগম nextoffset;
			ভঙ্গ;

		পক্ষ FDT_END:
			যদি ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				প্রত্যাগম -FDT_ERR_NOTFOUND;
			অন্যথা
				প্রত্যাগম nextoffset;
		}
	} যাবত্ (tag != FDT_BEGIN_গ্রন্থি);

	প্রত্যাগম offset;
}

পূর্ণাঙ্ক fdt_first_subnode(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	পূর্ণাঙ্ক depth = 0;

	offset = fdt_next_গ্রন্থি(fdt, offset, &depth);
	যদি (offset < 0 || depth != 1)
		প্রত্যাগম -FDT_ERR_NOTFOUND;

	প্রত্যাগম offset;
}

পূর্ণাঙ্ক fdt_next_subnode(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	পূর্ণাঙ্ক depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	কুরু {
		offset = fdt_next_গ্রন্থি(fdt, offset, &depth);
		যদি (offset < 0 || depth < 1)
			প্রত্যাগম -FDT_ERR_NOTFOUND;
	} যাবত্ (depth > 1);

	প্রত্যাগম offset;
}

ধ্রুব বর্ণ *fdt_find_string_(ধ্রুব বর্ণ *strtab, পূর্ণাঙ্ক tabsize, ধ্রুব বর্ণ *s)
{
	পূর্ণাঙ্ক len = strlen(s) + 1;
	ধ্রুব বর্ণ *last = strtab + tabsize - len;
	ধ্রুব বর্ণ *p;

	প্রতি (p = strtab; p <= last; p++)
		যদি (memcmp(p, s, len) == 0)
			প্রত্যাগম p;
	প্রত্যাগম রিক্ত;
}

পূর্ণাঙ্ক fdt_move(ধ্রুব শূন্য *fdt, শূন্য *buf, পূর্ণাঙ্ক bufsize)
{
	যদি (!can_assume(VALID_প্রবেশ) && bufsize < 0)
		প্রত্যাগম -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	যদি (fdt_totalsize(fdt) > (unsigned পূর্ণাঙ্ক)bufsize)
		প্রত্যাগম -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	প্রত্যাগম 0;
}
