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
int32_t fdt_ro_probe_(ধ্রুবক শূন্য *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	যদি (can_assume(VALID_DTB))
		ফেরত totalsize;

	/* The device tree must be at an 8-byte aligned address */
	যদি ((uintptr_t)fdt & 7)
		ফেরত -FDT_ERR_ALIGNMENT;

	যদি (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		যদি (!can_assume(LATEST)) {
			যদি (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ফেরত -FDT_ERR_BADVERSION;
			যদি (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ফেরত -FDT_ERR_BADVERSION;
		}
	} অন্যথা যদি (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		যদি (!can_assume(VALID_প্রবেশ) && fdt_আকার_dt_struct(fdt) == 0)
			ফেরত -FDT_ERR_BADSTATE;
	} অন্যথা {
		ফেরত -FDT_ERR_BADMAGIC;
	}

	যদি (totalsize < INT32_MAX)
		ফেরত totalsize;
	অন্যথা
		ফেরত -FDT_ERR_TRUNCATED;
}

স্থির পূর্ণসংখ্যা check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ফেরত (off >= hdrsize) && (off <= totalsize);
}

স্থির পূর্ণসংখ্যা check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t আকার_2)
{
	যদি (!check_off_(hdrsize, totalsize, base))
		ফেরত 0; /* block start out of bounds */
	যদি ((base + আকার_2) < base)
		ফেরত 0; /* overflow */
	যদি (!check_off_(hdrsize, totalsize, base + আকার_2))
		ফেরত 0; /* block end out of bounds */
	ফেরত 1;
}

size_t fdt_header_আকার(uint32_t version)
{
	যদি (version <= 1)
		ফেরত FDT_V1_আকার;
	অন্যথা যদি (version <= 2)
		ফেরত FDT_V2_আকার;
	অন্যথা যদি (version <= 3)
		ফেরত FDT_V3_আকার;
	অন্যথা যদি (version <= 16)
		ফেরত FDT_V16_আকার;
	অন্যথা
		ফেরত FDT_V17_আকার;
}

size_t fdt_header_আকার_2(ধ্রুবক শূন্য *fdt)
{
	ফেরত can_assume(LATEST) ? FDT_V17_আকার :
		fdt_header_আকার(fdt_version(fdt));
}

