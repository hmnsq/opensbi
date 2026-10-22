// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাৱ "libfdt_env.h"

#অন্তর্ভাৱ <fdt.h>
#অন্তর্ভাৱ <libfdt.h>

#অন্তর্ভাৱ "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(ধ্রুৱ শূন্য *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	যদি (can_assume(VALID_DTB))
		ঘূৰাই totalsize;

	/* The device tree must be at an 8-byte aligned address */
	যদি ((uintptr_t)fdt & 7)
		ঘূৰাই -FDT_ERR_ALIGNMENT;

	যদি (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		যদি (!can_assume(LATEST)) {
			যদি (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ঘূৰাই -FDT_ERR_BADVERSION;
			যদি (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ঘূৰাই -FDT_ERR_BADVERSION;
		}
	} অন্যথা যদি (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		যদি (!can_assume(VALID_প্ৰবেশ) && fdt_size_dt_struct(fdt) == 0)
			ঘূৰাই -FDT_ERR_BADSTATE;
	} অন্যথা {
		ঘূৰাই -FDT_ERR_BADMAGIC;
	}

	যদি (totalsize < INT32_MAX)
		ঘূৰাই totalsize;
	অন্যথা
		ঘূৰাই -FDT_ERR_TRUNCATED;
}

স্থির পূর্ণাঙ্ক check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ঘূৰাই (off >= hdrsize) && (off <= totalsize);
}

স্থির পূর্ণাঙ্ক check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	যদি (!check_off_(hdrsize, totalsize, base))
		ঘূৰাই 0; /* block start out of bounds */
	যদি ((base + size) < base)
		ঘূৰাই 0; /* overflow */
	যদি (!check_off_(hdrsize, totalsize, base + size))
		ঘূৰাই 0; /* block end out of bounds */
	ঘূৰাই 1;
}

size_t fdt_header_size_(uint32_t version)
{
	যদি (version <= 1)
		ঘূৰাই FDT_V1_SIZE;
	অন্যথা যদি (version <= 2)
		ঘূৰাই FDT_V2_SIZE;
	অন্যথা যদি (version <= 3)
		ঘূৰাই FDT_V3_SIZE;
	অন্যথা যদি (version <= 16)
		ঘূৰাই FDT_V16_SIZE;
	অন্যথা
		ঘূৰাই FDT_V17_SIZE;
}