পূর্ণসংখ্যা fdt_check_header(ধ্রুবক শূন্য *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	যদি ((uintptr_t)fdt & 7)
		ফেরত -FDT_ERR_ALIGNMENT;

	যদি (fdt_magic(fdt) != FDT_MAGIC)
		ফেরত -FDT_ERR_BADMAGIC;
	যদি (!can_assume(LATEST)) {
		যদি ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ফেরত -FDT_ERR_BADVERSION;
		যদি (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ফেরত -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_আকার_2(fdt);
	যদি (!can_assume(VALID_DTB)) {

		যদি ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ফেরত -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		যদি (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ফেরত -FDT_ERR_TRUNCATED;
	}

	যদি (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		যদি (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			যদি (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ফেরত -FDT_ERR_TRUNCATED;
		} অন্যথা {
			যদি (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_আকার_dt_struct(fdt)))
				ফেরত -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		যদি (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_আকার_dt_strings(fdt)))
			ফেরত -FDT_ERR_TRUNCATED;
	}

	ফেরত 0;
}

ধ্রুবক শূন্য *fdt_offset_ptr(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা offset, unsigned পূর্ণসংখ্যা len)
{
	unsigned পূর্ণসংখ্যা uoffset = offset;
	unsigned পূর্ণসংখ্যা absoffset = offset + fdt_off_dt_struct(fdt);

	যদি (offset < 0)
		ফেরত ফাঁকা;

	যদি (!can_assume(VALID_প্রবেশ))
		যদি ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ফেরত ফাঁকা;

	যদি (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		যদি (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_আকার_dt_struct(fdt)))
			ফেরত ফাঁকা;

	ফেরত fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_পরবর্তী_tag(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা startoffset, পূর্ণসংখ্যা *nextoffset)
{
	ধ্রুবক fdt32_t *tagp, *lenp;
	uint32_t tag;
	পূর্ণসংখ্যা offset = startoffset;
	ধ্রুবক অক্ষর *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	যদি (!can_assume(VALID_DTB) && !tagp)
		ফেরত FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	নির্বাচন (tag) {
	ক্ষেত্র FDT_BEGIN_গ্রন্থি:
		/* skip name */
		করো {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} যখন (p && (*p != '\0'));
		যদি (!can_assume(VALID_DTB) && !p)
			ফেরত FDT_END; /* premature end */
		বিরতি;

	ক্ষেত্র FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, আকার(*lenp));
		যদি (!can_assume(VALID_DTB) && !lenp)
			ফেরত FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += আকার(কাঠামো fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		যদি (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		বিরতি;

	ক্ষেত্র FDT_END:
	ক্ষেত্র FDT_END_গ্রন্থি:
	ক্ষেত্র FDT_NOP:
		বিরতি;

	পূর্বনির্ধারিত:
		ফেরত FDT_END;
	}

	যদি (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ফেরত FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ফেরত tag;
}

পূর্ণসংখ্যা fdt_check_গ্রন্থি_offset(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা offset)
{
	যদি (!can_assume(VALID_প্রবেশ)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ফেরত -FDT_ERR_BADOFFSET;

	যদি (fdt_পরবর্তী_tag(fdt, offset, &offset) != FDT_BEGIN_গ্রন্থি)
		ফেরত -FDT_ERR_BADOFFSET;

	ফেরত offset;
}

পূর্ণসংখ্যা fdt_check_prop_offset_(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা offset)
{
	যদি (!can_assume(VALID_প্রবেশ)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ফেরত -FDT_ERR_BADOFFSET;

	যদি (fdt_পরবর্তী_tag(fdt, offset, &offset) != FDT_PROP)
		ফেরত -FDT_ERR_BADOFFSET;

	ফেরত offset;
}

পূর্ণসংখ্যা fdt_পরবর্তী_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা offset, পূর্ণসংখ্যা *depth)
{
	পূর্ণসংখ্যা nextoffset = 0;
	uint32_t tag;

	যদি (offset >= 0)
		যদি ((nextoffset = fdt_check_গ্রন্থি_offset(fdt, offset)) < 0)
			ফেরত nextoffset;

	করো {
		offset = nextoffset;
		tag = fdt_পরবর্তী_tag(fdt, offset, &nextoffset);

		নির্বাচন (tag) {
		ক্ষেত্র FDT_PROP:
		ক্ষেত্র FDT_NOP:
			বিরতি;

		ক্ষেত্র FDT_BEGIN_গ্রন্থি:
			যদি (depth)
				(*depth)++;
			বিরতি;

		ক্ষেত্র FDT_END_গ্রন্থি:
			যদি (depth && ((--(*depth)) < 0))
				ফেরত nextoffset;
			বিরতি;

		ক্ষেত্র FDT_END:
			যদি ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ফেরত -FDT_ERR_NOTFOUND;
			অন্যথা
				ফেরত nextoffset;
		}
	} যখন (tag != FDT_BEGIN_গ্রন্থি);

	ফেরত offset;
}

পূর্ণসংখ্যা fdt_first_subnode(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা offset)
{
	পূর্ণসংখ্যা depth = 0;

	offset = fdt_পরবর্তী_গ্রন্থি(fdt, offset, &depth);
	যদি (offset < 0 || depth != 1)
		ফেরত -FDT_ERR_NOTFOUND;

	ফেরত offset;
}

পূর্ণসংখ্যা fdt_পরবর্তী_subnode(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা offset)
{
	পূর্ণসংখ্যা depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	করো {
		offset = fdt_পরবর্তী_গ্রন্থি(fdt, offset, &depth);
		যদি (offset < 0 || depth < 1)
			ফেরত -FDT_ERR_NOTFOUND;
	} যখন (depth > 1);

	ফেরত offset;
}

ধ্রুবক অক্ষর *fdt_find_string_(ধ্রুবক অক্ষর *strtab, পূর্ণসংখ্যা tabsize, ধ্রুবক অক্ষর *s)
{
	পূর্ণসংখ্যা len = strlen(s) + 1;
	ধ্রুবক অক্ষর *last = strtab + tabsize - len;
	ধ্রুবক অক্ষর *p;

	জন্য (p = strtab; p <= last; p++)
		যদি (memcmp(p, s, len) == 0)
			ফেরত p;
	ফেরত ফাঁকা;
}

পূর্ণসংখ্যা fdt_move(ধ্রুবক শূন্য *fdt, শূন্য *buf, পূর্ণসংখ্যা bufsize)
{
	যদি (!can_assume(VALID_প্রবেশ) && bufsize < 0)
		ফেরত -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	যদি (fdt_totalsize(fdt) > (unsigned পূর্ণসংখ্যা)bufsize)
		ফেরত -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ফেরত 0;
}