size_t fdt_header_size(ধ্রুৱ শূন্য *fdt)
{
	ঘূৰাই can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

পূর্ণাঙ্ক fdt_check_header(ধ্রুৱ শূন্য *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	যদি ((uintptr_t)fdt & 7)
		ঘূৰাই -FDT_ERR_ALIGNMENT;

	যদি (fdt_magic(fdt) != FDT_MAGIC)
		ঘূৰাই -FDT_ERR_BADMAGIC;
	যদি (!can_assume(LATEST)) {
		যদি ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ঘূৰাই -FDT_ERR_BADVERSION;
		যদি (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ঘূৰাই -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	যদি (!can_assume(VALID_DTB)) {

		যদি ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ঘূৰাই -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		যদি (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ঘূৰাই -FDT_ERR_TRUNCATED;
	}

	যদি (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		যদি (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			যদি (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ঘূৰাই -FDT_ERR_TRUNCATED;
		} অন্যথা {
			যদি (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ঘূৰাই -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		যদি (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ঘূৰাই -FDT_ERR_TRUNCATED;
	}

	ঘূৰাই 0;
}

ধ্রুৱ শূন্য *fdt_offset_ptr(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক offset, unsigned পূর্ণাঙ্ক len)
{
	unsigned পূর্ণাঙ্ক uoffset = offset;
	unsigned পূর্ণাঙ্ক absoffset = offset + fdt_off_dt_struct(fdt);

	যদি (offset < 0)
		ঘূৰাই রিক্ত;

	যদি (!can_assume(VALID_প্ৰবেশ))
		যদি ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ঘূৰাই রিক্ত;

	যদি (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		যদি (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ঘূৰাই রিক্ত;

	ঘূৰাই fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক startoffset, পূর্ণাঙ্ক *nextoffset)
{
	ধ্রুৱ fdt32_t *tagp, *lenp;
	uint32_t tag;
	পূর্ণাঙ্ক offset = startoffset;
	ধ্রুৱ ৱর্ণ *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	যদি (!can_assume(VALID_DTB) && !tagp)
		ঘূৰাই FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	ৱিকল্প (tag) {
	পক্ষ FDT_BEGIN_গ্ৰন্থি:
		/* skip name */
		কৰা {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} যেতিয়ালৈ (p && (*p != '\0'));
		যদি (!can_assume(VALID_DTB) && !p)
			ঘূৰাই FDT_END; /* premature end */
		ভঙ্গ;

	পক্ষ FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, পরিমাণ(*lenp));
		যদি (!can_assume(VALID_DTB) && !lenp)
			ঘূৰাই FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += পরিমাণ(সংরচনা fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		যদি (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		ভঙ্গ;

	পক্ষ FDT_END:
	পক্ষ FDT_END_গ্ৰন্থি:
	পক্ষ FDT_NOP:
		ভঙ্গ;

	স্ৱভাৱ:
		ঘূৰাই FDT_END;
	}

	যদি (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ঘূৰাই FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ঘূৰাই tag;
}

পূর্ণাঙ্ক fdt_check_গ্ৰন্থি_offset(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	যদি (!can_assume(VALID_প্ৰবেশ)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ঘূৰাই -FDT_ERR_BADOFFSET;

	যদি (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_গ্ৰন্থি)
		ঘূৰাই -FDT_ERR_BADOFFSET;

	ঘূৰাই offset;
}

পূর্ণাঙ্ক fdt_check_prop_offset_(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	যদি (!can_assume(VALID_প্ৰবেশ)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ঘূৰাই -FDT_ERR_BADOFFSET;

	যদি (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ঘূৰাই -FDT_ERR_BADOFFSET;

	ঘূৰাই offset;
}

পূর্ণাঙ্ক fdt_next_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক offset, পূর্ণাঙ্ক *depth)
{
	পূর্ণাঙ্ক nextoffset = 0;
	uint32_t tag;

	যদি (offset >= 0)
		যদি ((nextoffset = fdt_check_গ্ৰন্থি_offset(fdt, offset)) < 0)
			ঘূৰাই nextoffset;

	কৰা {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ৱিকল্প (tag) {
		পক্ষ FDT_PROP:
		পক্ষ FDT_NOP:
			ভঙ্গ;

		পক্ষ FDT_BEGIN_গ্ৰন্থি:
			যদি (depth)
				(*depth)++;
			ভঙ্গ;

		পক্ষ FDT_END_গ্ৰন্থি:
			যদি (depth && ((--(*depth)) < 0))
				ঘূৰাই nextoffset;
			ভঙ্গ;

		পক্ষ FDT_END:
			যদি ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ঘূৰাই -FDT_ERR_NOTFOUND;
			অন্যথা
				ঘূৰাই nextoffset;
		}
	} যেতিয়ালৈ (tag != FDT_BEGIN_গ্ৰন্থি);

	ঘূৰাই offset;
}

পূর্ণাঙ্ক fdt_first_subnode(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	পূর্ণাঙ্ক depth = 0;

	offset = fdt_next_গ্ৰন্থি(fdt, offset, &depth);
	যদি (offset < 0 || depth != 1)
		ঘূৰাই -FDT_ERR_NOTFOUND;

	ঘূৰাই offset;
}

পূর্ণাঙ্ক fdt_next_subnode(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	পূর্ণাঙ্ক depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	কৰা {
		offset = fdt_next_গ্ৰন্থি(fdt, offset, &depth);
		যদি (offset < 0 || depth < 1)
			ঘূৰাই -FDT_ERR_NOTFOUND;
	} যেতিয়ালৈ (depth > 1);

	ঘূৰাই offset;
}

ধ্রুৱ ৱর্ণ *fdt_find_string_(ধ্রুৱ ৱর্ণ *strtab, পূর্ণাঙ্ক tabsize, ধ্রুৱ ৱর্ণ *s)
{
	পূর্ণাঙ্ক len = strlen(s) + 1;
	ধ্রুৱ ৱর্ণ *last = strtab + tabsize - len;
	ধ্রুৱ ৱর্ণ *p;

	বাবে (p = strtab; p <= last; p++)
		যদি (memcmp(p, s, len) == 0)
			ঘূৰাই p;
	ঘূৰাই রিক্ত;
}

পূর্ণাঙ্ক fdt_move(ধ্রুৱ শূন্য *fdt, শূন্য *buf, পূর্ণাঙ্ক bufsize)
{
	যদি (!can_assume(VALID_প্ৰবেশ) && bufsize < 0)
		ঘূৰাই -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	যদি (fdt_totalsize(fdt) > (unsigned পূর্ণাঙ্ক)bufsize)
		ঘূৰাই -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ঘূৰাই 0;
}
